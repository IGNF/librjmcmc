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

#ifndef GEOMETRY_RECTANGLE_2_HPP
#define GEOMETRY_RECTANGLE_2_HPP
#include <utility> // for std::pair
#include <vector>

#include "geometry.hpp"

namespace geometry {
/**
 * \ingroup GroupGeometry
 */
template <class R_>
class Rectangle_2
{
public:
	  typedef  R_   R;
	  typedef typename R_::Point_2               Point_2;
	  typedef typename R_::Vector_2              Vector_2;
  	  typedef typename R_::FT                    FT;

private:
  typedef typename R_::RT                    RT;
  typedef typename R_::Line_2                Line_2;
  typedef typename R_::Segment_2             Segment_2;
  typedef typename R_::Iso_rectangle_2       Iso_rectangle_2;
/*
  typedef typename R_::Aff_transformation_2  Aff_transformation_2;
*/
  typedef Rectangle_2                        Self;
  Point_2 c;
  Vector_2 n;
  FT r;

public:

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
/*
  Rectangle_2(const Iso_rectangle_2 &i)
    : c(midpoint(i.min(),i.max())), n(0.,0.), r(0.) {
      Vector_2 v(i.max()-i.min());
      if       (!(is_zero(v.x()))) { n=Vector_2(0.5*v.x(),0.); r=v.y()/v.x();
      } else if(!(is_zero(v.y()))) { n=Vector_2(0.,0.5*v.y()); r=v.x()/v.y();
      }
    }
*/
  Rectangle_2(const Segment_2 &s)
    : c(midpoint(s.source(),s.target())), n(c,s.target()), r(0.) {}

  Rectangle_2(const Point_2 &pmin, const Point_2 &pmax)
    : c(midpoint(pmin,pmax)), n(0.,0.), r(0.) {
      Vector_2 v(pmax-pmin);
      if       (!(is_zero(v.x()))) { n=Vector_2(0.5*v.x(),0.); r=v.y()/v.x();
      } else if(!(is_zero(v.y()))) { n=Vector_2(0.,0.5*v.y()); r=v.x()/v.y();
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
/*
  Rectangle_2 transform(const Aff_transformation_2 &t) const
  {
   // correct result for reflections, translations, rotations,
   // isotropic scalings and their compositions
    return Rectangle_2(t.transform(c), t.transform(n), (t.is_even())?r:(-r));
   // shearing transformations should be avoided
   // but anisotropic scalings could be allowed if its eigen vectors
   // are aligned with the direction of the rectangle.
  }
*/
/* Convenience Modifiers */
  inline void translate       (const Vector_2& v ) { c=c+v; }
  // if v is (r.cost,r.sint), scale_and_rotate scales the rectangle by r and rotates it by t.
  inline void scale_and_rotate(const Vector_2& v ) { n=Vector_2(v.x()*n.x()-v.y()*n.y(),v.y()*n.x()+v.x()*n.y()); }
  inline void scale           (const FT &f       ) { n=f*n; }
  inline void reverse_orientation () { r = -r; }

  inline Rectangle_2 rotate(int i) const {
    switch (i%4) {
    case  0: return Rectangle_2(c, n,r);
    case  2: return Rectangle_2(c,-n,r);
    case  1: { Vector_2 m(-r*n.y(), r*n.x()); return Rectangle_2(c,m,1/r); }
    default: { Vector_2 m( r*n.y(),-r*n.x()); return Rectangle_2(c,m,1/r); }
    }
  }
/* Variants */
/*
  Self offset(const FT& d) const { // requires sqrt
        FT l = sqrt(n.squared_length());
        Sign s = sign(r);
        return Rectangle_2(c,n*(l+d)/l,s*(s*r*l+d)/(l+d));
  }
*/
  // if v is (r.cost,r.sint), scale_and_rotate scales the rectangle by r and rotates it by t.
  inline Rectangle_2 translated       (const Vector_2& v ) const { return Rectangle_2(c+v,n,r); }
  // if v is (r.cost,r.sint), scale_and_rotate scales the rectangle by r and rotates it by t.
  inline Rectangle_2 scaled_and_rotated(const Vector_2& v ) const { return Rectangle_2(c,Vector_2(v.x()*n.x()-v.y()*n.y(),v.y()*n.x()+v.x()*n.y()),r); }
  inline Rectangle_2 opposite() const { return Rectangle_2(c,n,-r); }
  inline Rectangle_2 reversed_orientation () const { return opposite(); }
  inline Rectangle_2 scaled(const FT &f) const { return Rectangle_2(c,f*n,r); }

  Rectangle_2 scaled_center(int i, const FT &f) const {
    // translate edges(i) and (i+2) linearly from the center with f : 0->degenerate, 1->identity...
    if(i%2) return Rectangle_2(c,f*n,r/f);
    return Rectangle_2(c,n,f*r);
  }

  Rectangle_2 scaled_edge(int i, const FT &f) const {
    // translates edge(i) linearly from edge(i+2) with f : 0->degenerate, 1->identity...
    // assert(!is_degenerate());
    Vector_2 m(-r*n.y(),r*n.x());
    switch (i%4) {
    case  0: return Rectangle_2(c+(1-f)*m, n,f*r);
    case  1: return Rectangle_2(c-(1-f)*n, m,f/r);
    case  2: return Rectangle_2(c-(1-f)*m,-n,f*r);
    default: return Rectangle_2(c+(1-f)*n,-m,f/r);
    }
  }
  inline Rectangle_2 rotation_scaled_corner(int i, const Vector_2& v) const {
    Vector_2 u(-r*v.y(),r*v.x());
    switch (i%4) {
    case  0: return Rectangle_2(c+v+u, n+v,r);
    case  1: return Rectangle_2(c-v+u, n+v,r);
    case  2: return Rectangle_2(c-v-u, n+v,r);
    default: return Rectangle_2(c+v-u, n+v,r);
    }
  }

/* Split and merge */
  std::pair<Rectangle_2,Rectangle_2> split(int i, const FT &f=0.5) const {
    // optimized version of make_pair(scale_edge(i,f),scale_edge(i+2,1-f));
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
  // the moving segment is based on the quadrant (n+-45deg,m+-45deg,-n+-45deg or -m+-45deg) b.c is relative to this
  Rectangle_2 merge(const Rectangle_2 &b) const {
    Vector_2 v(b.c-c);
    FT ar(abs(r));
    Vector_2 an(n), am(-n.y(),n.x());
    FT dot =  an*v;
    FT det =  am*v;
    if(dot*dot>det*det) { v=am; am=-ar*an; an=ar*v; det = -ar*dot; ar=1/ar; }
    if(det<0) { an=-an; am=-am; det = -det; }
    // am is rotate90(an)
    // (c,an,ar) is a representation of this such that b.c is in the (am+-45deg) quadrant

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
  inline bool is_counterclockwise_oriented () const { return r>0; }
  inline bool is_clockwise_oriented () const { return r<0; }
  inline bool is_collinear_oriented () const { return r==0; }

  inline bool has_on_positive_side(const Point_2& q) const { return oriented_side(q)==ON_POSITIVE_SIDE; }
  inline bool has_on_negative_side(const Point_2& q) const { return oriented_side(q)==ON_NEGATIVE_SIDE; }

  inline bool has_on_boundary(const Point_2& q) const { return bounded_side(q)==ON_BOUNDARY; }
  inline bool has_on_bounded_side(const Point_2& q) const { return bounded_side(q)==ON_BOUNDED_SIDE; }
  inline bool has_on_unbounded_side(const Point_2& q) const { return bounded_side(q)==ON_UNBOUNDED_SIDE; }

/* Rectangle predicates */
  inline bool is_iso_rectangle() const { return (is_zero(n.x()*n.y())); }
  inline bool is_square() const { return (r*r==RT(1)); }
  inline bool is_degenerate() const { return is_zero(r*n.squared_length()); }

/* Access methods */

  Iso_rectangle_2 bbox() const {
    FT nx = abs(n.x());
    FT ny = abs(n.y());
    FT rs = abs(r);
    FT dx = nx+rs*ny;
    FT dy = ny+rs*nx;
    return Iso_Rectangle_2(c.x()-dx,c.y()-dy,c.x()+dx,c.y()+dy);
  }

  // returns the positive scale so that q in on the boundary of "scaled this"
  // bounded_side = sign(scale-1)
  FT scale(const Point_2& q) const {
   Vector_2 v(q-c);
   FT n2  = n.squared_length();
   FT x = abs(n*v/n2);
   FT y = abs((n.x()*v.y()-n.y()*v.x())/(r*n2));
   return max(x,y);
  }
  FT scale_center(int i, const Point_2& q) const {
   Vector_2 v(q-c);
   FT n2  = n.squared_length();
   switch(i%4) {
   case 0 : return -(n.x()*v.y()-n.y()*v.x())/(r*n2);
   case 1 : return  n*v/n2;
   case 2 : return  (n.x()*v.y()-n.y()*v.x())/(r*n2);
   default: return -n*v/n2;
   }
  }
  inline FT scale_edge(int i, const Point_2& q) const { return (1+scale_center(i,q))*0.5; }

  inline FT area() const { return 4*r*n.squared_length(); }
  inline FT squared_perimeter() const { FT f(4*(1+r)); return f*f*n.squared_length(); }

  // optimized version of segment(i).squared_length();
  inline FT squared_length(int i) const { FT f(4*n.squared_length()); return (i%2)?(r*r*f):f; }

  inline const Point_2&  center() const { return c; }
  inline const Vector_2& normal() const { return n; }
  inline const FT&       ratio () const { return r; }

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

  // point(i) aliases
  inline Point_2 operator[](int i) const { return point(i); }

  Segment_2 segment(int i) const // segment between point(i) and point(i+1)
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
  // return segment(i).supporting_line(); // optimized version :
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

/*
  Iso_rectangle_2 iso_rectangle() const {
    assert(is_iso_rectangle());
    return Iso_rectangle_2(point(0),point(2));
  }
*/

/* Angle between 2 rectangle orientations */

  FT squared_cos(const Rectangle_2 &b) const {
    FT det = abs(n.x()*b.n.y()-n.y()*b.n.x());
    FT dot = abs(n*b.n);
    FT cos = max(det,dot);
    return  cos*cos/(n.squared_length()*b.n.squared_length());
  }

};


/****************************************/
/*         IO stream function           */
/****************************************/

#include <iostream>

template < class R >
std::ostream &
operator<<(std::ostream &os, const Rectangle_2<R> &r)
{
  return os << r.center() << ' ' << r.normal() << ' ' << r.ratio();
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

}; // namespace geometry

#endif // GEOMETRY_RECTANGLE_2_HPP
