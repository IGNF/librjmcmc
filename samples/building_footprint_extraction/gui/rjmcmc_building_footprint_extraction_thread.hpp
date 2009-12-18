#ifndef __RJMCMC_BUILDING_DETECTION_THREAD_H__
#define __RJMCMC_BUILDING_DETECTION_THREAD_H__

#include <wx/thread.h>
#include <GilViewer/layers/VectorLayerGhost.h>
#include <GilViewer/layers/ImageLayer.hpp>
#include "gui/rjmcmc_building_footprint_extraction_frame.hpp"
#include "core/building_footprint_extraction.hpp"
#include "core/rjmcmc_ostream_visitor.hpp"

class rjmcmc_building_footprint_extraction_thread : public wxThread
{
public:
	rjmcmc_building_footprint_extraction_thread(Layer::ptrLayerType layer, 
		rjmcmc_building_footprint_extraction_frame *frame);
	void begin(unsigned int dump, unsigned int save);
	bool iterate(unsigned int i, double t, const building_configuration& configuration, const building_sampler& sampler);
	void end(const building_configuration& configuration);
	void* Entry();
	
private:
	Layer::ptrLayerType m_layer;
	rjmcmc_building_footprint_extraction_frame *m_frame;
	std::ostringstream m_out;
	sampler_ostream_visitor<building_sampler> m_visitor;
	unsigned int m_dump;
};

#endif //__RJMCMC_BUILDING_DETECTION_THREAD_H__
