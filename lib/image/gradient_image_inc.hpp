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

template<typename ImageType>
struct gradient_functor
{
    typedef void result_type;
    ImageType& g;
    double sigma;

    gradient_functor(double sigma_, ImageType &result_) : sigma(sigma_), g(result_) {}

    template<typename ViewType>
    result_type operator()(const ViewType& v)
    {
        using namespace boost::gil;

        typedef typename ImageType::view_t g_view_t;
        typedef typename get_pixel_type<g_view_t>::type g_pixel_t;
        typedef typename kth_element_type<g_pixel_t,0>::type element_0_t;
        typedef typename kth_element_type<g_pixel_t,1>::type element_1_t;
        typedef pixel<element_0_t, gray_layout_t > g0_pixel_t;
        typedef pixel<element_1_t, gray_layout_t > g1_pixel_t;

        g.recreate(v.dimensions());

        unsigned int half_size = (unsigned int) (3* sigma) ;
        const size_t kws = 2 * half_size + 1;
        kernel_1d<float> ksmooth(kws, kws / 2);
        kernel_1d<float> kderiv(kws, kws / 2);
        initKernelGaussian1D(ksmooth, sigma);
        initKernelGaussianDeriv1D(kderiv, sigma);

        std::cout << ksmooth.front() << std::endl;
        std::cout << kderiv.front() << std::endl;
        std::cout << ksmooth.back() << std::endl;
        std::cout << kderiv.back() << std::endl;
        std::cout << g.dimensions().x << std::endl;
        std::cout << g.dimensions().y << std::endl;

        // todo: remove debug cout
        convolve_cols<g0_pixel_t> (v, ksmooth, kth_channel_view<0> (view(g)), convolve_option_extend_constant);
        convolve_rows<g0_pixel_t> (kth_channel_view<0> (view(g)), kderiv, kth_channel_view<0> (view(g)), convolve_option_extend_constant);

        convolve_rows<g1_pixel_t> (v, ksmooth, kth_channel_view<1> (view(g)), convolve_option_extend_constant);
        convolve_cols<g1_pixel_t> (kth_channel_view<1> (view(g)), kderiv, kth_channel_view<1> (view(g)), convolve_option_extend_constant);
    }
};

template<typename GradView, typename Image, typename View, typename IsoRectangle>
void gradient_view(oriented<GradView>& grad, Image& img, const oriented<View>& v_in, const IsoRectangle& bbox, double sigmaD)
{
	oriented<View> v;
	load_view(v, v_in, bbox);
        grad = oriented<GradView>(view(img),v.x0(),v.y0());
}

template<typename View, typename Image, typename IsoRectangle>
void gradient_view(oriented<View>& grad, Image& img, const std::string &file, const IsoRectangle& bbox, double sigmaD)
{
        rjmcmc::any_image_t           any_img;
	oriented<rjmcmc::any_view_t>  any_view;
        load_view(any_view, any_img, file, bbox);
        gradient_functor<Image> gradient_f(sigmaD, img);
        boost::gil::apply_operation( any_view.view(), gradient_f );
        grad = oriented<View>(view(img),any_view.x0(),any_view.y0());
}

#endif // GRADIENT_IMAGE_INC_HPP
