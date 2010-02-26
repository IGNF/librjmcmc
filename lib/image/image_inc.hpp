#ifndef IMAGE_INC_HPP
#define IMAGE_INC_HPP

#include "image/image.hpp"

struct static_cast_color_converter
{
	template<class Src, class Dst>
	void operator()(const Src &src, Dst &dst) const
	{
		typedef typename boost::gil::channel_type<Dst>::type type;
		boost::gil::at_c<0>(dst) = static_cast<type>(boost::gil::at_c<0>(src));
	}
};

template<typename View, typename IsoRectangle>
void rjmcmc::image::load(const View& view, const IsoRectangle& bbox, unsigned int step)
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
	}

	if(step>1) {
            //    resize_view(any_color_converted_view<gray32F_pixel_t>(v, dummy_color_converter()), boost::gil::view(m_image),  bilinear_sampler());
	} else {
		m_image.recreate(v.dimensions());
		boost::gil::copy_and_convert_pixels(v,boost::gil::view(m_image), static_cast_color_converter());
	}
}

#include <boost/gil/extension/io/tiff_dynamic_io.hpp>
template<typename IsoRectangle>
void rjmcmc::image::load(const std::string &file, const IsoRectangle& bbox, unsigned int step)
{
	image_t img;
	tiff_read_image(file.c_str(), img);
	load(const_view(img), bbox, step);
}

#include "image/image.cpp"

#endif // IMAGE_INC_HPP
