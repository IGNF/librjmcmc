// Author(s)     : Mathieu Bredif

#ifndef GEOMETRY_CIRCLE_2_IMAGE_ERROR_H
#define GEOMETRY_CIRCLE_2_IMAGE_ERROR_H

#include "Circle_2.h"


template<typename V, typename F>
inline typename F::result_type apply_operation(const V& v, const F &f) {
  return f(v);
}

template<typename T, typename F>
inline typename F::result_type apply_operation(const boost::gil::any_image_view<T>& v, const F &f) {
  return boost::gil::apply_operation(v, f);
}

struct Circle_2_image_error_functor
{
  double r, x, y;
  template<typename K>
    Circle_2_image_error_functor(const geometry::Circle_2<K> &c, int x0, int y0) {
      r = geometry::radius(c);
      x = c.center().x()-x0;
      y = c.center().y()-y0;
    }
  
  typedef double result_type;
  template <typename T> result_type operator()(const T& v) const {
    
// TODO: optimize with locators
    double res = 0;
    int i0 = std::max(0,(int) (x-r));
    int i1 = std::min((int) (v.width()-1),(int) (x+r+1));
    int j0 = std::max(0,(int) (y-r));
    int j1 = std::min((int) (v.height()-1),(int) (y+r+1));
    
    for ( int i = i0 ; i<=i1 ; ++i)
    {
      for ( int j = j0 ; j<=j1 ; ++j)
      {
        if ((x-i)*(x-i)+(y-j)*(y-j)>r*r) continue;
        double ndvi = v(i,j);
        res +=(140-ndvi) ;
      }
    }
    
    return res;
  }
};

template<typename OrientedView, typename K>
double image_error(const OrientedView& v, const geometry::Circle_2<K> &c) {
  Circle_2_image_error_functor f(c,v.x0(), v.y0());
  return apply_operation(v.view(), f);
}

#endif // GEOMETRY_CIRCLE_2_IMAGE_ERROR_H

