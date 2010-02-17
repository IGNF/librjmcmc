#ifndef IMAGE_INC_HPP
#define IMAGE_INC_HPP

#include "core/image.hpp"
#include <boost/gil/extension/matis/float_images.hpp>
#include <boost/gil/extension/io/tiff_dynamic_io.hpp> // to write debugging images

template<typename View>
void rjmcmc::image::load(const View& view, const Iso_Rectangle_2& bbox, unsigned int step)
{
	int x0 = 0;
	int y0 = 0;
	int x1 = (int) view.width();
	int y1 = (int) view.height();
	View v = view;
	if(!(bbox.is_degenerate ())) {
		x0 = std::max(x0,(int) bbox.min().x());
		y0 = std::max(y0,(int) bbox.min().y());
		x1 = std::min(x1,(int) bbox.max().x());
		y1 = std::min(y1,(int) bbox.max().y());
		v = subimage_view(v,x0,y0,x1-x0,y1-y0);
	}
	m_bbox = Iso_Rectangle_2(x0,y0,x1,y1);

	m_image.recreate(v.dimensions());
	if(step>1) {
            //    resize_view(any_color_converted_view<gray32F_pixel_t>(v, dummy_color_converter()), view(img),  bilinear_sampler());
	} else {
		boost::gil::copy_and_convert_pixels(v,boost::gil::view(m_image),boost::gil::dummy_color_converter());
	}
}

void rjmcmc::image::load(const std::string &file, const Iso_Rectangle_2& bbox, unsigned int step)
{
	image_t img;
	tiff_read_image(file.c_str(), img);
	load(const_view(img), bbox, step);
}

#endif // GRADIENT_IMAGE_INC_HPP
