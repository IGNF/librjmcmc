
#include "image/gradient_image.hpp"

#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>

//#include "gil/extension/numeric/sampler.hpp"
//#include "gil/extension/numeric/resample.hpp"

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

void init(gradient_image_t& g, const boost::gil::gray32F_image_t::const_view_t& v, double sigmaD) {
	using namespace boost::gil;
	g.recreate(v.dimensions());

	unsigned int half_size = (unsigned int) (3* sigmaD) ;
	const size_t kws = 2 * half_size + 1;
	kernel_1d<float> ksmooth(kws, kws / 2);
	kernel_1d<float> kderiv(kws, kws / 2);
	initKernelGaussian1D(ksmooth, sigmaD);
	initKernelGaussianDeriv1D(kderiv, sigmaD);

	convolve_cols<gray32F_pixel_t> (v, ksmooth, kth_channel_view<0> (view(g)), convolve_option_extend_constant);
	convolve_rows<gray32F_pixel_t> (kth_channel_view<0> (view(g)), kderiv, kth_channel_view<0> (view(g)), convolve_option_extend_constant);

	convolve_rows<gray32F_pixel_t> (v, ksmooth, kth_channel_view<1> (view(g)), convolve_option_extend_constant);
	convolve_cols<gray32F_pixel_t> (kth_channel_view<1> (view(g)), kderiv, kth_channel_view<1> (view(g)), convolve_option_extend_constant);

	tiff_write_view("v.tif", v);
	tiff_write_view("x.tif", kth_channel_view<0> (view(g)));
	tiff_write_view("y.tif", kth_channel_view<1> (view(g)));
}
