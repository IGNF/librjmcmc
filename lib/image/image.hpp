#ifndef RJMCMC_IMAGE_HPP_
#define RJMCMC_IMAGE_HPP_

#include <boost/gil/image.hpp>
#include <boost/gil/extension/matis/float_images.hpp>

namespace rjmcmc {
class image
{
public:
	typedef boost::gil::gray16s_image_t image_t;
	typedef boost::gil::gray16s_pixel_t pixel_t;

#ifdef GEOMETRY_RECTANGLE_2_H
	double error(const Rectangle_2 &s) const;
#endif // GEOMETRY_RECTANGLE_2_H

#ifdef GEOMETRY_CIRCLE_2_H
	double error(const Circle_2  &c) const;
#endif // GEOMETRY_CIRCLE_2_H

	template<typename IsoRectangle>
	void load(const std::string &file, const IsoRectangle& bbox, unsigned int step=0);

	template<typename View, typename IsoRectangle>
	void load(const View& view, const IsoRectangle& bbox, unsigned int step=0);

private:
	image_t m_image;
	int x0, y0;
};
};

#endif // IMAGE_HPP_
