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

#ifndef GEOMETRY_RECTANGLE_2_COMBINATION_HPP
#define GEOMETRY_RECTANGLE_2_COMBINATION_HPP

#include <CGAL/Polygon_2_algorithms.h>
#include "intersection/Rectangle_2_intersection.hpp"

namespace geometry {

namespace Impl {
template <class K> void collinear_orient(const typename K::Point_2& a, const typename K::Point_2& b, const typename K::Point_2& p, const typename K::Point_2& q, typename K::Point_2& min, typename K::Point_2& max) {
    if(CGAL::sign((b-a)*(q-p))==POSITIVE) {
  min = p; max = q;
    } else {
  min = q; max = p;
    }
}

template <class K> void branch(const typename K::Point_2& a, const typename K::Point_2& b, typename K::Point_2& c, typename K::Point_2& d, const typename K::Point_2& e, const typename K::Point_2& f) {
      typedef typename K::Line_2     Line_2;
      if(!collinear_are_ordered_along_line(a,b,c)) {
  if(!collinear_are_ordered_along_line(d,e,f)) {
    c=b; d=e; // no branch
  } else { // triangle
    if(!intersection(Line_2(b,e),Line_2(c,d)).assign(c)) {
      c=b; d=e;
    }
  }
      } else if(!collinear_are_ordered_along_line(d,e,f)) { //triangle
  if(!intersection(Line_2(b,e),Line_2(c,d)).assign(d)) {
    c=b; d=e;
  }
      } else {// noop, trapezoid
      }
}

/*
// drafts to optimize combination_area
template <class L, class FT> void triangle_area(const L& a, const L& b, const L& c, FT& area) {
      FT wa  = b.a()*c.b()-b.b()*c.a();
      FT wb  = c.a()*a.b()-c.b()*a.a();
      FT wc  = a.a()*b.b()-a.b()*b.a();
      FT det = a.center()()*wa+b.center()()*wb+c.c()*wc;
      FT f = det*det/(2*wa*wb*wc);
      area += f;
}

template <class P, class FT> void branch_area(const P& a, const P& b, const P& c, const P& d, const P& e, const P& f, FT& area) {
      typedef typename P::R       R;
      typedef typename R::Line_2  Line_2;
      Line_2 i(b,c);
      Line_2 j(c,d);
      Line_2 k(d,e);
      Line_2 l(e,b);

     if((a.x()*l.a()+a.y()*l.b()+l.c())*(c.x()*l.a()+c.y()*l.b()+l.c())<0) triangle_area(i,j,l,area);
     if((a.x()*l.a()+a.y()*l.b()+l.c())*(d.x()*l.a()+d.y()*l.b()+l.c())<0) triangle_area(j,k,l,area);
}

template <class L, class FT> void branch_area(const L& a, const L& b, const L& c, const L& d, FT& area) {
      typedef typename L::R       R;
      typedef typename R::Point_2  Point_2;
     Point_2 p;
     intersection(a,b).assign(p);
     if((p.x()*d.a()+p.y()*d.b()+d.c())<0) triangle_area(a,b,d,area);
     intersection(b,c).assign(p);
     if((p.x()*d.a()+p.y()*d.b()+d.c())<0) triangle_area(b,c,d,area);
}
*/

template <class K> typename K::FT iso_combination_area_aux(const typename K::Vector_2& v, typename K::FT anbn, const typename K::Vector_2& an, typename K::FT ar, const typename K::Vector_2& bn, typename K::FT br) {
  typedef typename K::FT FT;
  typedef typename K::Vector_2 V;

  FT an2  =an*an;
  FT bn2 = bn*bn;

  V atn(-an.y(),an.x());
  V artn(ar*atn);

  V btn(-bn.y(),bn.x());
  V brtn(br*btn);

  FT an2_anbn(an2-anbn);
  FT bn2_anbn(bn2-anbn);
  FT arnbn(ar*anbn);
  FT anbrn(br*anbn);
  FT brn2_arnbn(br*bn2-arnbn);
  FT arn2_anbrn(ar*an2-anbrn);

  FT rect0(0), rect1(0), rect2(0), rect3(0);

  if(an2_anbn  >0) { rect0=2*an2_anbn;   } else { rect1=2*bn2_anbn;   }
  if(arn2_anbrn>0) { rect3=2*arn2_anbrn; } else { rect2=2*brn2_arnbn; }

  FT anv =  an*v;
  FT btnv= btn*v;
  Sign sa=sign(anv);
  Sign sb=sign(btnv);
  FT anv_an2_anbn   (sa*anv+an2_anbn);
  FT btnv_brn2_arnbn(sb*btnv+brn2_arnbn);

  if(anv_an2_anbn   >rect0) { rect0=anv_an2_anbn;    rect1=sa*( bn*v)+bn2_anbn;   }
  if(btnv_brn2_arnbn>rect2) { rect2=btnv_brn2_arnbn; rect3=sb*(atn*v)+arn2_anbrn; }

  FT area0(rect2+ar*rect0+2*arnbn);
  FT area1(rect3+br*rect1+2*anbrn);
  return 2*((area0<area1)?area0:area1);
}

} // namespace Impl


template<class K> std::vector<typename K::Point_2> combination(const Rectangle_2<K> &a, const Rectangle_2<K> &b) {
  typedef typename K::Line_2 Line_2;
  typedef typename K::Point_2 Point_2;
  typedef typename K::FT FT;
  typedef typename K::RT RT;

    if(a.is_degenerate()) return b.container();
    if(b.is_degenerate()) return a.container();

    // TODO : optimize
    Line_2 l0[] = {a.line(0),a.line(1),a.line(2),a.line(3)};
    Line_2 l1[] = {b.line(0),b.line(1),b.line(2),b.line(3)};
    Point_2 p[12];
    std::vector<Point_2> res;
    RT invalid_area2(-1.);
    FT area2(invalid_area2);

    for(int i=0; i<2; ++i) {
      for(int j=0; j<2; ++j) {
  if(!intersection(l0[i  ],l1[j  ]).assign(p[0]) ||
     !intersection(l0[i+2],l1[j  ]).assign(p[3]) ||
     !intersection(l0[i+2],l1[j+2]).assign(p[6]) ||
     !intersection(l0[i  ],l1[j+2]).assign(p[9]) ) continue;

  Impl::collinear_orient<K>(p[9],p[0],a.point(i  ),a.point(i+1), p[ 8], p[ 1]);
  Impl::collinear_orient<K>(p[3],p[6],a.point(i+2),a.point(i+3), p[ 2], p[ 7]);
  Impl::collinear_orient<K>(p[0],p[3],b.point(j  ),b.point(j+1), p[11], p[ 4]);
  Impl::collinear_orient<K>(p[6],p[9],b.point(j+2),b.point(j+3), p[ 5], p[10]);

  Impl::branch<K>(p[9],p[0],p[ 1],p[ 2],p[3],p[6]);
  Impl::branch<K>(p[0],p[3],p[ 4],p[ 5],p[6],p[9]);
  Impl::branch<K>(p[3],p[6],p[ 7],p[ 8],p[9],p[0]);
  Impl::branch<K>(p[6],p[9],p[10],p[11],p[0],p[3]);

  FT area_tmp;
  area_2(p,p+12,area_tmp);
  FT area_tmp2 = area_tmp*area_tmp;
  if(area2==invalid_area2 || area_tmp2 < area2) { area2 = area_tmp2; res = std::vector<Point_2>(p,p+12); }
      }
    }
    return res;
  }


template<class K> typename K::FT combination_area(const Rectangle_2<K> &a, const Rectangle_2<K> &b) {
  typedef typename K::Line_2 Line_2;
  typedef typename K::Vector_2 Vector_2;
  typedef typename K::Point_2 Point_2;
  typedef typename K::FT FT;
  typedef typename K::RT RT;

    if(a.is_degenerate()) return abs(b.area());
    if(b.is_degenerate()) return abs(a.area());

    FT det = a.normal().x()*b.normal().y()-a.normal().y()*b.normal().x();
    FT dot = a.normal()*b.normal();
    FT ar(abs(a.ratio())), br(abs(b.ratio()));
    Vector_2 v(a.center(),b.center());
    if(is_zero(det)) {
      if(dot>0) return Impl::iso_combination_area_aux<K>(v,dot,a.normal(),ar,b.normal(),br);
  return Impl::iso_combination_area_aux<K>(v,-dot,a.normal(),ar,-b.normal(),br);
    }

    if(is_zero(dot)) {
  Vector_2 tn(-a.normal().y(),a.normal().x());
      if(det>0) return Impl::iso_combination_area_aux<K>(v,det,tn,ar,b.normal(),br);
  return Impl::iso_combination_area_aux<K>(v,-det,-tn,ar,b.normal(),br);
    }

    Line_2 l0[] = {a.line(0),a.line(1),a.line(2),a.line(3)};
    Line_2 l1[] = {b.line(0),b.line(1),b.line(2),b.line(3)};
    Point_2 p[12];
    RT invalid_area(-1.);
    FT area_min(invalid_area);
/*
    Vector_2 an(a.normal());
    Vector_2 bn(b.normal());
    if(dot<0) { dot=-dot; bn=-b.normal(); }
    if(det<0) { det=-det; std::swap(an,bn); std::swap(ar,br); v=-v; }

    FT an2 = an*an;
    FT bn2 = bn*bn;
    FT den = 4*an2*bn2;
    FT den_det = den/det;
    FT den_dot = den/dot;
// tarn,tbrn; -tbrn,an; bn,tarn; an,bn;
    FT parallelogram_area[] = {ar*br*den_det, br*den_dot, ar*den_dot, den_det };
*/
    for(int i=0; i<1; ++i) {
      for(int j=0; j<1; ++j) {
  if(!intersection(l0[i  ],l1[j  ]).assign(p[0]) ||
     !intersection(l0[i+2],l1[j  ]).assign(p[3]) ||
     !intersection(l0[i+2],l1[j+2]).assign(p[6]) ||
     !intersection(l0[i  ],l1[j+2]).assign(p[9]) ) continue;

//  FT area = parallelogram_area[i+2*j];

  Impl::collinear_orient<K>(p[9],p[0],a.point(i  ),a.point(i+1), p[ 8], p[ 1]);
  Impl::collinear_orient<K>(p[3],p[6],a.point(i+2),a.point(i+3), p[ 2], p[ 7]);
  Impl::collinear_orient<K>(p[0],p[3],b.point(j  ),b.point(j+1), p[11], p[ 4]);
  Impl::collinear_orient<K>(p[6],p[9],b.point(j+2),b.point(j+3), p[ 5], p[10]);

//  branch_area(l1[j  ],l1[ j+1   ],l1[j+2],l0[i  ],area);
//  branch_area(l1[j  ],l1[ j+1   ],l1[j+2],l0[i+2],area);
//  branch_area(l1[j+2],l1[(j+3)%4],l1[j  ],l0[i  ],area);
//  branch_area(l1[j+2],l1[(j+3)%4],l1[j  ],l0[i+2],area);

//       branch_area(l0[i  ],l0[ i+1   ],l0[i+2],l1[j  ],area);
//  branch_area(l0[i  ],l0[ i+1   ],l0[i+2],l1[j+2],area);
//      branch_area(l0[i+2],l0[(i+3)%4],l0[i  ],l1[j  ],area);
//  branch_area(l0[i+2],l0[(i+3)%4],l0[i  ],l1[j+2],area);

  Impl::branch<K>(p[9],p[0],p[ 1],p[ 2],p[3],p[6]);
  Impl::branch<K>(p[0],p[3],p[ 4],p[ 5],p[6],p[9]);
  Impl::branch<K>(p[3],p[6],p[ 7],p[ 8],p[9],p[0]);
  Impl::branch<K>(p[6],p[9],p[10],p[11],p[0],p[3]);

  FT area_tmp;
  area_2(p,p+12,area_tmp);
  area_tmp = abs(area_tmp);
  if(area_min==invalid_area || area_tmp < area_min) { area_min=area_tmp; }
      }
    }
    return area_min;
  }


}; // namespace geometry

#endif // GEOMETRY_RECTANGLE_2_COMBINATION_HPP
