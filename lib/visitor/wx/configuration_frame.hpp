#ifndef CONFIGURATION_FRAME_HPP
#define CONFIGURATION_FRAME_HPP

#include <wx/wx.h>
#include <wx/statusbr.h>

#include <GilViewer/gui/LayerControl.hpp>
#include <GilViewer/gui/BasicViewerFrame.h>
#include <GilViewer/convenient/MacrosGilViewer.hpp>

#include "visitor/controler.hpp"
#include "rjmcmc/variant.hpp"

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

	template<typename Config>
	void begin(unsigned int dump, unsigned int save, double t, const Config& config) {
		m_dump = dump;
		m_save = save;
		m_buttonGo  ->Disable();
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
			m_vlayer->Clear();
			m_vlayer << config;
			Refresh();
			wxMutexGuiLeave();
		}
		return true;
	}

	template<typename Config>
	void end(const Config&)
	{
		m_buttonStop->Disable();
		m_buttonGo  ->Enable();
	}

	wxAboutDialogInfo getAboutInfo() const;

	void controler(Controler *c) { m_controler=c; } 

	DECLARE_GILVIEWER_METHODS_FOR_EVENTS_TABLE() ;
	DECLARE_EVENT_TABLE() ;

private:
	Controler   *m_controler;
	wxButton    *m_buttonGo;
	wxButton    *m_buttonStop;
	PanelViewer *m_panel;

	int m_dump;
	int m_save;
	Layer::ptrLayerType m_vlayer;
};

#endif /* CONFIGURATION_FRAME_HPP */
