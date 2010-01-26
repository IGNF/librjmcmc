#ifndef IMAGE_HPP_
#define IMAGE_HPP_

#include <boost/gil/extension/io/tiff_dynamic_io.hpp>
#include "geometry/geometry.h"

namespace rjmcmc {
class image
{
public:
	typedef boost::gil::rgb16_image_t image_t;
	typedef boost::gil::rgb16_pixel_t pixel_t;

	double error(const Rectangle_2 &s) const;
	double error(const Circle_2  &c) const;

	void load(const std::string &file);

private:
	image_t m_image;
};
};

#endif // IMAGE_HPP_
