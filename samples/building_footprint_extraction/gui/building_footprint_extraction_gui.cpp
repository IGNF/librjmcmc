// parameters
#include "param/wx_parameter.hpp"
typedef parameters< wx_parameter > param;
#include "core/building_footprint_extraction_parameters_inc.hpp"

// images
#include "image/image_types.hpp"

// optimization
#include "core/building_footprint_extraction.hpp"
#include "simulated_annealing/simulated_annealing.hpp"

// visitors
#include "simulated_annealing/visitor/all.hpp"
#include "simulated_annealing/visitor/wx/all.hpp"

// template instanciations
#include "core/global_reconstruction_unary_energy_inc.hpp"
#include "image/oriented_inc.hpp"
#include "image/gradient_functor_inc.hpp"

// threading and wx
#include <boost/thread.hpp>
#include <wx/app.h>
#include "simulated_annealing/visitor/wx/resources/IGN.xpm"

class building_footprint_rectangle_gui : public wxApp, public Controler
{ 
    typedef simulated_annealing::composite_visitor<
            simulated_annealing::wx::log_visitor*,
            simulated_annealing::wx::configuration_visitor*,
            simulated_annealing::wx::parameters_visitor*,
            simulated_annealing::wx::chart_visitor*
            > visitor;

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
        m_confg_visitor = new simulated_annealing::wx::configuration_visitor((wxFrame *)NULL, wxID_ANY, _("librjmcmc: rectangular building footprint extraction"), wxDefaultPosition, wxSize(600,600) );
        m_confg_visitor->SetIcon(wxICON(IGN));
        m_param_visitor = new simulated_annealing::wx::parameters_visitor(m_confg_visitor);
        m_chart_visitor = new simulated_annealing::wx::chart_visitor(m_confg_visitor);
        m_visitor = new visitor(&m_log_visitor,m_confg_visitor,m_param_visitor,m_chart_visitor);
        m_confg_visitor->controler(this);
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

        gradient_functor gf(p->get<double>("sigmaD"));
        oriented_gradient_view grad_view(dsm_file,  bbox, gf);
        oriented_ndvi_view     ndvi_view(ndvi_file, bbox);

        m_confg_visitor->add_layer(dsm_file);
        m_confg_visitor->add_layer(ndvi_file);
        set_bbox(p,bbox);
        wxPoint p0(wxCoord(bbox.min().x()),wxCoord(bbox.min().y()));
        wxPoint p1(wxCoord(bbox.max().x()),wxCoord(bbox.max().y()));
        m_confg_visitor->set_bbox(wxRect(p0,p1));

        init_visitor        (p,*m_visitor);
        create_configuration(p,grad_view,ndvi_view,m_config);
        create_sampler      (p,m_sampler);
        create_schedule     (p,m_schedule);
        create_end_test     (p,m_end_test);

        m_thread = new boost::thread(
                simulated_annealing::optimize<configuration,sampler,schedule,end_test,visitor>,
                boost::ref(*m_config), boost::ref(*m_sampler),
                boost::ref(*m_schedule), boost::ref(*m_end_test),
                boost::ref(*m_visitor) );
    }

    virtual void stop() { m_end_test->stop(); }

    virtual void param_visibility(bool b) { m_param_visitor->Show(b); }
    virtual void chart_visibility(bool b) { m_chart_visitor->Show(b); }
    virtual bool param_visibility() const { return m_param_visitor->IsShown(); }
    virtual bool chart_visibility() const { return m_chart_visitor->IsShown(); }

    building_footprint_rectangle_gui() :
            m_config(NULL), m_sampler(NULL), m_schedule(NULL), m_end_test(NULL),
            m_visitor(NULL), m_thread(NULL),
            m_confg_visitor(NULL), m_param_visitor(NULL), m_chart_visitor(NULL)
    {
    }

    virtual ~building_footprint_rectangle_gui() {
        release();
        /*
        if(m_confg_visitor) { m_confg_visitor->Close(); }
        if(m_param_visitor) { m_param_visitor->Close(); }
        if(m_chart_visitor) { m_chart_visitor->Close(); }
        */
        if(m_visitor    ) { delete m_visitor; }
    }
private:
    void release()
    {
        if(m_end_test){ m_end_test->stop(); }
        if(m_thread)  { m_thread->join(); delete m_thread; m_thread=NULL; }
        if(m_end_test){ delete m_end_test;m_end_test=NULL; }
        if(m_config)  { delete m_config;  m_config  =NULL; }
        if(m_schedule){ delete m_schedule;m_schedule=NULL; }
        if(m_sampler) { delete m_sampler; m_sampler =NULL; }
    }

    configuration *m_config;
    sampler *m_sampler;
    schedule *m_schedule;
    end_test *m_end_test;
    visitor *m_visitor;
    boost::thread *m_thread;

    simulated_annealing::wx::configuration_visitor *m_confg_visitor;
    simulated_annealing::wx::parameters_visitor    *m_param_visitor;
    simulated_annealing::wx::chart_visitor         *m_chart_visitor;
    simulated_annealing::wx::log_visitor            m_log_visitor;
};

IMPLEMENT_APP(building_footprint_rectangle_gui);

