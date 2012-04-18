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

#ifndef GRADIENT_FUNCTOR_INC_HPP
#define GRADIENT_FUNCTOR_INC_HPP

#include "image/gradient_functor.hpp"

#include <boost/bind.hpp>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>

namespace internal {

#if !defined(M_PI)
    const double M_PI = 4.0 * atan(1.0);
#endif

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
