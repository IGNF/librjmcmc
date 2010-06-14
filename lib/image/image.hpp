#ifndef RJMCMC_IMAGE_HPP_
#define RJMCMC_IMAGE_HPP_

#include <boost/gil/image.hpp>
typedef boost::gil::gray16s_image_t ndvi_image_t;
typedef boost::gil::gray16s_view_t  ndvi_view_t;
typedef const boost::gil::gray16s_view_t  ndvi_const_view_t;

template<typename View, typename IsoRectangle>
void ndvi_image(ndvi_image_t& img,  int& x0, int& y0,
	const View& view, IsoRectangle& bbox, unsigned int step=0);

template<typename IsoRectangle>
void ndvi_image(ndvi_image_t& img,  int& x0, int& y0,
	const std::string &file, IsoRectangle& bbox, unsigned int step=0);

#endif // RJMCMC_IMAGE_HPP_
