#ifndef GRADIENT_IMAGE_INC_HPP
#define GRADIENT_IMAGE_INC_HPP

#include "image/gradient_image.hpp"
#include "image/image_inc.hpp"

#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>

#ifdef WIN32
const double M_PI = 4.0 * atan(1.0);
#endif // WIN32
template<typename Kernel1D>
void initKernelGaussian1D(Kernel1D& kernel, double sigma)
{
	// Gaussian smoothing
	typedef	typename Kernel1D::value_type vt;
	const vt z = 1.0 / (std::sqrt(2 * M_PI) * sigma);
	const vt sigmasquared = sigma * sigma;
	vt x = -1.0 * kernel.center();
	typename Kernel1D::iterator i;
	vt sum = 0.;
	for (i=kernel.begin(); i!=kernel.end(); ++i, ++x)
	{
		*i = z * (std::exp(-0.5*(x*x/sigmasquared)));
		sum += *i;
	}
	for (i=kernel.begin(); i!=kernel.end(); ++i) *i /= sum;
}

template<typename Kernel1D>
void initKernelGaussianDeriv1D(Kernel1D& kernel, double sigma)
{
	// Gaussian derivative smoothing
	typedef	typename Kernel1D::value_type vt;
	const vt z = 1.0 / (std::sqrt(2 * M_PI) * sigma);
	const vt sigmasquared = sigma * sigma;
	vt x = -1.0 * kernel.center();
	typename Kernel1D::iterator i;
	vt sum = 0.;
	for (i=kernel.begin(); i!=kernel.end(); ++i, ++x)
	{
		*i = - (x/sigmasquared) * z * (std::exp(-0.5*(x*x/sigmasquared)));
		sum += *i * x;
	}
	for (i=kernel.begin(); i!=kernel.end(); ++i) *i /= sum;
}

template<typename Image, typename View>
void gradient_image(Image& g, const View& v, double sigmaD)
{
	using namespace boost::gil;
	typedef gray32F_pixel_t pixel_t;
//	typedef typename Image::pixel_t pixel_t; // todo OT

	g.recreate(v.dimensions());

	unsigned int half_size = (unsigned int) (3* sigmaD) ;
	const size_t kws = 2 * half_size + 1;
	kernel_1d<float> ksmooth(kws, kws / 2);
	kernel_1d<float> kderiv(kws, kws / 2);
	initKernelGaussian1D(ksmooth, sigmaD);
	initKernelGaussianDeriv1D(kderiv, sigmaD);

	convolve_cols<pixel_t> (v, ksmooth, kth_channel_view<0> (view(g)), convolve_option_extend_constant);
	convolve_rows<pixel_t> (kth_channel_view<0> (view(g)), kderiv, kth_channel_view<0> (view(g)), convolve_option_extend_constant);

	convolve_rows<pixel_t> (v, ksmooth, kth_channel_view<1> (view(g)), convolve_option_extend_constant);
	convolve_cols<pixel_t> (kth_channel_view<1> (view(g)), kderiv, kth_channel_view<1> (view(g)), convolve_option_extend_constant);
}

// extra copy due to convolve_rows seeming incompatible with an input any_view // TODO : OT
template<typename Image, typename T>
void gradient_image(Image& g, const boost::gil::any_image_view<T>& v, double sigmaD)
{
	using namespace boost::gil;
	gray32F_image_t img(v.dimensions());
	copy_and_convert_pixels(v,view(img), static_cast_color_converter());
	gradient_image(g,view(img),sigmaD);
}


template<typename GradView, typename Image, typename View, typename IsoRectangle>
void gradient_view(oriented<GradView>& grad, Image& img, const oriented<View>& v_in, const IsoRectangle& bbox, double sigmaD)
{
	oriented<View> v;
	load_view(v, v_in, bbox);
	gradient_image(img, v.view(), sigmaD);
	grad = oriented<GradView>(view(img),v.x0(),v.y0());
}

template<typename View, typename Image, typename IsoRectangle>
void gradient_view(oriented<View>& grad, Image& img, const std::string &file, const IsoRectangle& bbox, double sigmaD)
{
	rjmcmc::any_image_t           any_img;
	oriented<rjmcmc::any_view_t>  any_view;
	load_view(any_view, any_img, file, bbox);
	gradient_image(img, any_view.view(), sigmaD);
	grad = oriented<View>(view(img),any_view.x0(),any_view.y0());
}

#endif // GRADIENT_IMAGE_INC_HPP
