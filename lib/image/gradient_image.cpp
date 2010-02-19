#include "geometry/Iso_rectangle_2_Segment_2_clip.h"
#include "geometry/Segment_2_iterator.h"

#include "image/gradient_image.hpp"

template<typename T> void Add1CirclePoints(const T& view, double cx, double cy, double dx, double dy, double d, double & res, double & w)
{
	int i = (int) (cx + dx);
	int j = (int) (cy + dy);
	if(i<0 || j<0 || i>=view.width() || j>=view.height()) return;
	rjmcmc::gradient_image::pixel_t grad = view(i,j);
	res += boost::gil::at_c<0>(grad) * dx + boost::gil::at_c<1>(grad) * dy;
	w   += d;
}

template<typename T> void Add4CirclePoints(const T& view, double cx, double cy, double d, double & res, double & w)
{
	Add1CirclePoints(view, cx, cy, 0, d, d, res, w);
	Add1CirclePoints(view, cx, cy, 0,-d, d, res, w);
	Add1CirclePoints(view, cx, cy, d, 0, d, res, w);
	Add1CirclePoints(view, cx, cy,-d, 0, d, res, w);
}

template<typename T> void Add8CirclePoints(const T& view, double cx, double cy, double dx, double dy, double & res, double & w)
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

double rjmcmc::gradient_image::integrated_flux(const Circle_2 &c) const
{
	double cx = c.center().x() - m_bbox.min().x();
	double cy = c.center().y() - m_bbox.min().y();
	double r  = CGAL::radius(c);
	double res = 0., w = 0.;
	double dx = 0;
	double dy = r;
	double p = 3 - 2*r;
	Add4CirclePoints(m_gradients._view, cx, cy, dy, res, w);
	while (dx < dy) {
		if (p < 0) {
			p += 4*dx+6;
		} else {
			--dy;
			Add8CirclePoints(m_gradients._view, cx, cy, dx, dy, res, w);
			p += 4*(dx-dy)+10;
		}
		++dx;
		Add8CirclePoints(m_gradients._view, cx, cy, dx, dy, res, w);
	}
	if(w==0) return 0.;
	return (res * CGAL::perimeter(c)) / w;
}

double rjmcmc::gradient_image::integrated_flux(const Segment_2& s0) const
{
	using namespace boost::gil;
	Segment_2 s(s0);
	if(!clip(m_bbox,s)) return 0;
	CGAL::Segment_2_iterator<K> it(s);

	int x0 = (int)m_bbox.min().x();
	int x1 = (int)m_bbox.max().x();
	int y0 = (int)m_bbox.min().y();
	int y1 = (int)m_bbox.max().y();

	typedef image_t::view_t::xy_locator xy_locator;
	xy_locator loc_grad = m_gradients._view.xy_at(
		(xy_locator::x_coord_t) (it.x()-x0),
		(xy_locator::y_coord_t) (it.y()-y0)
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
	Vector_2 normale = arete.perpendicular(CGAL::NEGATIVE);
	Vector_2 sum(gradient_sum[0], gradient_sum[1]);
	return CGAL::to_double(normale * sum);
}
