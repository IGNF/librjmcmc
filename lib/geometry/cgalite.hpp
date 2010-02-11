#ifndef __CGAL_HPP__
#define __CGAL_HPP__


#if USE_CGAL

#include "CGAL/Simple_cartesian.h"

#else

#include <cmath>
#include <iostream>
#include <algorithm> // min & max

#ifndef M_PI
const double M_PI = 4.0 * atan(1.0);
#endif // #ifndef M_PI

#define CGAL_BEGIN_NAMESPACE namespace CGAL {
#define CGAL_END_NAMESPACE }


CGAL_BEGIN_NAMESPACE

enum Orientation { CLOCKWISE=-1, DEGENERATE=0, COUNTERCLOCKWISE=1 };
enum Oriented_side { ON_NEGATIVE_SIDE=-1, ON_ORIENTED_BOUNDARY=0, ON_POSITIVE_SIDE=1 };
enum Bounded_side { ON_BOUNDED_SIDE=-1, ON_BOUNDARY=0, ON_UNBOUNDED_SIDE=1 } ;
enum Sign { NEGATIVE=-1, ZERO=0, POSITIVE=1 } ;

struct Origin {};
extern Origin ORIGIN;
class IO {
public:
    enum Mode {ASCII = 0, PRETTY, BINARY};
    static int mode;
};

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

  template <typename T> class Iso_Rectangle_2 {
    Point_2<T> p_[2];
  public:
    Iso_Rectangle_2() {}
    Iso_Rectangle_2(const Point_2<T>& p, const Point_2<T>& q) {
      p_[0]=p; p_[1]=q;
    }
    Iso_Rectangle_2(T x0, T y0, T x1, T y1) {
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

  template<typename T> bool clip(
    const internal::Iso_Rectangle_2<T>& r, internal::Segment_2<T>& s)
  {
    const internal::Point_2<T>& p(s.source());
    const internal::Point_2<T>& q(s.target());
    internal::Vector_2<T> d(q-p);
    T a[] = { p.x(), p.y() };
    T b[] = { q.x(), q.y() };
    T tmin = 0, tmax = 1;
    for(int i=0; i<2; ++i) {
      if(d[i]==0 && (p[i]<r.min()[i] || p[i]>r.max()[i])) {
        return false;
      } else {
        int j = 1-i, k = (d[i] > 0);
        int l = 1-k;
	T m[] = {r.min()[i], r.max()[i]}, t;
        t = (m[l]-p[i])/d[i];
        if(t>tmin) { tmin=t; a[i]=m[l];  a[j]=p[j]+t*d[j]; } // a=p+td
        t = (m[k]-p[i])/d[i];
        if(t<tmax) { tmax=t; b[i]=m[k];  b[j]=p[j]+t*d[j]; } // b=p+td
        if(tmax <= tmin) return false;
      }
    }
    internal::Point_2<T> pa(a[0],a[1]);
    internal::Point_2<T> pb(b[0],b[1]);
    s = internal::Segment_2<T>(pa,pb);
    return true;
  }
 
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
    switch(os.iword(IO::mode)) {
    case IO::ASCII :
      return os << p.x() << ' ' << p.y();
    case IO::BINARY :
      return os << p.x() << p.y();
    default:
      return os << "Point_2(" << p.x() << ", " << p.y() << ")";
    }
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
  std::ostream & operator<<(std::ostream &os, const internal::Vector_2<T> &v)
  {
    switch(os.iword(IO::mode)) {
    case IO::ASCII :
      return os << v.x() << ' ' << v.y();
    case IO::BINARY :
      return os << v.x() << v.y();
    default:
      return os << "Vector_2(" << v.x() << ", " << v.y() << ")";
    }
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
  typedef internal::Iso_Rectangle_2<T> Iso_Rectangle_2;
};

CGAL_END_NAMESPACE

#endif //  USE_CGAL

#endif //  __CGAL_HPP__
