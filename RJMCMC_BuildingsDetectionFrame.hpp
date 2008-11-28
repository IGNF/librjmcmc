#ifndef RJMCMC_BUILDINGSDETECTIONFRAME_HPP_
#define RJMCMC_BUILDINGSDETECTIONFRAME_HPP_

#include <wx/wx.h>
#include <wx/statusbr.h>
#include <wx/thread.h>

#include "gui/PanelViewer.hpp"
#include "gui/LayerControl.hpp"

#include "BuildingsDetectorParameters.hpp"

class RJMCMC_BuildingsDetectionThread;

class RJMCMC_BuildingsDetectionFrame : public wxFrame
{
public:
	RJMCMC_BuildingsDetectionFrame(const wxString& title, const wxPoint& pos,const wxSize& size = wxSize(500,500));
	~RJMCMC_BuildingsDetectionFrame() {;}

	void OnGoButton(wxCommandEvent& event);
	void OnThreadEnd();

	RJMCMC_BuildingsDetectionThread *m_thread;

	DECLARE_EVENT_TABLE();


private:
	wxButton *m_buttonGo;
	PanelViewer *m_panel;
	std::vector< std::pair<wxStaticText*, wxTextCtrl*> > m_pileText;
};

class RJMCMC_BuildingsDetectionThread : public wxThread
{
public :
	RJMCMC_BuildingsDetectionThread( Layer::ptrLayerType layer , PanelViewer *panel, RJMCMC_BuildingsDetectionFrame *frame ) : wxThread(wxTHREAD_JOINABLE), m_layer(layer)/*, m_panel(panel)*/, m_frame(frame) {;}

	virtual void* Entry();

private:
	Layer::ptrLayerType m_layer;
	//PanelViewer *m_panel;
	RJMCMC_BuildingsDetectionFrame *m_frame;
};

#endif /* RJMCMC_BUILDINGSDETECTIONFRAME_HPP_ */
