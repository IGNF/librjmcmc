#ifndef IMAGE_INC_HPP
#define IMAGE_INC_HPP

#include "image/image.hpp"

template<typename T, typename U> T my_min(const T& t, const U& u) { return std::min(t,T(u)); }
template<typename T, typename U> T my_max(const T& t, const U& u) { return std::max(t,T(u)); }

template<typename IsoRectangle, typename T, typename U>
void clip_bbox(IsoRectangle& bbox, T x0, T y0, U x1, U y1)
{
	if(bbox.is_degenerate ()) {
		bbox = IsoRectangle(x0,y0,x1,y1);
	} else {
		bbox = IsoRectangle(
			my_max(bbox.min().x(),x0),
			my_max(bbox.min().y(),y0),
			my_min(bbox.max().x(),x1),
			my_min(bbox.max().y(),y1)
		);
	}
}


template<typename IsoRectangle, typename View>
void clip_bbox(IsoRectangle& bbox, const oriented<View>& v)
{
	clip_bbox(v.x0(), v.y0(), v.x0()+v.view().width(), v.y0()+v.view().height());
}

template<typename View, typename IsoRectangle>
void load_view(oriented<View>& out, const oriented<View>& in, const IsoRectangle& bbox)
{
	using namespace boost::gil;
	int x0 = (int) bbox.min().x();
	int y0 = (int) bbox.min().y();
	int x1 = (int) bbox.max().x();
	int y1 = (int) bbox.max().y();
	out = oriented<View>(subimage_view(in.view(),x0-in.x0(),y0-in.y0(),x1-x0,y1-y0),x0,y0);
}

#include <boost/gil/extension/io_new/tiff_read.hpp>
template<typename IsoRectangle>
void clip_bbox(IsoRectangle& bbox, const std::string &file)
{
	using namespace boost::gil;
    image_read_info<tiff_tag> info = read_image_info(file, tiff_tag());
	clip_bbox(bbox,0,0,info._width,info._height);
}

template<typename View, typename Image, typename IsoRectangle>
void load_view(oriented<View>& out, Image& img, const std::string &file, const IsoRectangle& bbox)
{
	int x0 = (int) bbox.min().x();
	int y0 = (int) bbox.min().y();
	int x1 = (int) bbox.max().x();
	int y1 = (int) bbox.max().y();

	using namespace boost::gil;
    read_image( file, img, image_read_settings<tiff_tag>( point_t(x0,y0), point_t(x1-x0,y1-y0) ) );
	out = oriented<View>(view(img),x0,y0);
}

#endif // IMAGE_INC_HPP
