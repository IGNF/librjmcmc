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

#ifndef GEOMETRY_CIRCLE_2_RECTANGLE_2_INTERSECTION_HPP
#define GEOMETRY_CIRCLE_2_RECTANGLE_2_INTERSECTION_HPP

#include "geometry/Rectangle_2.hpp"
#include "geometry/Circle_2.hpp"


namespace geometry {
#if USE_CGAL
using namespace CGAL;
#endif

namespace Impl {

template<class T> inline double circular_triangle_area(T x0, T y0, T x1, T y1, T R ) {
	T mx= 0.5*(x0+x1);
	T my= 0.5*(y0+y1);
	T R2 = R*R;
	T r2 = mx*mx+my*my;
	T r  = sqrt(r2);
	return (x1-x0)*(y1-y0)*0.5+R2*acos(r/R)-r*sqrt(R2-r2);
}

template<class T> bool intersection_area_rectangle_circle_aux(
	T& area,
	bool out00, bool out01, bool out11, bool out10, 
	T x0, T y0, T x1, T y1, T rn,
	T x02, T y02, T x12, T y12, T rn2
)
{
	if(!(out11 && x1>0 && y1>0) ) return false;
	if(out00 && x0>0 && y0>0) {
		area = 0;
		return true;
	}
	if(out01 && x0>0 && y1>0) {
		if(out10 && x1>0 && y0>0) {
			T xr0 = sqrt(rn2-y02);
			T yr0 = sqrt(rn2-x02);
			area  = circular_triangle_area(x0,y0,xr0,yr0,rn);
			return true;
		} else {
			T yr0 = sqrt(rn2-x02);
			T yr1 = sqrt(rn2-x12);
			area += circular_triangle_area(x0,yr1,x0,yr0,rn) - (x1-x0)*(y1-yr1);
		}
	} else {
		if(out10 && x1>0 && y0>0) {
			T xr0 = sqrt(rn2-y02);
			T xr1 = sqrt(rn2-y12);
			area += circular_triangle_area(xr1,y0,xr0,y0,rn) - (y1-y0)*(x1-xr1);
		} else {
			T xr1 = sqrt(rn2-y12);
			T yr1 = sqrt(rn2-x12);
			area += circular_triangle_area(xr1,yr1,x1,y1,rn) - (x1-xr1)*(y1-yr1);
		}
	}
	return false;
}

}; // namespace Impl


template<class K> inline typename K::FT intersection_area(const Rectangle_2<K> &r, const geometry::Circle_2<K> &c)
{
	typedef typename K::Vector_2 Vector_2;
	typedef typename K::FT FT;
	if(r.is_degenerate() || c.is_degenerate()) return 0;
        Vector_2 v(r.center()-c.center());
        Vector_2 n(r.normal());
        FT n2 = n.squared_length();
        FT vn = v*n;
        FT rn = sqrt(c.squared_radius()*n2);
        FT x0 = std::max(vn-n2,-rn);
        FT x1 = std::min(vn+n2, rn);
	FT dx = x1-x0;
	if(dx<=0) return 0;
        Vector_2 m(-r.normal().y(),r.normal().x());
        FT m2 = std::abs(r.ratio())*n2;
        FT vm = v*m;
        FT y0 = std::max(vm-m2,-rn);
        FT y1 = std::min(vm+m2, rn);
	FT dy = y1-y0;
	if(dy<=0) return 0;
	FT x02 = x0*x0;
	FT y02 = y0*y0;
	FT x12 = x1*x1;
	FT y12 = y1*y1;
	FT rn2 = rn*rn;
	bool out00 = (x02+y02>=rn2) ;
	bool out01 = (x02+y12>=rn2) ;
	bool out11 = (x12+y12>=rn2) ;
	bool out10 = (x12+y02>=rn2) ;
	FT area = dx*dy;
	if(Impl::intersection_area_rectangle_circle_aux(area, out00, out01, out11, out10, x0, y0, x1, y1, rn, x02, y02, x12, y12, rn2)) return area/n2;
	if(Impl::intersection_area_rectangle_circle_aux(area, out01, out11, out10, out00,-y1, x0,-y0, x1, rn, y12, x02, y02, x12, rn2)) return area/n2;
	if(Impl::intersection_area_rectangle_circle_aux(area, out11, out10, out00, out01,-x1,-y1,-x0,-y0, rn, x12, y12, x02, y02, rn2)) return area/n2;
	if(Impl::intersection_area_rectangle_circle_aux(area, out10, out00, out01, out11, y0,-x1, y1,-x0, rn, y02, x12, y12, x02, rn2)) return area/n2;
	return area/n2;
}

template<class K> inline typename K::FT intersection_area(const geometry::Circle_2<K> &c, const Rectangle_2<K> &r)
{
	return intersection_area(r,c);
}

template<class K> bool do_intersect(const Rectangle_2<K> &r, const geometry::Circle_2<K> &c)
{
	typedef typename K::Vector_2 Vector_2;
	typedef typename K::FT FT;
	if(r.is_degenerate() || c.is_degenerate()) return false;
        Vector_2 v(r.center()-c.center());
        Vector_2 n(r.normal());
        FT n2 = n.squared_length();
        FT vn = v*n;
        FT rn = sqrt(c.squared_radius()*n2);
        FT x0 = std::max(vn-n2,-rn);
        FT x1 = std::min(vn+n2, rn);
	FT dx = x1-x0;
	if(dx<=0) return false;
        Vector_2 m(-r.normal().y(),r.normal().x());
        FT m2 = std::abs(r.ratio())*n2;
        FT vm = v*m;
        FT y0 = std::max(vm-m2,-rn);
        FT y1 = std::min(vm+m2, rn);
	FT dy = y1-y0;
	if(dy<=0) return false;
	FT rn2 = rn*rn;
	FT x02 = x0*x0;
	FT y02 = y0*y0;
	if(x02+y02>=rn2 && x0>0 && y0>0) return false;
	FT y12 = y1*y1;
	if(x02+y12>=rn2 && x0>0 && y1<0) return false;
	FT x12 = x1*x1;
	if(x12+y12>=rn2 && x1<0 && y1<0) return false;
	if(x12+y02>=rn2 && x1<0 && y0>0) return false;
	return true;
}

template<class K> inline bool do_intersect(const geometry::Circle_2<K> &c, const Rectangle_2<K> &r) {
	return do_intersect(r,c);
}

}; // namespace geometry

#endif // GEOMETRY_CIRCLE_2_RECTANGLE_2_INTERSECTION_HPP
