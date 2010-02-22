#ifndef RJMCMC_BUILDINGSDETECTIONFRAME_HPP_
#define RJMCMC_BUILDINGSDETECTIONFRAME_HPP_

// TODO : nettoyage des headers
#include <wx/wx.h>
#include <wx/statusbr.h>
#include <wx/thread.h>

#include <GilViewer/gui/LayerControl.hpp>
#include <GilViewer/gui/BasicViewerFrame.h>
#include <GilViewer/convenient/MacrosGilViewer.hpp>

class rjmcmc_building_footprint_rectangle_thread;
class parameters_frame;
class chart_frame;

class rjmcmc_building_footprint_rectangle_frame: public BasicViewerFrame
{
public:
        rjmcmc_building_footprint_rectangle_frame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = _("frame"));
        ~rjmcmc_building_footprint_rectangle_frame();

	void OnGoButton(wxCommandEvent& event);
	void OnStopButton(wxCommandEvent& event);

	void OnThreadEnd();

	wxAboutDialogInfo getAboutInfo() const;


	parameters_frame* m_parameters_frame;
	chart_frame* m_chart_frame;

	DECLARE_GILVIEWER_METHODS_FOR_EVENTS_TABLE() ;
	DECLARE_EVENT_TABLE() ;

private:
        rjmcmc_building_footprint_rectangle_thread *m_thread;
	wxCriticalSection m_cs;
	wxButton *m_buttonGo;
	wxButton *m_buttonStop;
	PanelViewer *m_panel;
};

#endif /* RJMCMC_BUILDINGSDETECTIONFRAME_HPP_ */
