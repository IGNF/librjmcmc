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

#ifndef GEOMETRY_GEOMETRY_HPP
#define GEOMETRY_GEOMETRY_HPP

#include <cmath>
#ifndef M_PI
const double M_PI = 4.0 * atan(1.0);
#endif // #ifndef M_PI

#include <iostream>
#include <algorithm> // min & max

#if USE_CGAL

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Iso_rectangle_2.h>

namespace geometry {
using namespace CGAL;

template <typename K>
struct Iso_rectangle_2_traits {
  typedef Iso_rectangle_2<K> type;
};

#else

namespace geometry {

enum Orientation { CLOCKWISE=-1, DEGENERATE=0, COUNTERCLOCKWISE=1 };
enum Oriented_side { ON_NEGATIVE_SIDE=-1, ON_ORIENTED_BOUNDARY=0, ON_POSITIVE_SIDE=1 };
enum Bounded_side { ON_BOUNDED_SIDE=-1, ON_BOUNDARY=0, ON_UNBOUNDED_SIDE=1 } ;
enum Sign { NEGATIVE=-1, ZERO=0, POSITIVE=1 } ;

struct Origin {};

template <typename T> struct Simple_cartesian;

namespace internal {
  template <typename T> class Vector_2;
  template <typename T> class Point_2 {
    T c_[2];
  public:
    Point_2(       ) { c_[0]= 0; c_[1]=0; }
    Point_2(T x,T y) { c_[0]= x; c_[1]=y; }
    inline T x() const { return c_[0]; }
    inline T y() const { return c_[1]; }
    inline T operator[](unsigned int i) const { return c_[i]; }
    inline Vector_2<T> operator-(const Point_2& p) const { return Vector_2<T>(x()-p.x(),y()-p.y()); }
    inline Point_2 operator+(const Vector_2<T>& v) const { return Point_2(x()+v.x(),y()+v.y()); }
    inline Point_2 operator-(const Vector_2<T>& v) const { return Point_2(x()-v.x(),y()-v.y()); }
    inline Vector_2<T> operator-(Origin) const { return Vector_2<T>(x(),y()); }
  };
  template <typename T>  class Vector_2 {
    T c_[2];
  public:
    Vector_2(       ) { c_[0]= 0; c_[1]=0; }
    Vector_2(T x,T y) { c_[0]= x; c_[1]=y; }
    inline T x() const { return c_[0]; }
    inline T y() const { return c_[1]; }
    inline T operator[](unsigned int i) const { return c_[i]; }
    inline Vector_2 perpendicular(Sign s) const { return Vector_2(-s*y(),s*x()); }
    inline T squared_length() const { return x()*x()+y()*y(); }
    template<typename U> inline Vector_2 operator*(U u) const { return Vector_2(u*x(),u*y()); }
    inline T operator*(const Vector_2& v) const { return x()*v.x()+y()*v.y(); }
    inline Vector_2 operator+(const Vector_2& v) const { return Vector_2(x()+v.x(),y()+v.y()); }
    inline Vector_2 operator-() const { return Vector_2(-x(),-y()); }
    inline Vector_2 operator*(const T& v) const { return Vector_2(x()*v,y()*v); }
    inline Vector_2 operator/(const T& v) const { return Vector_2(x()/v,y()/v); }
  };
  template <typename T>  class Line_2 {
    T c_[3];
  public:
    Line_2(T a, T b, T c) { c_[0]=a; c_[1]=b; c_[2]=c; }
  };
  template <typename T>  class Segment_2 {
    Point_2<T> p_[2];
  public:
    typedef Simple_cartesian<T> R;
    Segment_2(const Point_2<T>& p, const Point_2<T>& q) { p_[0]=p; p_[1]=q; }
    inline const Point_2<T>& source() const { return p_[0]; }
    inline const Point_2<T>& target() const { return p_[1]; }
  };

  template <typename T> inline Point_2<T> operator+(Origin, Vector_2<T>& v) { return Point_2<T>(v.x(),v.y()); }

  template <typename T> class Iso_rectangle_2 {
    Point_2<T> p_[2];
  public:
    Iso_rectangle_2() {}
    Iso_rectangle_2(const Point_2<T>& p, const Point_2<T>& q) {
      p_[0]=p; p_[1]=q;
    }
    Iso_rectangle_2(T x0, T y0, T x1, T y1) {
      p_[0]=Point_2<T>(x0,y0); p_[1]=Point_2<T>(x1,y1);
    }
    inline const Point_2<T>& min() const { return p_[0]; }
    inline const Point_2<T>& max() const { return p_[1]; }
    inline const Point_2<T>& vertex(int i) const { return p_[i%2]; }
    inline const Point_2<T>& operator[](int i) const { return p_[i%2]; }
    T area() const { return (p_[1].x()-p_[0].x())*(p_[1].y()-p_[0].y()); }

    inline bool is_degenerate () const { return (p_[1].x()==p_[0].x()) || (p_[1].y()==p_[0].y()); }

    Bounded_side bounded_side ( const Point_2<T>& p) const {
      if(p.x()<p_[0].x() || p.x()>p_[1].x() || p.y()<p_[0].y() || p.y()>p_[1].y())
        return ON_UNBOUNDED_SIDE;
      if(p.x()>p_[0].x() && p.x()<p_[1].x() && p.y()>p_[0].y() && p.y()<p_[1].y())
        return ON_BOUNDED_SIDE;
      return ON_BOUNDARY;
    }

    inline bool has_on_boundary ( const Point_2<T>& p) const {
      return bounded_side(p)==ON_BOUNDARY;
    }
    inline bool has_on_bounded_side ( const Point_2<T>& p) const {
      return (p.x()>p_[0].x() && p.x()<p_[1].x() && p.y()>p_[0].y() && p.y()<p_[1].y());
    }
    inline bool has_on_unbounded_side ( const Point_2<T>& p) const {
      return (p.x()<p_[0].x() || p.x()>p_[1].x() || p.y()<p_[0].y() || p.y()>p_[1].y());
    }
  };

};

 
  template < typename T >
  std::istream & operator>>(std::istream &is, internal::Point_2<T> &p)
  {
	T x,y;
	is >> x >> y;
	if (is) p = internal::Point_2<T>(x, y);
	return is;
  }

  template < typename T >
  std::ostream & operator<<(std::ostream &os, const internal::Point_2<T> &p)
  {
    return os << p.x() << " " << p.y();
  }

  template < typename T >
  std::istream & operator>>(std::istream &is, internal::Vector_2<T> &v)
  {
	T x,y;
	is >> x >> y;
	if (is) v = internal::Vector_2<T>(x, y);
	return is;
  }

  template < typename T >
  bool operator==(const internal::Vector_2<T> &u, const internal::Vector_2<T> &v)
  {
	return u.x()==v.x() && u.y()==v.y();
  }

  template < typename T >
  bool operator==(const internal::Point_2<T> &p, const internal::Point_2<T> &q)
  {
	return p.x()==p.x() && q.y()==q.y();
  }

  template < typename T >
  bool operator==(const internal::Iso_rectangle_2<T> &u, const internal::Iso_rectangle_2<T> &v)
  {
	return u.min()==v.min() && u.max()==v.max();
  }

  template < typename T >
  std::ostream & operator<<(std::ostream &os, const internal::Vector_2<T> &v)
  {
      return os << v.x() << " " << v.y();
  }

  template < typename T, typename U >
  inline internal::Vector_2<U> operator*(T t, const internal::Vector_2<U> &v)
  {
    return v*t;
  }

template<typename T> inline internal::Point_2<T> midpoint(
	const internal::Point_2<T>& p,
	const internal::Point_2<T>& q ) {
  return internal::Point_2<T>(0.5*(p.x()+q.x()),0.5*(p.y()+q.y()));
}

template<typename T> inline bool is_zero(T t) { return t==0; }
template<typename T> inline double to_double(T t) { return double(t); }
template<typename T> inline Sign sign(T t) { return (t<0)?NEGATIVE:((t>0)?POSITIVE:ZERO); }
template<typename T> inline T min(T x, T y) { return std::min(x,y); }
template<typename T> inline T max(T x, T y) { return std::max(x,y); }
template<typename T> inline T abs(T x) { return std::abs(x); }

template <typename T>
struct Simple_cartesian {
  typedef T RT;
  typedef T FT;
  typedef internal::Point_2<T> Point_2;
  typedef internal::Vector_2<T> Vector_2;
  typedef internal::Segment_2<T> Segment_2;
  typedef internal::Line_2<T> Line_2;
  typedef internal::Iso_rectangle_2<T> Iso_rectangle_2;
};

template <typename K>
struct Iso_rectangle_2_traits {
  typedef internal::Iso_rectangle_2<typename K::RT> type;
};

#endif //  USE_CGAL

}; // namespace geometry

#endif //  GEOMETRY_GEOMETRY_HPP
