#ifndef __RJMCMC_BUILDING_DETECTION_THREAD_H__
#define __RJMCMC_BUILDING_DETECTION_THREAD_H__

#include <wx/thread.h>
#include <GilViewer/layers/Layer.hpp>
#include "gui/rjmcmc_building_footprint_rectangle_frame.hpp"

#include "core/building_footprint_rectangle.hpp" // defines configuration & sampler
#include "visitor/ostream_visitor.hpp"

class rjmcmc_building_footprint_rectangle_thread : public wxThread
{
public:
        rjmcmc_building_footprint_rectangle_thread(
		Layer::ptrLayerType ilayer, 
		Layer::ptrLayerType vlayer, 
                rjmcmc_building_footprint_rectangle_frame *frame);
	void begin(int dump, int save, double t, const configuration& config);
	bool iterate(unsigned int i, double t, const configuration& config, const sampler& sample);
	void end(const configuration& config);
	void* Entry();
	
private:
	Layer::ptrLayerType m_ilayer, m_vlayer;
        rjmcmc_building_footprint_rectangle_frame *m_frame;
	std::ostringstream m_out;
	rjmcmc::ostream_visitor<sampler> m_visitor;
	int m_dump;
	int m_save;
};

#endif //__RJMCMC_BUILDING_DETECTION_THREAD_H__
