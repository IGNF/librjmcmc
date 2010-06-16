// Author(s)     : Mathieu Bredif, Didier Boldo

#ifndef GEOMETRY_CIRCLE_2_INTEGRATED_FLUX_H
#define GEOMETRY_CIRCLE_2_INTEGRATED_FLUX_H

#include "Circle_2.h"
#include <boost/gil/image.hpp>
#include <boost/gil/extension/matis/float_images.hpp>

template<typename View>
void Add1CirclePoints(const View& view, double cx, double cy, double dx, double dy, double d, double & res, double & w)
{
//    typedef View::pixel_t pixel_t;
    typedef boost::gil::dev2n32F_pixel_t pixel_t;
	int i = (int) (cx + dx);
	int j = (int) (cy + dy);
	if(i<0 || j<0 || i>=view.width() || j>=view.height()) return;
	const pixel_t& grad = view(i,j);
	res += boost::gil::at_c<0>(grad) * dx + boost::gil::at_c<1>(grad) * dy;
	w   += d;
}

template<typename View>
void Add4CirclePoints(const View& view, double cx, double cy, double d, double & res, double & w)
{
	Add1CirclePoints(view, cx, cy, 0, d, d, res, w);
	Add1CirclePoints(view, cx, cy, 0,-d, d, res, w);
	Add1CirclePoints(view, cx, cy, d, 0, d, res, w);
	Add1CirclePoints(view, cx, cy,-d, 0, d, res, w);
}

template<typename View>
void Add8CirclePoints(const View& view, double cx, double cy, double dx, double dy, double & res, double & w)
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

template<typename OrientedView, typename K>
double integrated_flux(const OrientedView& v, const geometry::Circle_2<K> &c)
{
	typedef typename OrientedView::view_t view_t;
    view_t view(v.view());
	int x0 = v.x0();
	int y0 = v.y0();

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

#endif // GEOMETRY_CIRCLE_2_INTEGRATED_FLUX_H
