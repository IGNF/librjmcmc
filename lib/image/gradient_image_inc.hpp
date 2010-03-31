#ifndef GRADIENT_IMAGE_INC_HPP
#define GRADIENT_IMAGE_INC_HPP

#include "image/gradient_image.hpp"
#include <boost/gil/extension/io/tiff_dynamic_io.hpp> // to write debugging images

void init(rjmcmc::gradient_image::image_t& g, const boost::gil::gray32F_image_t::const_view_t& v, double sigmaD=1);

template<typename View, typename IsoRectangle>
void rjmcmc::gradient_image::load(const View& view, const IsoRectangle& bbox, double sigmaD, unsigned int step)
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

	boost::gil::gray32F_image_t img(v.dimensions());
	if(step>1) {
            //    resize_view(any_color_converted_view<gray32F_pixel_t>(v, dummy_color_converter()), boost::gil::view(img),  bilinear_sampler());
	} else {
		boost::gil::copy_and_convert_pixels(v,boost::gil::view(img), boost::gil::dummy_color_converter());
	}
	init(m_gradients, boost::gil::const_view(img), sigmaD);
}

template<typename IsoRectangle>
void rjmcmc::gradient_image::load(const std::string &file, const IsoRectangle& bbox, double sigmaD, unsigned int step)
{
	boost::gil::gray32F_image_t img;
	tiff_read_image(file.c_str(), img);
	load(const_view(img), bbox, sigmaD, step);
}

#include "image/gradient_image.cpp"

#ifdef GEOMETRY_CIRCLE_2_H
template<typename View> void Add1CirclePoints(const View& view, double cx, double cy, double dx, double dy, double d, double & res, double & w)
{
	int i = (int) (cx + dx);
	int j = (int) (cy + dy);
	if(i<0 || j<0 || i>=view.width() || j>=view.height()) return;
	const boost::gil::dev2n32F_pixel_t& /*typename View::pixel_t&*/  grad = view(i,j);
	res += boost::gil::at_c<0>(grad) * dx + boost::gil::at_c<1>(grad) * dy;
	w   += d;
}

template<typename View> void Add4CirclePoints(const View& view, double cx, double cy, double d, double & res, double & w)
{
	Add1CirclePoints(view, cx, cy, 0, d, d, res, w);
	Add1CirclePoints(view, cx, cy, 0,-d, d, res, w);
	Add1CirclePoints(view, cx, cy, d, 0, d, res, w);
	Add1CirclePoints(view, cx, cy,-d, 0, d, res, w);
}

template<typename View> void Add8CirclePoints(const View& view, double cx, double cy, double dx, double dy, double & res, double & w)
{
	double d = sqrt(dx*dx+dy*dy);
	Add1CirclePoints(view, cx, cy, dx, dy, d, res, w);
	Add1CirclePoints(view, cx, cy,-dx, dy, d, res, w);
	Add1CirclePoints(view, cx, cy,-dx,-dy, d, res, w);
	Add1CirclePoints(view, cx, cy, dx,-dy, d, res, w);
	Add1CirclePoints(view, cx, cy, dy, dx, d, res, w);
	Add1CirclePoints(view, cx, cy,-dy, dx, d, res, w);
	Add1CirclePoints(view, cx, cy,-dy,-dx, d, res, w);
	Add1CirclePoints(view, cx, cy, dy,-dx, d, res, w);
}

template<typename View> double integrated_flux(const View& view, int x0, int y0, const Circle_2 &c)
{
	double cx = c.center().x() - x0;
	double cy = c.center().y() - y0;
	double r  = geometry::radius(c);
	double res = 0., w = 0.;
	double dx = 0;
	double dy = r;
	double p = 3 - 2*r;
	Add4CirclePoints(view, cx, cy, dy, res, w);
	while (dx < dy) {
		if (p < 0) {
			p += 4*dx+6;
		} else {
			--dy;
			Add8CirclePoints(view, cx, cy, dx, dy, res, w);
			p += 4*(dx-dy)+10;
		}
		++dx;
		Add8CirclePoints(view, cx, cy, dx, dy, res, w);
	}
	if(w==0) return 0.;
	return (res * geometry::perimeter(c)) / w;
}
#endif // GEOMETRY_CIRCLE_2_H


#ifdef GEOMETRY_RECTANGLE_2_H
#include "geometry/Segment_2_iterator.h"
#include "geometry/Iso_rectangle_2_Segment_2_clip.h"
template<typename View> double integrated_flux(const View& view, int x0, int y0, const Segment_2& s0)
{
	using namespace boost::gil;
	int x1 = x0+view.width();
	int y1 = y0+view.height();
	Segment_2 s(s0);
	Iso_rectangle_2 bbox(x0,y0,x1,y1);
	if(!clip(bbox,s)) return 0;
	geometry::Segment_2_iterator<K> it(s);

	typedef typename View::xy_locator xy_locator;
	xy_locator loc_grad = view.xy_at(
		(typename xy_locator::x_coord_t) (it.x()-x0),
		(typename xy_locator::y_coord_t) (it.y()-y0)
	);

	point2<std::ptrdiff_t> movement[2] = {
		point2<std::ptrdiff_t> (it.step(0), 0),
		point2<std::ptrdiff_t> (0, it.step(1))
	};
	float gradient_sum[2] =	{ 0., 0. };
	for (; !it.end() ; ++it)
	{
		if(it.x()>=x0 && it.x()<x1 && it.y()>=y0 && it.y()<y1) {
			float length = it.length();
			const dev2n32F_pixel_t& grad = *loc_grad;
			gradient_sum[0] += length * at_c<0> (grad);
			gradient_sum[1] += length * at_c<1> (grad);
		}
		loc_grad += movement[it.axis()];
	}

	Vector_2 arete(s.target()-s.source());
	Vector_2 normale = arete.perpendicular(geometry::NEGATIVE);
	Vector_2 sum(gradient_sum[0], gradient_sum[1]);
	return geometry::to_double(normale * sum);
}

template<typename View> double integrated_flux(const View& view, int x0, int y0, const Rectangle_2& r)
{
	return 	  std::max(0.,integrated_flux(view,x0,y0,r.segment(0)))
		+ std::max(0.,integrated_flux(view,x0,y0,r.segment(1)))
		+ std::max(0.,integrated_flux(view,x0,y0,r.segment(2)))
		+ std::max(0.,integrated_flux(view,x0,y0,r.segment(3)));
}

template<typename View> double inverted_integrated_flux(const View& view, int x0, int y0, const Rectangle_2& r)
{
        return 	  std::max(0.,-integrated_flux(view,x0,y0,r.segment(0)))
                + std::max(0.,-integrated_flux(view,x0,y0,r.segment(1)))
                + std::max(0.,-integrated_flux(view,x0,y0,r.segment(2)))
                + std::max(0.,-integrated_flux(view,x0,y0,r.segment(3)));
}
#endif // GEOMETRY_RECTANGLE_2_H

#endif // GRADIENT_IMAGE_INC_HPP
