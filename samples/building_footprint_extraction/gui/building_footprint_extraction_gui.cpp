#include "param/wx_parameter_traits.hpp"
#include "visitor/wx/wx_parameter_frame.hpp"
#include "visitor/wx/wx_parameter_frame.cpp"
#include "core/building_footprint_extraction_parameters_inc.hpp"
#include "visitor/wx/configuration_frame.hpp"
#include "visitor/wx/configuration_frame.cpp"
#include "visitor/wx/chart_frame.hpp"
#include "visitor/wx/chart_frame.cpp"
#include "visitor/wx/wx_log_visitor.hpp"
#include <boost/thread.hpp>

#include <GilViewer/layers/ImageLayer.hpp>
#include <GilViewer/layers/image_types.hpp>

#include "core/building_footprint_extraction.hpp"

// member function instanciations
#include "energy/image_gradient_unary_energy_inc.hpp"
#include "core/global_reconstruction_unary_energy_inc.hpp"
#include "image/gradient_image_inc.hpp"
#include "image/image_inc.hpp"

template<typename Configuration, typename Sampler>
class wx_visitor
{
public:
	void begin(int dump, int save, double t, const Configuration& config)
	{
		m_stopping = false;
		m_wx_log_visitor.begin(dump,save,t,config);
		m_confg_frame->begin(dump,save,t,config);
		m_chart_frame->begin(dump,save,t,config);
		m_param_frame->begin(dump,save,t,config);
	}

	bool iterate(unsigned int i, double t, const Configuration& config, const Sampler& sample)
	{
		bool b = !m_stopping;
		b = m_wx_log_visitor.iterate(i,t,config,sample) && b;
		b = m_confg_frame->iterate(i,t,config,sample) && b;
		b = m_chart_frame->iterate(i,t,config,sample) && b;
		b = m_param_frame->iterate(i,t,config,sample) && b;
		return b;
	}

	void end(const Configuration& config)
	{
		m_wx_log_visitor.end(config);
		m_confg_frame->end(config);
		m_chart_frame->end(config);
		m_param_frame->end(config);
	}

	void toggle_param_visibility() { m_param_frame->Show(!m_param_frame->IsShown()); }
	void toggle_chart_visibility() { m_chart_frame->Show(!m_chart_frame->IsShown()); }
	void stop() {
		m_stopping = true;
	}

	void controler(Controler *c) {
		m_confg_frame->controler(c);
	}

	wx_visitor()
	{
		m_confg_frame = new rjmcmc_2d_viewer_frame((wxFrame *)NULL, wxID_ANY, _("RJMCMC building footprint extraction") );
		m_param_frame = new parameters_frame(m_confg_frame);
		m_chart_frame = new chart_frame(m_confg_frame);

		m_confg_frame->Show();
		m_param_frame->Show();
		m_chart_frame->Show();

		m_stopping = false;
	}

	~wx_visitor() {
		m_confg_frame->Destroy();
		m_param_frame->Destroy();
		m_chart_frame->Destroy();
	}
private:
        rjmcmc_2d_viewer_frame  *m_confg_frame;
        parameters_frame        *m_param_frame;
        chart_frame             *m_chart_frame;
        wx_log_visitor<Sampler> m_wx_log_visitor;
        bool                    m_stopping;
};

template<typename Configuration, typename Sampler>
class wx_controler : public Controler
{
public:
	typedef wx_visitor<Configuration,Sampler> visitor_type;

	virtual void go(void *layer) {
		m_visitor.stop();
		if(m_thread) m_thread->join();
		Layer::ptrLayerType *l = (Layer::ptrLayerType *)layer;
		ImageLayer *i = dynamic_cast<ImageLayer*>(&**l); // not so nice...
		m_thread = new boost::thread(
			building_footprint_extraction<visitor_type,any_view_type>,
			boost::ref(m_visitor),
			boost::ref(i->View()->value)
		);
	}

	virtual void toggle_param_visibility() { m_visitor.toggle_param_visibility(); }
	virtual void toggle_chart_visibility() { m_visitor.toggle_chart_visibility(); }
	virtual void stop() { m_visitor.stop(); }

	wx_controler() : m_thread(NULL) { m_visitor.controler(this); }

	virtual ~wx_controler() {
		if(m_thread) delete m_thread;
	}
private:
	visitor_type m_visitor;
	boost::thread *m_thread;
};

#include "core/building_footprint_extraction.hpp"
#include <wx/app.h>

class building_footprint_extraction_gui : public wxApp
{
private:
	typedef wx_controler<configuration,sampler> controler_type;
	bool OnInit()
	{
		char **my_argv = new char*[argc];
		for (int i=0;i<argc;++i)
		{
			wxString mystr(argv[i]);
			my_argv[i] = new char[mystr.size()+1];
			strcpy(my_argv[i], mystr.To8BitData());
		}
		if(!param::Instance()->parse(argc,my_argv)) return false;
		m_controler = new controler_type();
		return true;
	}
	controler_type *m_controler;
};

IMPLEMENT_APP(building_footprint_extraction_gui);
