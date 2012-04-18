/***********************************************************************
This file is part of the librjmcmc project source files.

Copyright : Institut Geographique National (2008-2012)
Contributors : Mathieu Brédif, Olivier Tournaire, Didier Boldo
email : librjmcmc@ign.fr

This software is a generic C++ library for stochastic optimization.

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software. You can use,
modify and/or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty and the software's author, the holder of the
economic rights, and the successive licensors have only limited liability.

In this respect, the user's attention is drawn to the risks associated
with loading, using, modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean that it is complicated to manipulate, and that also
therefore means that it is reserved for developers and experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and, more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

***********************************************************************/

#ifndef GEOMETRY_CIRCLE_2_INTERSECTION_HPP
#define GEOMETRY_CIRCLE_2_INTERSECTION_HPP

#include "geometry/Circle_2.hpp"

#if USE_CGAL

#include <CGAL/Circle_2_Circle_2_intersection.h>

namespace geometry {
using namespace CGAL;

#else

namespace geometry {

template<class K> inline bool do_intersect(const Circle_2<K> &c, const Circle_2<K> &d)
{
	typename K::Vector_2 v = d.center() - c.center();
	typename K::FT v2 = v.squared_length();
	return (v2 < d.squared_radius() + c.squared_radius() + 2 * c.radius()*d.radius());
}

#endif

	// Tout vient de :
	// Weisstein, Eric W. "Circle-Circle Intersection." From MathWorld--A Wolfram Web Resource.
	// http://mathworld.wolfram.com/Circle-CircleIntersection.html
template<class K> inline typename K::FT intersection_area(const geometry::Circle_2<K> &c0, const geometry::Circle_2<K> &c1)
{
	typename K::Vector_2 diff(c1.center() - c0.center());
	typename K::FT d2 = diff.squared_length();
	typename K::FT r0 = radius(c0), r02 = c0.squared_radius(), r1 = radius(c1), r12 = c1.squared_radius();
	typename K::FT a = d2-r02-r12;
	typename K::FT b = 2*r0*r1;
	if(a>b) { // d²>(r0+r1)²
		return 0.;
	} else if(-a>b) { // d²<(r0-r1)²
		return M_PI * std::min(r02,r12);
	}
	typename K::FT d = sqrt(to_double(d2));
	typename K::FT area = r02*acos((d2+r02-r12)/(2.*d*r0)) 
                + r12*acos((d2+r12-r02)/(2.*d*r1))
                - 0.5 * ::sqrt((-d+r0+r1)*(d+r0-r1)*(d-r0+r1)*(d+r0+r1));
	return area;
}

}; // namespace geometry;

#endif // GEOMETRY_CIRCLE_2_INTERSECTION_HPP
