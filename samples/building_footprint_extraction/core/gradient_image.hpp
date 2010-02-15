#ifndef GRADIENT_IMAGE_HPP
#define GRADIENT_IMAGE_HPP

#include <boost/gil/image.hpp>
#include <boost/gil/extension/matis/float_images.hpp>

#include "geometry/geometry.h"

namespace rjmcmc {
class gradient_image
{
public:
	typedef boost::gil::dev2n32F_image_t image_t;
	typedef boost::gil::dev2n32F_pixel_t pixel_t;

	double integrated_flux(const Segment_2 &s) const;
	double integrated_flux(const Circle_2  &c) const;

	void load(const std::string &file, const Iso_Rectangle_2& bbox, double sigmaD=1, unsigned int step=0);

	template<typename View>
	void load(const View& view, const Iso_Rectangle_2& bbox, double sigmaD=1, unsigned int step=0);

private:
	image_t m_gradients;
	Iso_Rectangle_2 m_bbox;
};
};

#endif // GRADIENT_IMAGE_HPP
