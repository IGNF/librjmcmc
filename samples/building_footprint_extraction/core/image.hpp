#ifndef RJMCMC_IMAGE_HPP_
#define RJMCMC_IMAGE_HPP_

#include <boost/gil/image.hpp>
#include <boost/gil/extension/matis/float_images.hpp>
#include "geometry/geometry.h"

namespace rjmcmc {
class image
{
public:
	typedef boost::gil::gray16s_image_t image_t;
	typedef boost::gil::gray16s_pixel_t pixel_t;

	double error(const Rectangle_2 &s) const;
	double error(const Circle_2  &c) const;

	void load(const std::string &file, const Iso_rectangle_2& bbox, unsigned int step=0);

	template<typename View>
	void load(const View& view, const Iso_rectangle_2& bbox, unsigned int step=0);

private:
	image_t m_image;
	Iso_rectangle_2 m_bbox;
};
};

#endif // IMAGE_HPP_
