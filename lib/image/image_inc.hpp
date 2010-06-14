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
void ndvi_image(ndvi_image_t& ndvi,  int& x0, int& y0,
	const View& view, IsoRectangle& bbox, unsigned int step)
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
            //    resize_view(any_color_converted_view<gray32F_pixel_t>(v, dummy_color_converter()), boost::gil::view(ndvi),  bilinear_sampler());
	} else {
		ndvi.recreate(v.dimensions());
		boost::gil::copy_and_convert_pixels(v,boost::gil::view(ndvi), static_cast_color_converter());
	}
}

#include <boost/gil/extension/io/tiff_dynamic_io.hpp>

template<typename IsoRectangle>
void ndvi_image(ndvi_image_t& ndvi,  int& x0, int& y0,
	const std::string &file, IsoRectangle& bbox, unsigned int step)
{
	rjmcmc::any_image_t img;
	tiff_read_image(file.c_str(), img);
	ndvi_image(ndvi, x0, y0, const_view(img), bbox, step);
}

/*
    image_read_info< tiff_tag > info = read_image_info( filename
                                                    , tiff_tag()
                                                    );

    read_image( filename
              , img
              , image_read_settings< tiff_tag >( point_t( 0, 0 ), point_t( 50, 50 ) )
              );
*/

#endif // IMAGE_INC_HPP
