#include "rjmcmc_building_footprint_extraction_thread.hpp"

struct layer_visitor {
public:
  layer_visitor( Layer::ptrLayerType& layer) : m_layer(layer) {}

  void energy(double e) { m_energy=e; }
  typedef void result_type;
  void operator()(const Rectangle_2& r) const {
    std::vector<double> vectx, vecty;
    for (unsigned int i=0; i<5;++i)
    {
      vectx.push_back(CGAL::to_double(r[i].x()));
      vecty.push_back(CGAL::to_double(r[i].y()));
    }
    m_layer->AddPolygon(vectx, vecty );
    std::ostringstream oss;
    oss << m_energy;
    double x = CGAL::to_double(r.center().x());
    double y = CGAL::to_double(r.center().y());
    m_layer->AddText(x, y, oss.str() , wxColour(255,0,0) );
  }
  void operator()(const Circle_2& c) const {
    double x = CGAL::to_double(c.center().x());
    double y = CGAL::to_double(c.center().y());
    double r = CGAL::to_double(c.radius());
    m_layer->AddCircle(x,y,r);
    std::ostringstream oss;
    oss << m_energy;
    m_layer->AddText(x, y, oss.str() , wxColour(255,0,0) );
  }
private:
  Layer::ptrLayerType& m_layer;
  double m_energy;
};

Layer::ptrLayerType& operator<<(Layer::ptrLayerType& layer, const building_configuration& config)
{
	layer->Clear();
	building_configuration::const_iterator it = config.begin(), end = config.end();
	layer_visitor visitor(layer);
	for (; it != end; ++it)
	{
		visitor.energy(config.energy(it));
		rjmcmc::apply_visitor(visitor,config[it]);
	}
	return layer;
}


rjmcmc_building_footprint_extraction_thread::rjmcmc_building_footprint_extraction_thread(Layer::ptrLayerType layer, 
		rjmcmc_building_footprint_extraction_frame *frame) :
		wxThread(wxTHREAD_JOINABLE), m_layer(layer), m_frame(frame), m_out(""), m_visitor(m_out) {}


void rjmcmc_building_footprint_extraction_thread::begin(unsigned int dump, unsigned int save)
{
	m_dump = dump;
	m_visitor.begin(dump,save);
	wxMutexGuiEnter();
	{
		wxLogMessage( wxString( m_out.str().c_str(),*wxConvCurrent ).GetData());
	}
	wxMutexGuiLeave();
	m_out.str("");
}

bool rjmcmc_building_footprint_extraction_thread::iterate(unsigned int i, double t, const building_configuration& configuration, const building_sampler& sampler)
{
	if(!m_visitor.iterate(i,t,configuration,sampler))
		return false;
	
	if ( building_footprint_extraction_parameters::Instance()->m_do_save )
	{
		if ( i%building_footprint_extraction_parameters::Instance()->m_nb_iterations_save == 0 && building_footprint_extraction_parameters::Instance()->m_nb_iterations_save != 0 )
		{
			std::ostringstream out;
			out.width(8);
			out.fill('0');
			out << i;
			m_layer->Save( out.str() + ".shp" );
		}
	}
	if ( i%building_footprint_extraction_parameters::Instance()->m_nb_iterations_dump == 0 && building_footprint_extraction_parameters::Instance()->m_nb_iterations_dump != 0 )
	{
		wxMutexGuiEnter();
		{
			m_layer << configuration;
			m_frame->Refresh();
			wxLogMessage( wxString( m_out.str().c_str(),*wxConvCurrent ).GetData());
		}
		wxMutexGuiLeave();
		m_out.str("");	
		if ( TestDestroy() )
		{
			wxLogMessage( _("Computation stopped") );
			return false;
		}
	}
	return true;
}

void rjmcmc_building_footprint_extraction_thread::end(const building_configuration& configuration)
{
	m_visitor.end(configuration);
	wxMutexGuiEnter();
	{
		wxLogMessage( wxString( m_out.str().c_str(),*wxConvCurrent ).GetData());
		m_frame->OnThreadEnd();
	}
	wxMutexGuiLeave();
	m_out.str("");
}

void* rjmcmc_building_footprint_extraction_thread::Entry()
{
	rjmcmc_building_footprint_extraction(*this);
	return NULL;
}

