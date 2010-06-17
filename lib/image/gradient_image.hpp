#ifndef GRADIENT_IMAGE_HPP
#define GRADIENT_IMAGE_HPP

#include <boost/gil/image.hpp>
#include <boost/gil/extension/matis/float_images.hpp>
typedef boost::gil::dev2n32F_image_t gradient_image_t;
typedef boost::gil::dev2n32F_view_t  gradient_view_t;
#include "image.hpp"

class gradient_functor
{
public:
  typedef void result_type;
  gradient_functor(double sigma) : m_sigma(sigma) {}
  double sigma() const { return m_sigma; }
  void sigma(double s) { m_sigma = s; }
  
  template<typename Image, typename View>
    result_type operator()(Image& g, const View& v) const;
private:
  double m_sigma;
};

#endif // GRADIENT_IMAGE_HPP
