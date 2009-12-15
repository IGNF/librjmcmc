#ifndef __RJMCMC_BUILDING_DETECTION_THREAD_H__
#define __RJMCMC_BUILDING_DETECTION_THREAD_H__

#if USE_GUI

#include <wx/thread.h>
#include <GilViewer/layers/ImageLayer.hpp>
#include "gui/rjmcmc_building_footprint_extraction_frame.hpp"
class PanelViewer;

#endif // USE_GUI

class rjmcmc_building_footprint_extraction_thread
#if USE_GUI
 : public wxThread
#endif
{
public:
	virtual void* Entry();

#if USE_GUI
	rjmcmc_building_footprint_extraction_thread(Layer::ptrLayerType layer, PanelViewer *panel, rjmcmc_building_footprint_extraction_frame *frame) :
		wxThread(wxTHREAD_JOINABLE), m_layer(layer), m_frame(frame) {}
private:
	Layer::ptrLayerType m_layer;
	rjmcmc_building_footprint_extraction_frame *m_frame;

#endif // USE_GUI

};

#endif //__RJMCMC_BUILDING_DETECTION_THREAD_H__
