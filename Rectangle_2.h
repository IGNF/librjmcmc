// Copyright (c) 1999  Utrecht University (The Netherlands),
// ETH Zurich (Switzerland), Freie Universitaet Berlin (Germany),
// INRIA Sophia-Antipolis (France), Martin-Luther-University Halle-Wittenberg
// (Germany), Max-Planck-Institute Saarbruecken (Germany), RISC Linz (Austria),
// and Tel-Aviv University (Israel).  All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; version 2.1 of the License.
// See the file LICENSE.LGPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL: ????????????
//
//
// Author(s)     : Mathieu Bredif

#ifndef CGAL_RECTANGLE_2_H
#define CGAL_RECTANGLE_2_H

#include <CGAL/basic.h>
#include <CGAL/Iso_rectangle_2.h>

#include <CGAL/Polygon_2.h>
//#include <CGAL/Polygon_with_holes_2.h>
//#include <CGAL/Boolean_set_operations_2.h>

#include <boost/array.hpp>

CGAL_BEGIN_NAMESPACE

template <class K> inline typename K::FT area(const std::vector<Point_2<K> > &p, const K& k=K()) {
  return polygon_area_2(p.begin(),p.end(),k);
}

template <class P> void collinear_orient(const P& a, const P& b, const P& p, const P& q, P& min, P& max) {
    if(CGAL::sign((b-a)*(q-p))==POSITIVE) {
	min = p; max = q;
    } else {
	min = q; max = p;
    }
}

template <class P> void branch(const P& a, const P& b, P& c, P& d, const P& e, const P& f) {
      typedef typename P::R                R;
      typedef typename R::Line_2           Line_2;
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
template <class L, class FT> void triangle_area(const L& a, const L& b, const L& c, FT& area) {
      FT wa  = b.a()*c.b()-b.b()*c.a();
      FT wb  = c.a()*a.b()-c.b()*a.a();
      FT wc  = a.a()*b.b()-a.b()*b.a();
      FT det = a.c()*wa+b.c()*wb+c.c()*wc;
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

template <class V, class FT> FT iso_combination_area_aux(const V& v, FT anbn, const V& an, FT ar, const V& bn, FT br) {
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


// intersection area between rectangle [-n2,n2]x[-m2,m2] and triangle (tx,ry),(rx,ry),(tx,ty)
template <class FT> inline FT triangle_area(FT n2, FT m2, FT tx, FT ty, FT rx, FT ry) {
	if(tx>=n2 || ry>=m2) return 0; // no intersection, above
	if(tx<-n2) { ty = ty+(tx+n2)*(ry-ty)/(tx-rx); tx=-n2; } // crop left
	if(ry<-m2) { rx = rx+(ry+m2)*(rx-tx)/(ty-ry); ry=-m2; } // crop bottom
	if(ty<ry) return 0;  // no intersection, underneath
	if(ty<=m2) {
		if(rx<=n2) return (ty-ry)*(rx-tx)/2; // all inside
		return (1+(rx-n2)/(rx-tx))*(ty-ry)*(n2-tx)/2; // cut right
	}
	if(rx<=n2) return (1+(ty-m2)/(ty-ry))*(rx-tx)*(m2-ry)/2; // cut top
	FT mx = tx+(m2-ty)*(rx-tx)/(ry-ty);
	if(mx>=n2) return (n2-tx)*(m2-ry);  // rectangle
	FT ny = ty+(tx-n2)*(ry-ty)/(tx-rx);
	return (mx-tx)*(m2-ry)+((m2+ny)/2-ry)*(n2-mx); // rectangle with the upper right corner cut
}


template <class R_>
class Rectangle_2
{
  typedef typename R_::RT                    RT;
  typedef typename R_::FT                    FT;
  typedef typename R_::Point_2               Point_2;
  typedef typename R_::Vector_2              Vector_2;
  typedef typename R_::Line_2                Line_2;
  typedef typename R_::Segment_2             Segment_2;
  typedef typename R_::Iso_rectangle_2       Iso_rectangle_2;
  typedef typename R_::Aff_transformation_2  Aff_transformation_2;
  typedef Rectangle_2                    Self;

  Point_2 c;
  Vector_2 n;
  FT r;

public:
  typedef  R_   R;

/* Constructors */
  Rectangle_2() : c(0.,0.), n(0.,0.), r(0.) {}

  Rectangle_2(const Point_2 &p, const Vector_2 &v, const FT &f)
    : c(p), n(v), r(f) {}

  Rectangle_2(const Point_2 &p0, const Point_2 &p1, const Point_2 &p2) : n(0.5*(p1-p0)) {
      FT n2 = n.squared_length();
      if(is_zero(n2)) {  // degeneracy p0=p1
        r = 0;
        c = midpoint(p0,p2);
        n = p2-c;
      } else {  // non-degenerate edge [p0,p1]
        Vector_2 m(-n.y(),n.x());
	r = 0.5*((p2-p1)*m)/n2;
	c = midpoint(p0,p1)+r*m;
      }
    }

  Rectangle_2(const Iso_rectangle_2 &i)
    : c(midpoint(i.min(),i.max())), n(0.,0.), r(0.) {
      Vector_2 v(i.max()-i.min());
      if       (!(CGAL_NTS is_zero(v.x()))) { n=Vector_2(0.5*v.x(),0.); r=v.y()/v.x();
      } else if(!(CGAL_NTS is_zero(v.y()))) { n=Vector_2(0.,0.5*v.y()); r=v.x()/v.y();
      }
    }

  Rectangle_2(const Point_2 &pmin, const Point_2 &pmax)
    : c(midpoint(pmin,pmax)), n(0.,0.), r(0.) {
      Vector_2 v(pmax-pmin);
      if       (!(CGAL_NTS is_zero(v.x()))) { n=Vector_2(0.5*v.x(),0.); r=v.y()/v.x();
      } else if(!(CGAL_NTS is_zero(v.y()))) { n=Vector_2(0.,0.5*v.y()); r=v.x()/v.y();
      }
    }

/* Equality */
  bool operator==(const Rectangle_2 &i) const
  {
    Vector_2 v(-n.y(),n.x());
    return c==i.c && ( (r==i.r       && (n==i.n || n==-i.n)) ||
                       (r*i.r==RT(1) && (v==i.n || v==-i.n)) );
  }

  inline bool operator!=(const Rectangle_2 &i) const
  {
    return ! (*this == i);
  }

/* Modifiers */
  inline void center(const Point_2& p ) { c=p; }
  inline void normal(const Vector_2& v) { n=v; }
  inline void ratio (FT f             ) { r=f; }

  inline void reverse_orientation () { r = -r; }
  inline Self opposite () { return Rectangle_2(c,n,-r); }

  Rectangle_2 transform(const Aff_transformation_2 &t) const
  {
   // correct result for reflections, translations, rotations,
   // isotropic scalings and their compositions
    return Rectangle_2(t.transform(c), t.transform(n), (t.is_even())?r:(-r));
   // shearing transformations should be avoided
   // but anisotropic scalings could be allowed if its eigen vectors
   // are aligned with the direction of the rectangle.
  }
/* Convenience Modifiers */
  inline void translate(const Vector_2& v ) { c=c+v; }
  inline void rotate   (const Vector_2& v ) { n=Vector_2(v*n,v.x()*n.y()-v.y()*n.x()); }
  inline void scale    (FT f              ) { n=f*n; }

/* Variants */
  Self translate_edges(int i, const FT &f) const {
    // translate edges(i) and (i+2) linearly around the center with f : 0->degenerate, 1->identity...
    if(i%2) return Rectangle_2(c,f*n,r/f);
    return Rectangle_2(c,n,f*r);
  }

  Self translate_edge(int i, const FT &f) const {
    // translates edge(i) linearly around edge(i+2) with f : 0->degenerate, 1->identity...
    // assert(!is_degenerate());
    Vector_2 m(-r*n.y(),r*n.x());
    switch (i%4) {
    case  0: return Rectangle_2(c+(1-f)*m, n,f*r);
    case  1: return Rectangle_2(c-(1-f)*n, m,f/r);
    case  2: return Rectangle_2(c-(1-f)*m,-n,f*r);
    default: return Rectangle_2(c+(1-f)*n,-m,f/r);
    }
  }

/* Split and merge */
  std::pair<Self,Self> split(int i, const FT &f=0.5) const {
    // optimized version of make_pair(scale(i,f),scale(i+2,1-f));
    Vector_2 m(-r*n.y(),r*n.x());
    switch (i%4) {
    case  0: { Point_2 p(c-f*m); return std::make_pair(Rectangle_2(p+m, n,f*r),Rectangle_2(p,-n,(1-f)*r)); }
    case  1: { Point_2 p(c+f*n); return std::make_pair(Rectangle_2(p-n, m,f/r),Rectangle_2(p,-m,(1-f)/r)); }
    case  2: { Point_2 p(c+f*m); return std::make_pair(Rectangle_2(p-m,-n,f*r),Rectangle_2(p, n,(1-f)*r)); }
    default: { Point_2 p(c-f*n); return std::make_pair(Rectangle_2(p+n,-m,f/r),Rectangle_2(p, m,(1-f)/r)); }
    }
  }

  // assymmetric
  // merge(rect.split(f)) == rect
  // moves one segment of this to match the supporting line of the furthest corresponding segment of a
  // rotated version of b. The rotation is the smallest of the 4 possible rotations to align b with this.
  // the moving segment is based on the quadrant (n+-45°,m+-45°,-n+-45° or -m+-45°) b.c is relative to this
  Self merge(const Rectangle_2 &b) const {
    Vector_2 v(c,b.c);
    FT ar(abs(r));
    Vector_2 an(n), am(-n.y(),n.x());
    FT dot =  an*v;
    FT det =  am*v;
    if(dot*dot>det*det) { v=am; am=-ar*an; an=ar*v; det = -ar*dot; ar=1/ar; }
    if(det<0) { an=-an; am=-am; det = -det; }
    // am is rotate90(an)
    // (c,an,ar) is a representation of this such that b.c is in the (am+-45°) quadrant

    FT f((det+max(abs(b.r*(an*b.n)),abs(-an.x()*b.n.y()+an.y()*b.n.x())))/am.squared_length());
    return Rectangle_2(c+((f-ar)/2)*am,an,((f+ar)/2));
  }




/* Predicates */
  inline bool is_simple() const { return true; }
  inline bool is_convex() const { return true; }

  inline Orientation orientation() const { return sign(r); }

  inline Oriented_side oriented_side(const Point_2& q) const { return Oriented_side(orientation()*bounded_side(q)); }

  Bounded_side  bounded_side(const Point_2& q) const {
   Vector_2 v(q-c);
   FT n2  = n.squared_length(); n2 *= n2;
   FT dot = n*v;
   Sign s0 = sign(dot*dot-n2);
   if(s0==POSITIVE) return ON_UNBOUNDED_SIDE; // outside slab line1-line3
   FT det = n.x()*v.y()-n.y()*v.x(); //=determinant(n,v);
   Sign s1 = sign(det*det-r*r*n2);
   if(s1==POSITIVE) return ON_UNBOUNDED_SIDE; // outside slab line0-line2
   return (s0==ZERO||s1==ZERO) ? ON_BOUNDARY : ON_BOUNDED_SIDE;
  }

/* Convenience Predicates */
  inline bool is_counterclockwise_oriented () const { return sign(r)==POSITIVE; }
  inline bool is_clockwise_oriented () const { return sign(r)==NEGATIVE; }
  inline bool is_collinear_oriented () const { return (CGAL_NTS is_zero(r)); }

  inline bool has_on_positive_side(const Point_2& q) const { return oriented_side(q)==ON_POSITIVE_SIDE; }
  inline bool has_on_negative_side(const Point_2& q) const { return oriented_side(q)==ON_NEGATIVE_SIDE; }

  inline bool has_on_boundary(const Point_2& q) const { return bounded_side(q)==ON_BOUNDARY; }
  inline bool has_on_bounded_side(const Point_2& q) const { return bounded_side(q)==ON_BOUNDED_SIDE; }
  inline bool has_on_unbounded_side(const Point_2& q) const { return bounded_side(q)==ON_UNBOUNDED_SIDE; }

/* Rectangle predicates */
  inline bool is_iso_rectangle() const { return (CGAL_NTS is_zero(n.x()*n.y())); }
  inline bool is_square() const { return (r*r==RT(1)); }
  inline bool is_degenerate() const { return CGAL_NTS is_zero(r*n.squared_length()); }

/* Access methods */
  Bbox_2 bbox() const {
    std::pair<double,double> cx = to_interval(c.x());
    std::pair<double,double> cy = to_interval(c.y());
    double nx = to_interval(abs(n.x())).second;
    double ny = to_interval(abs(n.y())).second;
    double rs = to_interval(abs(r)).second;
    double dx = nx+rs*ny;
    double dy = ny+rs*nx;
    return Bbox_2(cx.first-dx,cy.first-dy,cx.second+dx,cy.second+dy);
  }

  inline FT area() const { return r*4*n.squared_length(); }
  // optimized version of edge(i).squared_length();
  inline FT squared_length(int i) const { FT l2=4*n.squared_length(); return (i%2)?(r*r*l2):l2; }

  inline Point_2  center() const { return c; }
  inline Vector_2 normal() const { return n; }
  inline FT       ratio () const { return r; }

  inline int left_vertex   () const { return (r*n.y()>0)?((n.x()>0)?3:2):((n.x()>0)?0:1); }
  inline int right_vertex  () const { return (r*n.y()>0)?((n.x()>0)?1:0):((n.x()>0)?2:3); }
  inline int bottom_vertex () const { return (r*n.x()>0)?((n.y()>0)?0:1):((n.y()>0)?3:2); }
  inline int top_vertex    () const { return (r*n.x()>0)?((n.y()>0)?2:3):((n.y()>0)?1:0); }

/* Random access methods */
  Point_2 point(int i) const
  {
    Vector_2 m(-r*n.y(),r*n.x());
    switch (i%4) {
    case  0: return c-n-m;
    case  1: return c+n-m;
    case  2: return c+n+m;
    default: return c-n+m;
    }
  }

  void point(int i, boost::array<float,2> &pt) const
	{
		Vector_2 m(-r*n.y(),r*n.x());
		switch (i%4)
		{
			case 0:
			pt[0] = c[0]-n[0]-m[0];
			pt[1] = c[1]-n[1]-m[1];
			return;
			case 1:
			pt[0] = c[0]+n[0]-m[0];
			pt[1] = c[1]+n[1]-m[1];
			return;
			case 2:
			pt[0] = c[0]+n[0]+m[0];
			pt[1] = c[1]+n[1]+m[1];
			return;
			default:
			pt[0] = c[0]-n[0]+m[0];
			pt[1] = c[1]-n[1]+m[1];
			return;
	 	}
	}

  // point(i) aliases
  inline Point_2 vertex(int i) const { return point(i); }
  inline Point_2 operator[](int i) const { return point(i); }

  inline Segment_2 edge(int i) const // edge between point(i) and point(i+1)
  {
    Vector_2 m(-r*n.y(),r*n.x());
    switch (i%4) {
    case  0: { Point_2 p(c-m); return Segment_2(p-n,p+n); }
    case  1: { Point_2 p(c+n); return Segment_2(p-m,p+m); }
    case  2: { Point_2 p(c+m); return Segment_2(p+n,p-n); }
    default: { Point_2 p(c-n); return Segment_2(p+m,p-m); }
    }
  }

  Line_2 line(int i) const // line passing through point(i) and point(i+1)
  {
  // optimization: line(i)==edge(i).supporting_line();
    Vector_2 rn(r*n);
    FT dot(rn*n);
    switch (i%4) {
    case  0: return Line_2(- n.y(),  n.x(), c.x()* n.y()-c.y()* n.x()+dot);
    case  2: return Line_2(  n.y(),- n.x(),-c.x()* n.y()+c.y()* n.x()+dot);
    case  1: return Line_2(-rn.x(),-rn.y(), c.x()*rn.x()+c.y()*rn.y()+dot);
    default: return Line_2( rn.x(), rn.y(),-c.x()*rn.x()-c.y()*rn.y()+dot);
    }
  }

  Vector_2 normal(int i) const { // not normalized, points outside for counterclockwise oriented rectangles
    // optimization: normal(i).direction()==(point(i+1)-point(i)).perpendicular(CLOCKWISE).direction()
    switch (i%4) {
    case  0: return Vector_2( n.y(),-n.x());
    case  2: return Vector_2(-n.y(), n.x());
    case  1: return  r*n;
    default: return -r*n;
    }
  }

/* Polygon_2 Miscellaneous */
  inline int size() const { return 4; }
  inline bool is_empty() const { return false; }

  std::vector<Point_2> container (bool force_ccw=false) const {
    int i=(force_ccw && is_clockwise_oriented())?2:0;
    Point_2 points[] = {point(0),point(i+1),point(2),point(i+3)};
    return std::vector<Point_2>(points,points+4);
  }

/* Casting */

  Iso_rectangle_2 iso_rectangle() const {
    assert(is_iso_rectangle());
    return Iso_rectangle_2(point(0),point(2));
  }

/* Angle between 2 rectangle orientations */

  FT squared_cos(const Rectangle_2 &b) const {
    FT det = abs(n.x()*b.n.y()-n.y()*b.n.x());
    FT dot = abs(n*b.n);
    FT cos = max(det,dot);
    return  cos*cos/(n.squared_length()*b.n.squared_length());
  }


/* Intersection test */

  bool do_intersect(const Rectangle_2 &b) const {
    // optimized version of :
    // return intersection_area(b)>0;
    FT det = abs(n.x()*b.n.y()-n.y()*b.n.x());
    FT dot = abs(n*b.n);
    Vector_2 v(c,b.c);

    FT n2  = n.squared_length();
    FT br(abs(b.r));
    FT ax0 = n*v;
    FT dax = dot+br*det+n2;
    if(ax0*ax0>=dax*dax) return false;

    FT ar(abs(r));
    FT ay0 = n.x()*v.y()-n.y()*v.x();
    FT day = det+br*dot+ar*n2;
    if(ay0*ay0>=day*day) return false;

    FT bn2 = b.n.squared_length();
    FT bx0 = b.n*v;
    FT dbx = dot+ar*det+bn2;
    if(bx0*bx0>=dbx*dbx) return false;

    FT by0 = v.x()*b.n.y()-v.y()*b.n.x();
    FT dby = det+ar*dot+br*bn2;
    if(by0*by0>=dby*dby) return false;

    return true;
  }


/* Intersection */
  inline Object intersection(const Rectangle_2 &b) const { return intersection_two_slabs(b); }

  Object intersection_unoptimized(const Rectangle_2 &b) const {
    if(is_degenerate() || b.is_degenerate()) return Object();
    typedef std::vector<Point_2>               Container;
    typedef Polygon_2<R_, Container> Polygon_2;
    typedef Polygon_with_holes_2<R_, Container> Polygon_with_holes_2;
    std::vector<Point_2> pa(container(true)), pb(b.container(true));
    std::vector<Polygon_with_holes_2> res;
    Polygon_2 polya(pa.begin(),pa.end()), polyb(pb.begin(),pb.end());
    CGAL::intersection(polya,polyb,std::back_inserter(res));
    return (res.empty())?Object():make_object(res.back().outer_boundary().container());
  }

  Object intersection_two_slabs(const Rectangle_2 &b) const {
    if(is_degenerate() || b.is_degenerate()) return Object();
	std::vector<Vector_2> v;
	std::vector<Line_2> l;
	for(unsigned int i=0; i<4; ++i)	{ v.push_back(b[i]-ORIGIN); l.push_back(b.line(i)); }
	FT n2 = n.squared_length();
	FT rn2 = r*r*n2;
	Vector_2 vc(c-ORIGIN);
	Vector_2 trn(-r*n.y(),r*n.x());
	FT ctrn = vc*trn;
	FT cn = vc*n;

	Vector_2 ln[] = {  n, trn };
	FT lc[] = { cn+n2, ctrn+rn2, cn-n2, ctrn-rn2 };

 // basically it is the iterative intersection of the convex polygon initialized with b
 // with the 2 slabs line0/line2 and line1/line3.
	for(unsigned int i=0; i<2; ++i)	{
		int begin0=-1, end0=-1;
		int begin1=-1, end1=-1;
		unsigned int n=v.size();
		for(unsigned int j=0; j<n; ++j) {
			FT dot = ln[i]*v[j];
			if( dot>lc[i]) {
				if(begin1<=end1) begin1=j;
				if(end0==-1 && begin0!=-1) end0=j;
			} else {
				if(begin0<=end0) begin0=j;
				if(dot<lc[i+2]) {
					if(end1==-1 && begin1!=-1) end1=j;
				} else {
					if(begin1<=end1) begin1=j;
				}
			}
		}
		if(begin0==-1 || begin1==-1 ) return Object(); // outside the slab
		if(end0  ==-1) {
			if(begin0!=0) end0=0;
			else begin0=begin1;
		}
		if(end1  ==-1) {
			if(begin1!=0) end1=0;
			else {
				if(end0==-1) continue; // inside the slab
				begin1=begin0;
			}
		}

		std::vector<Vector_2> w;
		std::vector<Line_2> m;
		if(end0!=-1) {  // cut outside line(i+1)
			for(int j=begin1; j!=end0; j=(j+1)%n) {
				w.push_back(v[j]);
				m.push_back(l[j]);
			}
			Point_2 inter;
			Line_2 li(ln[i].x(),ln[i].y(),-lc[i]);
			CGAL::intersection(l[(end0+n-1)%n],li).assign(inter);
			w.push_back(inter-ORIGIN);
			m.push_back(li);
			m.push_back(l[(begin0+n-1)%n]);
			CGAL::intersection(li,m.back()).assign(inter);
			w.push_back(inter-ORIGIN);
		}
                if(end1!=-1) { // cut outside line(i+3)
			for(int j=begin0; j!=end1; j=(j+1)%n) {
				w.push_back(v[j]);
				m.push_back(l[j]);
			}
			Point_2 inter;
			Line_2 li(ln[i].x(),ln[i].y(),-lc[i+2]);
			CGAL::intersection(l[(end1+n-1)%n],li).assign(inter);
			w.push_back(inter-ORIGIN);
			m.push_back(li);
			m.push_back(l[(begin1+n-1)%n]);
			CGAL::intersection(li,m.back()).assign(inter);
			w.push_back(inter-ORIGIN);
		}
		std::swap(v,w);
		std::swap(l,m);
	}
	std::vector<Point_2> p;
	for(unsigned int i=0; i<v.size(); ++i)	{ p.push_back(ORIGIN+v[i]); }
	return make_object(p);
  }

/* Intersection area */
  inline FT intersection_area(const Rectangle_2 &b) const { return intersection_area_four_triangles_and_an_isorectangle(b); }

  FT intersection_area_two_slabs(const Rectangle_2 &b) const {
    if(is_degenerate() || b.is_degenerate()) return 0;
	std::vector<Vector_2> v;
	std::vector<Line_2> l;
	for(unsigned int i=0; i<4; ++i)	{ v.push_back(b[i]-ORIGIN); l.push_back(b.line(i)); }
	FT n2 = n.squared_length();
	FT rn2 = r*r*n2;
	Vector_2 vc(c-ORIGIN);
	Vector_2 trn(-r*n.y(),r*n.x());
	FT ctrn = vc*trn;
	FT cn = vc*n;

	Vector_2 ln[] = {  n, trn };
	FT lc[] = { cn+n2, ctrn+rn2, cn-n2, ctrn-rn2 };

 // basically it is the iterative intersection of the convex polygon initialized with b
 // with the 2 slabs line0/line2 and line1/line3.
	for(unsigned int i=0; i<2; ++i)	{
		int begin0=-1, end0=-1;
		int begin1=-1, end1=-1;
		unsigned int n=v.size();
		for(unsigned int j=0; j<n; ++j) {
			FT dot = ln[i]*v[j];
			if( dot>lc[i]) {
				if(end0==-1 && begin0!=-1) end0=j;
				if(begin1<=end1) begin1=j;
			} else {
				if(begin0<=end0) begin0=j;
				if(dot<lc[i+2]) {
					if(end1==-1 && begin1!=-1) end1=j;
				} else {
					if(begin1<=end1) begin1=j;
				}
			}
		}
		if(begin0==-1 || begin1==-1 ) return 0; // outside the slab
		if(end0  ==-1) {
			if(begin0!=0) end0=0;
			else begin0=begin1;
		}
		if(end1  ==-1) {
			if(begin1!=0) end1=0;
			else {
				if(end0==-1) continue; // inside the slab
				begin1=begin0;
			}
		}

		std::vector<Vector_2> w;
		std::vector<Line_2> m;
		if(end0!=-1) {  // cut outside line(i+1)
			for(int j=begin1; j!=end0; j=(j+1)%n) {
				w.push_back(v[j]);
				m.push_back(l[j]);
			}
			Point_2 inter;
			Line_2 li(ln[i].x(),ln[i].y(),-lc[i]);
			CGAL::intersection(l[(end0+n-1)%n],li).assign(inter);
			w.push_back(inter-ORIGIN);
			m.push_back(li);
			m.push_back(l[(begin0+n-1)%n]);
			CGAL::intersection(li,m.back()).assign(inter);
			w.push_back(inter-ORIGIN);
		}
                if(end1!=-1) { // cut outside line(i+3)
			for(int j=begin0; j!=end1; j=(j+1)%n) {
				w.push_back(v[j]);
				m.push_back(l[j]);
			}
			Point_2 inter;
			Line_2 li(ln[i].x(),ln[i].y(),-lc[i+2]);
			CGAL::intersection(l[(end1+n-1)%n],li).assign(inter);
			w.push_back(inter-ORIGIN);
			m.push_back(li);
			m.push_back(l[(begin1+n-1)%n]);
			CGAL::intersection(li,m.back()).assign(inter);
			w.push_back(inter-ORIGIN);
		}
		std::swap(v,w);
		std::swap(l,m);
	}
	std::vector<Point_2> p;
	for(unsigned int i=0; i<v.size(); ++i)	{ p.push_back(ORIGIN+v[i]); }
	FT area;
	area_2(p.begin(),p.end(),area);
	return abs(area);
  }


  FT intersection_area_four_triangles_and_an_isorectangle(const Rectangle_2 &b) const {
    if(is_degenerate() || b.is_degenerate()) return 0;
    	Vector_2 v(c,b.c);
    	Vector_2 m(-n.y(),n.x());
    	FT n2  = n.squared_length();
    	FT m2  = abs(r)*n2;
	FT br(abs(b.r));
    	FT cx = n*v;
    	FT cy = m*v;
    	FT nx = n*b.n;
    	FT ny = m*b.n;
	switch(sign(nx)) {
	case ZERO : { // m and b.n are collinear
		ny = abs(ny);
		FT mx = br*ny;
		FT lx = cx-mx;
		FT rx = cx+mx;
		FT by = cy-ny;
		FT ty = cy+ny;
        	if(rx<=-n2 || n2<=lx || ty<=-m2 || m2<=by) return 0;
		return (min(n2,rx)-max(-n2,lx))*(min(m2,ty)-max(-m2,by))/n2;
		}
	case NEGATIVE : // b.n =rotate180(b.n)
		{ nx = -nx; ny = -ny; }
	default : ;
	}
    	FT mx =-br*ny;
    	FT my = br*nx;
	switch(sign(ny)) {
	case ZERO : { // n and b.n are collinear
		FT lx = cx-nx;
		FT rx = cx+nx;
		FT by = cy-my;
		FT ty = cy+my;
        	if(rx<=-n2 || n2<=lx || ty<=-m2 || m2<=by) return 0;
		return (min(n2,rx)-max(-n2,lx))*(min(m2,ty)-max(-m2,by))/n2;
		}
	case NEGATIVE : // b.n =rotate90(b.n)
		nx = -nx; std::swap(nx,mx);
		ny = -ny; std::swap(ny,my);
	default : { // nx>0, ny>0, mx<0 and my>0 case
		FT sumx(nx+mx), sumy(ny+my);
		FT difx(nx-mx), dify(ny-my);
		FT x[] = { cx-sumx, cx+difx, cx+sumx, cx-difx }; // bottom, right, top, left
		FT y[] = { cy-sumy, cy+dify, cy+sumy, cy-dify };
	        if(y[0]>=m2 || y[2]<=-m2 || x[3]>=n2 || x[1]<=-n2) return 0; // one edge of "this" separates the 2 rectangles
		FT area =  triangle_area(n2, m2, x[2], y[2], x[1], y[1])
			+  triangle_area(m2, n2, y[3],-x[3], y[2],-x[2])
			+  triangle_area(n2, m2,-x[0],-y[0],-x[3],-y[3])
			+  triangle_area(m2, n2,-y[1], x[1],-y[0], x[0]);
		// iso-rectangle area
		FT lx = x[0], by = y[1];
		FT rx = x[2], ty = y[3];
		FT s  = 1;
		if(lx>rx) { std::swap(lx,rx); s=-s; }
		if(by>ty) { std::swap(by,ty); s=-s; }
		if(by>=m2 || ty<=-m2 || lx>=n2 || rx<=-n2) return area/n2;
		return (area + s*(min(n2,rx)-max(-n2,lx))*(min(m2,ty)-max(-m2,by)))/n2;
	}
	}

}

/* IGN/Matis specific */
  std::vector<Point_2> combination(const Rectangle_2 &b) const {
    if(  is_degenerate()) return b.container();
    if(b.is_degenerate()) return   container();

    // TODO : optimize
    Line_2 l0[] = {  line(0),  line(1),  line(2),  line(3)};
    Line_2 l1[] = {b.line(0),b.line(1),b.line(2),b.line(3)};
    Point_2 p[12];
    std::vector<Point_2> res;
    RT invalid_area2(-1.);
    FT area2(invalid_area2);

    for(int i=0; i<2; ++i) {
      for(int j=0; j<2; ++j) {
        if(!CGAL::intersection(l0[i  ],l1[j  ]).assign(p[0]) ||
           !CGAL::intersection(l0[i+2],l1[j  ]).assign(p[3]) ||
           !CGAL::intersection(l0[i+2],l1[j+2]).assign(p[6]) ||
           !CGAL::intersection(l0[i  ],l1[j+2]).assign(p[9]) ) continue;

        collinear_orient(p[9],p[0],  point(i  ),  point(i+1), p[ 8], p[ 1]);
        collinear_orient(p[3],p[6],  point(i+2),  point(i+3), p[ 2], p[ 7]);
        collinear_orient(p[0],p[3],b.point(j  ),b.point(j+1), p[11], p[ 4]);
        collinear_orient(p[6],p[9],b.point(j+2),b.point(j+3), p[ 5], p[10]);

        branch(p[9],p[0],p[ 1],p[ 2],p[3],p[6]);
        branch(p[0],p[3],p[ 4],p[ 5],p[6],p[9]);
        branch(p[3],p[6],p[ 7],p[ 8],p[9],p[0]);
        branch(p[6],p[9],p[10],p[11],p[0],p[3]);

        FT area_tmp;
        area_2(p,p+12,area_tmp);
        FT area_tmp2 = area_tmp*area_tmp;
        if(area2==invalid_area2 || area_tmp2 < area2) { area2 = area_tmp2; res = std::vector<Point_2>(p,p+12); }
      }
    }
    return res;
  }


  FT combination_area(const Rectangle_2 &b) const {
    if(  is_degenerate()) return abs(b.area());
    if(b.is_degenerate()) return abs(  area());

    FT det = n.x()*b.n.y()-n.y()*b.n.x();
    FT dot = n*b.n;
    FT ar(abs(r)), br(abs(b.r));
    Vector_2 v(c,b.c);
    if(is_zero(det)) {
    	if(dot>0) return iso_combination_area_aux(v,dot,n,ar,b.n,br);
	return iso_combination_area_aux(v,-dot,n,ar,-b.n,br);
    }

    if(is_zero(dot)) {
	Vector_2 tn(-n.y(),n.x());
    	if(det>0) return iso_combination_area_aux(v,det,tn,ar,b.n,br);
	return iso_combination_area_aux(v,-det,-tn,ar,b.n,br);
    }
    return abs(CGAL::area(combination(b)));
/*

    RT invalid_area(-1.);
    FT area_min(invalid_area);

    Vector_2 an(  n);
    Vector_2 bn(b.n);
    if(dot<0) { dot=-dot; bn=-b.n; }
    if(det<0) { det=-det; std::swap(an,bn); std::swap(ar,br); v=-v; }

    FT an2 = an*an;
    FT bn2 = bn*bn;
    FT den = 4*an2*bn2;
    FT den_det = den/det;
    FT den_dot = den/dot;
// tarn,tbrn; -tbrn,an; bn,tarn; an,bn;
    FT parallelogram_area[] = {ar*br*den_det, br*den_dot, ar*den_dot, den_det };



    for(int i=0; i<1; ++i) {
      for(int j=0; j<1; ++j) {
        if(!CGAL::intersection(l0[i  ],l1[j  ]).assign(p[0]) ||
           !CGAL::intersection(l0[i+2],l1[j  ]).assign(p[3]) ||
           !CGAL::intersection(l0[i+2],l1[j+2]).assign(p[6]) ||
           !CGAL::intersection(l0[i  ],l1[j+2]).assign(p[9]) ) continue;

        FT area = parallelogram_area[i+2*j];

        collinear_orient(p[9],p[0],  point(i  ),  point(i+1), p[ 8], p[ 1]);
        collinear_orient(p[3],p[6],  point(i+2),  point(i+3), p[ 2], p[ 7]);
        collinear_orient(p[0],p[3],b.point(j  ),b.point(j+1), p[11], p[ 4]);
        collinear_orient(p[6],p[9],b.point(j+2),b.point(j+3), p[ 5], p[10]);

//        branch_area(l1[j  ],l1[ j+1   ],l1[j+2],l0[i  ],area);
        branch_area(l1[j  ],l1[ j+1   ],l1[j+2],l0[i+2],area);
//        branch_area(l1[j+2],l1[(j+3)%4],l1[j  ],l0[i  ],area);
        branch_area(l1[j+2],l1[(j+3)%4],l1[j  ],l0[i+2],area);

        branch_area(l0[i  ],l0[ i+1   ],l0[i+2],l1[j  ],area);
//        branch_area(l0[i  ],l0[ i+1   ],l0[i+2],l1[j+2],area);
        branch_area(l0[i+2],l0[(i+3)%4],l0[i  ],l1[j  ],area);
//        branch_area(l0[i+2],l0[(i+3)%4],l0[i  ],l1[j+2],area);

        branch(p[9],p[0],p[ 1],p[ 2],p[3],p[6]);
        branch(p[0],p[3],p[ 4],p[ 5],p[6],p[9]);
        branch(p[3],p[6],p[ 7],p[ 8],p[9],p[0]);
        branch(p[6],p[9],p[10],p[11],p[0],p[3]);

        FT area_tmp;
        area_2(p,p+12,area_tmp);
        FT area_tmp = abs(area_tmp);
        if(area_min==invalid_area2 || area_tmp < area_min) { area_min=area_tmp; }
      }
    }
    return area_min;
*/
  }

  FT energy(const Rectangle_2 &b, FT f=1, FT g=1) const {
    if(f==0) return g*intersection_area(b);
    FT comb  = combination_area (b);
    FT inter = intersection_area(b);
    FT a0    = abs(area());
    FT a1    = abs(b.area());
    return f*(comb-a0-a1+inter)+g*inter;
  }
};


template <class K> typename K::FT area(const Object &obj, const K& k) {
  typedef typename K::FT                FT;
  typedef typename K::Point_2           Point_2;
  typedef typename K::Iso_rectangle_2   Iso_rectangle_2;
  typedef typename K::Triangle_2        Triangle_2;
  typedef std::vector<Point_2>          Point_2_vector;
  typedef Polygon_2<K,Point_2_vector>   Polygon_2;
  if (const Iso_rectangle_2 * r = object_cast<Iso_rectangle_2>(&obj)) return r->area();
  if (const Triangle_2      * t = object_cast<Triangle_2>     (&obj)) return t->area();
  if (const Point_2_vector  * v = object_cast<Point_2_vector> (&obj)) return polygon_area_2(v->begin(),v->end(),k);
  if (const Polygon_2       * p = object_cast<Polygon_2     > (&obj)) return p->area();
  return 0;
}

template <class T, class U> void intersection_area(const T &t, const U &u, const typename T::K& k=typename T::K()) {
  return area(intersection(t,u),k);
}

template <class K> inline bool do_intersect(const Rectangle_2<K> &r1, const Rectangle_2<K> &r2)
{
  return r1.do_intersect(r2);
}

template <class K> inline Object intersection(const Rectangle_2<K> &r1, const Rectangle_2<K> &r2) {
  return r1.intersection(r2);
}

template <class K> inline typename K::FT intersection_area(const Rectangle_2<K> &r1, const Rectangle_2<K> &r2) {
  return r1.intersection_area(r2);
}

template < class R >
std::ostream &
operator<<(std::ostream &os, const Rectangle_2<R> &r)
{
  switch(os.iword(IO::mode)) {
  case IO::ASCII :
    return os << r.center() << ' ' << r.normal() << ' ' << r.ratio();
  case IO::BINARY :
    return os << r.center() << r.normal() << r.ratio();
  default:
    return os << "Rectangle_2(" << r.center() << ", " << r.normal() << ", " << r.ratio() << ")";
  }
}

template < class R >
std::istream &
operator>>(std::istream &is, Rectangle_2<R> &b)
{
  typename R::Point_2 p;
  typename R::Vector_2 n;
  typename R::FT r;

  is >> p >> n >> r;

  if (is)
    b = Rectangle_2<R>(p, n, r);
  return is;
}


template<class Seg2> class Segment_2_iterator {
  typedef Seg2             Segment_2;
  typedef typename Segment_2::R      R;
  typedef typename R::FT             FT;
public:
  // rasterize the segment s on a pixel grid [i-f,i+1-f]x[j-f,j+1-f]
  // pixels centers are (i+0.5-f, j+0.5-f)
  Segment_2_iterator () : t(1) {}
  Segment_2_iterator (const Segment_2& s, FT f=0) : t(0) {
	for (int i = 0; i < 2; ++i) {
		p[i] = (int) std::floor(to_double(s.source()[i]+f));
		q[i] = (int) std::floor(to_double(s.target()[i]+f));
		if(p[i]==q[i]) { NextCrossingT[i]=2; Step[i]=0; DeltaT[i]=0; continue; }
		int dir = (q[i]>p[i]);
		FT invlength =  1.0f / (s.target()[i]-s.source()[i]);
		NextCrossingT[i] = (p[i]+dir -(s.source()[i]+f))*invlength;
		Step  [i] = 2*dir-1;
		DeltaT[i] = Step[i] * invlength;
	}
        m_axis = NextCrossingT[1] < NextCrossingT[0];
        if(p[m_axis]==q[m_axis]) NextCrossingT[m_axis]=1;
  }
  Segment_2_iterator operator++( int ) { Segment_2_iterator tmp(*this); operator++(); return tmp; }
  inline int axis() const { return m_axis; } // 0->x, 1->y
  inline int step() const { return Step[m_axis]; } // + or - 1
  Segment_2_iterator& operator++()
  {
    t = NextCrossingT[m_axis];
    p[m_axis] += step();
    NextCrossingT[m_axis] += DeltaT[m_axis];
    m_axis = NextCrossingT[1] < NextCrossingT[0];
    if(p[m_axis]==q[m_axis]) NextCrossingT[m_axis]=1;
    return *this;
  }
  inline bool end  () const { return t==1; }
  inline bool begin() const { return t==0; }
  inline int x() const { return p[0]; }
  inline int y() const { return p[1]; }
  inline FT prev  () const { return t; }
  inline FT next  () const { return NextCrossingT[m_axis]; }
  inline FT length() const { return NextCrossingT[m_axis]-t; }

private :
  int p[2], q[2], Step[2], m_axis;
  FT NextCrossingT[2], DeltaT[2], t;
};


template<class Rect2> class Rectangle_2_point_iterator {
  typedef Rect2                      Rectangle_2;
  typedef typename Rectangle_2::R    R;
  typedef typename R::FT             FT;
  typedef typename R::Point_2        Point_2;

public:
  Rectangle_2_point_iterator () {}
  Rectangle_2_point_iterator (const Rectangle_2& rec) {
    int b = rec.bottom_vertex(), t = b+2;
    int l = rec.  left_vertex(), r = l+2;
    Point_2 p[] = { rec[b],rec[l],rec[r],rec[t] };
    y_ = (int) std::ceil(to_double(p[0].y()));
    ly = (int) std::ceil(to_double(p[1].y()));
    ry = (int) std::ceil(to_double(p[2].y()));
    ty = (int) std::ceil(to_double(p[3].y()));
    if(rec.is_iso_rectangle()) {
	dxl[0]=dxl[1]=dxr[0]=dxr[1]=0;

	xl[0] = xl[1] = p[1].x();
	xr[0] = xr[1] = p[2].x();
    } else {
	dxl[0]=(p[1].x()-p[0].x())/(p[1].y()-p[0].y());
	dxl[1]=(p[3].x()-p[1].x())/(p[3].y()-p[1].y());
	dxr[0]=(p[2].x()-p[0].x())/(p[2].y()-p[0].y());
	dxr[1]=(p[3].x()-p[2].x())/(p[3].y()-p[2].y());

	xl[0] = p[0].x()+(y_-p[0].y())*dxl[0];
	xl[1] = p[1].x()+(ly-p[1].y())*dxl[1];
	xr[0] = p[0].x()+(y_-p[0].y())*dxr[0];
	xr[1] = p[2].x()+(ry-p[2].y())*dxr[1];
    }
    x_=rx=0; --y_;
    operator++();
  }
  Rectangle_2_point_iterator operator++( int ) { Rectangle_2_point_iterator tmp(*this); operator++(); return tmp; }
  Rectangle_2_point_iterator& operator++() {
    ++x_;
    while(x_>rx && y_<ty) {
    	++y_;
        int i=(y_>=ly);	x_ = (int) std::ceil (xl[i]); xl[i]+=dxl[i];
        int j=(y_>=ry);	rx = (int) std::floor(xr[j]); xr[j]+=dxr[j];
    }
    return *this;
  }
  inline bool end() const { return y_>=ty; }
  inline int x() const { return x_; }
  inline int y() const { return y_; }

private :
  int x_, y_, rx, ly, ry, ty;
  FT xl[2], xr[2], dxl[2], dxr[2];
};

/*
template<class Rect2> class Rectangle_2_square_iterator {
  typedef Rect2                      Rectangle_2;
  typedef typename Rectangle_2::R    R;
  typedef typename R::Segment_2      Segment_2;
  typedef Segment_2_iterator<Segment_2>  Iterator;
  typedef typename R::Point_2        Point_2;

public:
  Rectangle_2_square_iterator (const Rectangle_2& rec) {
    int b = rec.bottom_vertex(), t = b+2;
    int l = rec.  left_vertex(), r = l+2;
    Point_2 p[] = { rec[b],rec[l],rec[r],rec[t] };
    il[0] = Iterator(Segment_2(p[0],p[1]));
    il[1] = Iterator(Segment_2(p[1],p[3]));
    ir[0] = Iterator(Segment_2(p[0],p[2]));
    ir[1] = Iterator(Segment_2(p[2],p[3]));
    x_ = (int) std::floor(to_double(p[0].x()));
    y_ = (int) std::floor(to_double(p[0].y()));
    operator++();
  }
  Rectangle_2_square_iterator operator++( int ) { Rectangle_2_square_iterator tmp(*this); operator++(); return tmp; }
  Rectangle_2_square_iterator& operator++() {
    ++x_;
    while(!end() && (x_>=ir[0].x() || x_>=ir[1].x() ) ) {
	while(!il[0].end() && y_==il[0].y()  ) ++(il[0]);
	while(!ir[0].end() && y_==ir[0].y()  ) ++(ir[0]);
	while(!il[1].end() && y_+1>=il[1].y()) ++(il[1]);
	while(!ir[1].end() && y_+1>=ir[1].y()) ++(ir[1]);
	++y_;
	x_=max(il[0].x(),il[1].x())+1;
    }
    return *this;
  }
  inline bool end() const { return il[1].end() || ir[1].end(); }
  inline int x() const { return x_; }
  inline int y() const { return y_; }

private :
  int x_, y_;
  Iterator il[2], ir[2];
};
*/
CGAL_END_NAMESPACE

#endif // CGAL_RECTANGLE_2_H
