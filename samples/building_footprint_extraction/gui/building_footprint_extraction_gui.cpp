// parameters
#include "param/wx_parameter.hpp"
typedef parameters< wx_parameter > param;
#include "core/building_footprint_extraction_parameters_inc.hpp"

// images
#include "image/image_types.hpp"

// optimization
#include "core/building_footprint_extraction.hpp"
#include "rjmcmc/simulated_annealing.hpp"

// visitors
#include "visitor/wx/wx_log_visitor.hpp"
#include "visitor/wx/configuration_frame.hpp"
#include "visitor/wx/chart_frame.hpp"
#include "visitor/wx/wx_parameter_frame.hpp"
#include "visitor/composite_visitor.hpp"

// template instanciations
#include "core/global_reconstruction_unary_energy_inc.hpp"
#include "image/gradient_image_inc.hpp"
#include "image/image_inc.hpp"

// threading and wx
#include <boost/thread.hpp>
#include <wx/app.h>
#include "resources/IGN.xpm"

class building_footprint_rectangle_gui : public wxApp, public Controler
{ 
    typedef composite_visitor<wx_log_visitor*,configuration_frame*,parameters_frame*,chart_frame*> visitor;

private:
    bool OnInit()
    {
        char **my_argv = new char*[argc];
        for (int i=0;i<argc;++i)
        {
            wxString mystr(argv[i]);
            my_argv[i] = new char[mystr.size()+1];
            strcpy(my_argv[i], mystr.To8BitData());
        }
        param *p = param::instance();
        initialize_parameters(p);
        if(!p->parse(argc,my_argv)) return false;
        m_confg_frame = new configuration_frame((wxFrame *)NULL, wxID_ANY, _("librjmcmc: building/tree footprint extraction") );
        m_confg_frame->SetIcon(wxICON(IGN));
        m_param_frame = new parameters_frame(m_confg_frame);
        m_chart_frame = new chart_frame(m_confg_frame);
        m_visitor = new visitor(&m_wx_log_visitor,m_confg_frame,m_param_frame,m_chart_frame);
        m_confg_frame->controler(this);
        return true;
    }

public:
    virtual void go() {
        release();
        param *p = param::instance();
		update_values(p);

	Iso_rectangle_2 bbox = get_bbox(p);
	std::string  dsm_file = p->get<boost::filesystem::path>("dsm" ).string();
	std::string ndvi_file = p->get<boost::filesystem::path>("ndvi").string();
	clip_bbox(bbox,dsm_file );
	clip_bbox(bbox,ndvi_file);

	oriented_gradient_view grad_view;
	gradient_view(grad_view, m_grad, dsm_file, bbox, p->get<double>("sigmaD"));

	oriented_ndvi_view ndvi_view;
	load_view(ndvi_view, m_ndvi, ndvi_file, bbox);

		m_confg_frame->add_layer(dsm_file);
		m_confg_frame->add_layer(ndvi_file);
		set_bbox(p,bbox);
		wxPoint p0(wxCoord(bbox.min().x()),wxCoord(bbox.min().y()));
		wxPoint p1(wxCoord(bbox.max().x()),wxCoord(bbox.max().y()));
		m_confg_frame->set_bbox(wxRect(p0,p1));

        init_visitor        (p,*m_visitor);
        create_configuration(p,grad_view,ndvi_view,m_config);
        create_sampler      (p,m_sampler);
        create_temperature  (p,m_temp);
        create_end_test     (p,m_end_test);

        m_thread = new boost::thread(
                simulated_annealing<configuration,sampler,temperature,end_test,visitor>,
                boost::ref(*m_config), boost::ref(*m_sampler),
                boost::ref(*m_temp),   boost::ref(*m_end_test),
                boost::ref(*m_visitor) );
    }

    virtual void stop() { m_end_test->stop(); }

    virtual void param_visibility(bool b) { m_param_frame->Show(b); }
    virtual void chart_visibility(bool b) { m_chart_frame->Show(b); }
    virtual bool param_visibility() const { return m_param_frame->IsShown(); }
    virtual bool chart_visibility() const { return m_chart_frame->IsShown(); }

    building_footprint_rectangle_gui() :
        m_config(NULL), m_sampler(NULL), m_temp(NULL), m_end_test(NULL), 
        m_visitor(NULL), m_thread(NULL),
		m_confg_frame(NULL), m_param_frame(NULL), m_chart_frame(NULL)
    {
    }

    virtual ~building_footprint_rectangle_gui() {
        release();
        /*
        if(m_confg_frame) { m_confg_frame->Close(); }
        if(m_param_frame) { m_param_frame->Close(); }
        if(m_chart_frame) { m_chart_frame->Close(); }
        */
        if(m_visitor    ) { delete m_visitor; }
    }
private:
    void release()
    {
        if(m_end_test){ m_end_test->stop(); }
        if(m_thread)  { m_thread->join(); delete m_thread; m_thread=NULL; }
        if(m_end_test){ delete m_end_test;m_end_test=NULL; }
        if(m_config)  { delete m_config;  m_config =NULL; }
        if(m_temp)    { delete m_temp;    m_temp   =NULL; }
        if(m_sampler) { delete m_sampler; m_sampler=NULL; }
    }

    configuration *m_config;
    sampler *m_sampler;
    temperature *m_temp;
    end_test *m_end_test;
    visitor *m_visitor;
    boost::thread *m_thread;

    configuration_frame *m_confg_frame;
    parameters_frame    *m_param_frame;
    chart_frame         *m_chart_frame;
    wx_log_visitor       m_wx_log_visitor;
	gradient_image_t     m_grad;
	ndvi_image_t		 m_ndvi;
};

IMPLEMENT_APP(building_footprint_rectangle_gui);

