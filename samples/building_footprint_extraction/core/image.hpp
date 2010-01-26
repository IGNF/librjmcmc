#ifndef IMAGE_HPP_
#define IMAGE_HPP_

#include <boost/gil/extension/io/tiff_dynamic_io.hpp>
#include <boost/gil/extension/matis/float_images.hpp>
#include "core/bbox.hpp"
#include "geometry/geometry.h"

namespace rjmcmc {
class image
{
public:
	typedef boost::gil::dev2n32F_image_t image_t;
	typedef boost::gil::dev2n32F_pixel_t pixel_t;

	double integrated_flux(const Segment_2 &s) const;
	double integrated_flux(const Circle_2  &c) const;


	double error(const Rectangle_2 &s) const { return 0; }
	double error(const Circle_2  &c) const { return 0; }

	void load(const std::string &file, double sigmaD=1, unsigned int step=0);
	void load(const std::string &file, const bbox_2& bbox, double sigmaD=1, unsigned int step=0); // subimage loading

private:
	image_t m_gradients;
	bbox_2 m_bbox;
};
};

#endif // IMAGE_HPP_
