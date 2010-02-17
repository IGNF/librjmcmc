#ifndef IMAGE_INC_HPP
#define IMAGE_INC_HPP

#include "core/image.hpp"

struct static_cast_color_converter
{
	template<class Src, class Dst>
	void operator()(const Src &src, Dst &dst) const
	{
		typedef typename boost::gil::channel_type<Dst>::type type;
		boost::gil::at_c<0>(dst) = static_cast<type>(boost::gil::at_c<0>(src));
	}
};

template<typename View>
void rjmcmc::image::load(const View& view, const Iso_rectangle_2& bbox, unsigned int step)
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
	m_bbox = Iso_rectangle_2(x0,y0,x1,y1);

	if(step>1) {
            //    resize_view(any_color_converted_view<gray32F_pixel_t>(v, dummy_color_converter()), boost::gil::view(m_image),  bilinear_sampler());
	} else {
		m_image.recreate(v.dimensions());
		boost::gil::copy_and_convert_pixels(v,boost::gil::view(m_image), static_cast_color_converter());
	}
}


#endif // GRADIENT_IMAGE_INC_HPP
