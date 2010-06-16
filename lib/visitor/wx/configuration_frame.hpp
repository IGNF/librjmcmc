#ifndef CONFIGURATION_FRAME_HPP
#define CONFIGURATION_FRAME_HPP

#include <wx/wx.h>
#include <wx/statusbr.h>

#include <GilViewer/gui/LayerControl.hpp>
#include <GilViewer/gui/BasicViewerFrame.h>
#include <GilViewer/convenient/MacrosGilViewer.hpp>
#include <GilViewer/layers/image_types.hpp>

#include "visitor/controler.hpp"
#include "rjmcmc/variant.hpp"
#include "geometry/wx/paint.h"

// to be specialized by each object type (-> files lib/geometry/*_paint.h)
template<typename T> void paint(Layer::ptrLayerType&, const std::string& s, const T&);

struct layer_painter {
public:
  layer_painter( Layer::ptrLayerType& layer) : m_layer(layer) {}

  void energy(double e) { m_energy=e; }

  typedef void result_type;

  template<typename T> void operator()(const T& t) const {
    std::ostringstream oss;
    oss << m_energy;
    paint(m_layer,oss.str(),t);
  }

private:
  Layer::ptrLayerType& m_layer;
  double m_energy;
};


template<typename Config>
Layer::ptrLayerType& operator<<(Layer::ptrLayerType& layer, const Config& config)
{
	typename Config::const_iterator it = config.begin(), end = config.end();
	layer_painter painter(layer);
	for (; it != end; ++it)
	{
		painter.energy(config.energy(it));
		rjmcmc::apply_visitor(painter,config[it]);
	}
	return layer;
}


class configuration_frame: public BasicViewerFrame
{
public:
        configuration_frame(
		wxWindow* parent,
		wxWindowID id,
		const wxString& title,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxDEFAULT_FRAME_STYLE,
		const wxString& name = _("frame"));

        ~configuration_frame() {}

	void OnGoButton   (wxCommandEvent&);
	void OnStopButton (wxCommandEvent&);
	void OnChartButton(wxCommandEvent&);
	void OnParamButton(wxCommandEvent&);

	void init(int dump, int save);

	template<typename Configuration, typename Sampler>
        void begin(const Configuration& config, const Sampler& sample, double t)
	{
		m_buttonGo  ->Disable();
		m_buttonStop->Enable();
	}
	template<typename Configuration, typename Sampler>
        void end(const Configuration& config, const Sampler& sample, double t)
	{
		m_buttonStop->Disable();
		m_buttonGo  ->Enable();
	}

	template<typename Configuration, typename Sampler>
	bool iterate(const Configuration& config, const Sampler&, double, unsigned int i)
	{
		if ( m_save && (i%m_save == 0) )
		{
			std::ostringstream out;
			out.width(8);
			out.fill('0');
			out << i;
                        m_vlayer->save( out.str() + ".shp" );
		}
		if (m_dump && (i % m_dump == 0))
		{
			wxMutexGuiEnter();
			m_vlayer->clear();
			m_vlayer << config;
			Refresh();
			wxMutexGuiLeave();
		}
		return true;
	}

	wxAboutDialogInfo getAboutInfo() const;

	void controler(Controler *c) { m_controler=c; } 

	void add_layer(const std::string& file);
	wxRect get_bbox() const;
	void set_bbox(const wxRect& r);

	DECLARE_GILVIEWER_METHODS_FOR_EVENTS_TABLE() ;
	DECLARE_EVENT_TABLE() ;

private:
        Controler    *m_controler;
        wxButton     *m_buttonGo;
        wxButton     *m_buttonStop;
        panel_viewer *m_panel;

	int m_dump;
	int m_save;
        layer::ptrLayerType m_vlayer;
};

#endif /* CONFIGURATION_FRAME_HPP */
