#ifndef RECTANGLE_2_UNIFORM_DENSITY_HPP_
#define RECTANGLE_2_UNIFORM_DENSITY_HPP_

#include "Rectangle_2.h"

template<typename K>
class Rectangle_2_uniform_density
{
    typedef typename K::Point_2  Point_2;
    typedef typename K::Vector_2 Vector_2;
    typedef typename K::FT FT;

public:
  typedef geometry::Rectangle_2<K> value_type ;
  typedef Rectangle_2_uniform_density<K> self;

// valid <-> pdf>0
  bool valid(const value_type &r) const
  {
      FT sl0 = r.squared_length(0);
      FT sl1 = r.squared_length(1);
    return m_x0 <= r.center().x()
      &&   m_x1 >= r.center().x()
      &&   m_y0 <= r.center().y()
      &&   m_y1 >= r.center().y()
      &&   m_r0 <= r.ratio()
      &&   m_r1 >= r.ratio()
      &&   m_sl0<= sl0
      &&   m_sl0<= sl1
      &&   m_sl1>= sl0
      &&   m_sl1>= sl1;
  }

  inline double pdf(const value_type &r) const {
    return valid(r)? m_pdf : 0 ;
  }

  double sample(value_type &t) const
  {
    Point_2  p(m_x0+m_dx*m_die(), m_y0+m_dy*m_die());
    FT       r(m_r0+m_dr*m_die());
    Vector_2 v;
    FT v2;
    // rejection sampling of a vector v in the annulus of squared radii m_sl0 and m_sl1
    do {
      v  = Vector_2(m_dl*(m_die()-0.5), m_dl*(m_die()-0.5));
      v2 = v.squared_length();
    } while (v2<m_sl0 || m_sl1<v2);
    t = value_type(p, v, r);
    return m_pdf;
  }

  template<typename IsoRectangle>
    self(const IsoRectangle &bbox, double l0, double l1, double r0, double r1)
  {
    m_x0 = bbox.min().x();
    m_x1 = bbox.max().x();
    m_y0 = bbox.min().y();
    m_y1 = bbox.max().y();
    m_r0 = r0;
    m_r1 = r1;
    if(m_x0>m_x1) std::swap(m_x0,m_x1);
    if(m_y0>m_y1) std::swap(m_y0,m_y1);
    if(m_r0>m_r1) std::swap(m_r0,m_r1);
    if(m_l0>m_l1) std::swap(m_l0,m_l1);
    m_dx = m_x1-m_x0;
    m_dy = m_y1-m_y0;
    m_dr = m_r1-m_r0;
    m_sl0 = l0*l0;
    m_sl1 = l1*l1;
    m_dl  = 2.*std::abs(l1);
    m_pdf = 1.0/(m_dx*m_dy*M_PI*(m_sl1-m_sl0)*m_dr);
  }

private:
  double m_x0, m_x1, m_dx;
  double m_y0, m_y1, m_dy;
  double m_r0, m_r1, m_dr;
  double m_sl0,m_sl1;
  double m_dl;
  double m_pdf;
};

#endif /*RECTANGLE_2_UNIFORM_DENSITY_HPP_*/
