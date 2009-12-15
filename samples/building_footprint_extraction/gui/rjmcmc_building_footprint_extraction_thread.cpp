#include "rjmcmc_building_footprint_extraction_thread.hpp"

Layer::ptrLayerType& operator<<(Layer::ptrLayerType& layer, const building_configuration& config) {
	layer->Clear();
	building_configuration::const_iterator it = config.begin(), end = config.end();
	for (; it != end; ++it)
	{
		building_configuration::value_type r = config[it];
		std::vector<double> vectx, vecty;
		for (unsigned int i=0; i<5;++i)
		{
			vectx.push_back(CGAL::to_double(r[i].x()));
			vecty.push_back(CGAL::to_double(r[i].y()));
		}
		layer->AddPolygon(vectx, vecty );
		std::ostringstream oss;
		oss << config.energy(it);
		layer->AddText( CGAL::to_double(r.center().x()) , CGAL::to_double(r.center().y()) , oss.str() , wxColour(255,0,0) );
	}
	return layer;
}


rjmcmc_building_footprint_extraction_thread::rjmcmc_building_footprint_extraction_thread(Layer::ptrLayerType layer, 
		rjmcmc_building_footprint_extraction_frame *frame) :
		wxThread(wxTHREAD_JOINABLE), m_layer(layer), m_frame(frame), m_out(""), m_visitor(m_out) {}


void rjmcmc_building_footprint_extraction_thread::begin(unsigned int dump, unsigned int save) {
	m_dump = dump;
	m_visitor.begin(dump,save);
	wxMutexGuiEnter();
	{
		wxLogMessage( wxString( m_out.str().c_str(),*wxConvCurrent ).GetData());
	}
	wxMutexGuiLeave();
	m_out.str("");
}

bool rjmcmc_building_footprint_extraction_thread::iterate(unsigned int i, double t, const building_configuration& configuration, const building_sampler& sampler) {
	if(!m_visitor.iterate(i,t,configuration,sampler)) return false;
/*
	if ( i % m_save == 0 && m_save != 0 )
	{
		std::ostringstream out;
		out.width(8);
		out.fill('0');
		out << i;
		m_layer->Save(out.str() + ".shp");
	}
*/	if (i % m_dump == 0 && m_dump!=0)
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

void rjmcmc_building_footprint_extraction_thread::end(const building_configuration& configuration) {
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

