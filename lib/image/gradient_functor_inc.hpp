#ifndef GRADIENT_FUNCTOR_INC_HPP
#define GRADIENT_FUNCTOR_INC_HPP

#include "image/gradient_functor.hpp"

#include <boost/bind.hpp>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>

namespace internal {

template<typename Kernel1D>
  void initKernelGaussian1D(Kernel1D& kernel, double m_sigma)
{
	// Gaussian smoothing
  typedef	typename Kernel1D::value_type vt;
  const vt z = 1.0 / (std::sqrt(2 * M_PI) * m_sigma);
  const vt m_sigmasquared = m_sigma * m_sigma;
  vt x = -1.0 * kernel.center();
  typename Kernel1D::iterator i;
  vt sum = 0.;
  for (i=kernel.begin(); i!=kernel.end(); ++i, ++x)
  {
    *i = z * (std::exp(-0.5*(x*x/m_sigmasquared)));
    sum += *i;
  }
  for (i=kernel.begin(); i!=kernel.end(); ++i) *i /= sum;
}

template<typename Kernel1D>
  void initKernelGaussianDeriv1D(Kernel1D& kernel, double m_sigma)
{
	// Gaussian derivative smoothing
  typedef	typename Kernel1D::value_type vt;
  const vt z = 1.0 / (std::sqrt(2 * M_PI) * m_sigma);
  const vt m_sigmasquared = m_sigma * m_sigma;
  vt x = -1.0 * kernel.center();
  typename Kernel1D::iterator i;
  vt sum = 0.;
  for (i=kernel.begin(); i!=kernel.end(); ++i, ++x)
  {
    *i = - (x/m_sigmasquared) * z * (std::exp(-0.5*(x*x/m_sigmasquared)));
    sum += *i * x;
  }
  for (i=kernel.begin(); i!=kernel.end(); ++i) *i /= sum;
}

}

template<typename Image, typename View>
typename gradient_functor::result_type gradient_functor::operator()(Image& g, const View& v) const
{
  using namespace boost::gil;
  
  typedef typename Image::view_t g_view_t;
  typedef typename get_pixel_type<g_view_t>::type g_pixel_t;
  typedef typename kth_element_type<g_pixel_t,0>::type element_0_t;
  typedef typename kth_element_type<g_pixel_t,1>::type element_1_t;
  typedef pixel<element_0_t, gray_layout_t > g0_pixel_t;
  typedef pixel<element_1_t, gray_layout_t > g1_pixel_t;
  
  g.recreate(v.dimensions());
  
  unsigned int half_size = (unsigned int) (3* m_sigma) ;
  const size_t kws = 2 * half_size + 1;
  kernel_1d<float> ksmooth(kws, kws / 2);
  kernel_1d<float> kderiv(kws, kws / 2);
  internal::initKernelGaussian1D(ksmooth, m_sigma);
  internal::initKernelGaussianDeriv1D(kderiv, m_sigma);
  
  convolve_cols<g0_pixel_t> (v, ksmooth, kth_channel_view<0> (view(g)), convolve_option_extend_constant);
  convolve_rows<g0_pixel_t> (kth_channel_view<0> (view(g)), kderiv, kth_channel_view<0> (view(g)), convolve_option_extend_constant);
  
  convolve_rows<g1_pixel_t> (v, ksmooth, kth_channel_view<1> (view(g)), convolve_option_extend_constant);
  convolve_cols<g1_pixel_t> (kth_channel_view<1> (view(g)), kderiv, kth_channel_view<1> (view(g)), convolve_option_extend_constant);
}

#endif // GRADIENT_FUNCTOR_INC_HPP
