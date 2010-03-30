#ifndef RJMCMC_BUILDINGSDETECTIONFRAME_HPP_
#define RJMCMC_BUILDINGSDETECTIONFRAME_HPP_

#include <wx/wx.h>
#include <wx/statusbr.h>

#include <GilViewer/gui/LayerControl.hpp>
#include <GilViewer/gui/BasicViewerFrame.h>
#include <GilViewer/convenient/MacrosGilViewer.hpp>

#include "controler.hpp"
#include "rjmcmc/variant.hpp"
#include "core/geometry.h"

struct layer_visitor {
public:
  layer_visitor( Layer::ptrLayerType& layer) : m_layer(layer) {}

  void energy(double e) { m_energy=e; }
  typedef void result_type;
#ifdef GEOMETRY_RECTANGLE_2_H
  void operator()(const Rectangle_2& r) const {
    std::vector<double> vectx, vecty;
    for (unsigned int i=0; i<4;++i)
    {
      vectx.push_back(geometry::to_double(r[i].x()));
      vecty.push_back(geometry::to_double(r[i].y()));
    }
    m_layer->AddPolygon(vectx, vecty );
    std::ostringstream oss;
    oss << m_energy;
    double x = geometry::to_double(r.center().x());
    double y = geometry::to_double(r.center().y());
    m_layer->AddText(x, y, oss.str() , wxColour(255,0,0) );
  }
#endif // GEOMETRY_RECTANGLE_2_H

#ifdef GEOMETRY_CIRCLE_2_H
  void operator()(const Circle_2& c) const {
    double x = geometry::to_double(c.center().x());
    double y = geometry::to_double(c.center().y());
    double r = geometry::to_double(geometry::radius(c));
    m_layer->AddCircle(x,y,r);
    std::ostringstream oss;
    oss << m_energy;
    m_layer->AddText(x, y, oss.str() , wxColour(255,0,0) );
  }
#endif // GEOMETRY_CIRCLE_2_H
private:
  Layer::ptrLayerType& m_layer;
  double m_energy;
};


template<typename Config>
Layer::ptrLayerType& operator<<(Layer::ptrLayerType& layer, const Config& config)
{
	typename Config::const_iterator it = config.begin(), end = config.end();
	layer_visitor visitor(layer);
	for (; it != end; ++it)
	{
		visitor.energy(config.energy(it));
		rjmcmc::apply_visitor(visitor,config[it]);
	}
	return layer;
}


class rjmcmc_building_footprint_extraction_frame: public BasicViewerFrame
{
public:
	rjmcmc_building_footprint_extraction_frame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = _("frame"));
	~rjmcmc_building_footprint_extraction_frame();

	void OnGoButton(wxCommandEvent&);
	void OnStopButton(wxCommandEvent&);
	void OnChartButton(wxCommandEvent&);
	void OnParamButton(wxCommandEvent&);

	template<typename Config>
	void begin(unsigned int dump, unsigned int save, double t, const Config& config) {
		m_dump = dump;
		m_save = save;
		m_buttonGo->Disable();
		m_buttonStop->Enable();
	}

	template<typename Config, typename Sampler>
	bool iterate(unsigned int i, double t, const Config& config, const Sampler&)
	{
		if ( m_save && (i%m_save == 0) )
		{
			std::ostringstream out;
			out.width(8);
			out.fill('0');
			out << i;
			m_vlayer->Save( out.str() + ".shp" );
		}
		if (m_dump && (i % m_dump == 0))
		{
			wxMutexGuiEnter();
			{
				m_vlayer->Clear();
				m_vlayer << config;
				Refresh();
			}
			wxMutexGuiLeave();
		}
		return true;
	}

	template<typename Config>
	void end(const Config&)
	{
		m_buttonStop->Disable();
		m_buttonGo->Enable();
	}

	wxAboutDialogInfo getAboutInfo() const;

	void controler(Controler *c) { m_controler=c; } 

	DECLARE_GILVIEWER_METHODS_FOR_EVENTS_TABLE() ;
	DECLARE_EVENT_TABLE() ;

private:
	Controler *m_controler;
	wxButton *m_buttonGo;
	wxButton *m_buttonStop;
	PanelViewer *m_panel;

	int m_dump;
	int m_save;
	Layer::ptrLayerType m_vlayer;
};

#endif /* RJMCMC_BUILDINGSDETECTIONFRAME_HPP_ */
