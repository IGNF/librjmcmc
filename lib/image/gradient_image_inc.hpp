#ifndef GRADIENT_IMAGE_INC_HPP
#define GRADIENT_IMAGE_INC_HPP

#include "image/gradient_image.hpp"
#include <boost/gil/extension/io/tiff_dynamic_io.hpp> // to write debugging images

void init(gradient_image_t& grad, const boost::gil::gray32F_image_t::const_view_t& v, double sigmaD=1);

template<typename View, typename IsoRectangle>
void gradient_image(gradient_image_t& grad, int& x0, int& y0,
	const View& view, IsoRectangle& bbox, double sigmaD, unsigned int step)
{
	x0 = 0;
	y0 = 0;
	View v = view;
	if(!(bbox.is_degenerate ())) {
		x0 = std::max(x0,(int) bbox.min().x());
		y0 = std::max(y0,(int) bbox.min().y());
		int x1 = std::min((int) view.width (),(int) bbox.max().x());
		int y1 = std::min((int) view.height(),(int) bbox.max().y());
		v = subimage_view(v,x0,y0,x1-x0,y1-y0);
		bbox = IsoRectangle(x0,y0,x1,y1);
	}

	boost::gil::gray32F_image_t img(v.dimensions());
	if(step>1) {
            //    resize_view(any_color_converted_view<gray32F_pixel_t>(v, dummy_color_converter()), boost::gil::view(img),  bilinear_sampler());
	} else {
		boost::gil::copy_and_convert_pixels(v,boost::gil::view(img), boost::gil::dummy_color_converter());
	}
	init(grad, const_view(img), sigmaD);
}

template<typename IsoRectangle>
void gradient_image(gradient_image_t& grad,  int& x0, int& y0,
	const std::string &file, IsoRectangle& bbox, double sigmaD, unsigned int step)
{
	rjmcmc::any_image_t img;
	tiff_read_image(file.c_str(), img);
	gradient_image(grad, x0, y0, const_view(img), bbox, sigmaD, step);
}

#include "image/gradient_image.cpp"

#endif // GRADIENT_IMAGE_INC_HPP
