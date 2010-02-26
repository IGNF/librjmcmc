// Author(s)     : Mathieu Bredif

#ifndef GEOMETRY_CIRCLE_2_INTERSECTION_H
#define GEOMETRY_CIRCLE_2_INTERSECTION_H

#include "Circle_2.h"

#if USE_CGAL

#include <CGAL/Circle_2_Circle_2_intersection.h>

namespace geometry {
using namespace CGAL;

#else

namespace geometry {

template<class K> inline bool do_intersect(const Circle_2<K> &c, const Circle_2<K> &d)
{
	Vector_2 v = d.center() - c.center();
	FT v2 = v.squared_length();
	return (v2 < d.squared_radius() + c.squared_radius() + 2 * c.radius()*d.radius());
}

#endif

	// Tout vient de :
	// Weisstein, Eric W. "Circle-Circle Intersection." From MathWorld--A Wolfram Web Resource.
	// http://mathworld.wolfram.com/Circle-CircleIntersection.html
template<class K> inline typename K::FT intersection_area(const geometry::Circle_2<K> &c0, const geometry::Circle_2<K> &c1)
{
	typename K::Vector_2 diff(c1.center() - c0.center());
	FT d2 = diff.squared_length();
	FT r0 = radius(c0), r02 = c0.squared_radius(), r1 = radius(c1), r12 = c1.squared_radius();
	FT a = d2-r02-r12;
	FT b = 2*r0*r1;
	if(a>b) { // d²>(r0+r1)²
		return 0.;
	} else if(-a>b) { // d²<(r0-r1)²
		return M_PI * std::min(r02,r12);
	}
	FT d = sqrt(to_double(d2));
	FT area = r02*acos((d2+r02-r12)/(2.*d*r0)) 
                + r12*acos((d2+r12-r02)/(2.*d*r1))
                - 0.5 * ::sqrt((-d+r0+r1)*(d+r0-r1)*(d-r0+r1)*(d+r0+r1));
	return area;
}

}; // namespace geometry;

#endif // GEOMETRY_CIRCLE_2_INTERSECTION_H
