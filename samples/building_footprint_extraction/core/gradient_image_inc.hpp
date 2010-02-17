#ifndef GRADIENT_IMAGE_INC_HPP
#define GRADIENT_IMAGE_INC_HPP

#include "core/gradient_image.hpp"
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>
#include <boost/gil/extension/io/tiff_dynamic_io.hpp> // to write debugging images

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

template<typename I, typename V> void init(I& g, const V& v, double sigmaD=1, unsigned int step=0) {
	using namespace boost::gil;
	g.recreate(v.dimensions());
	gray32F_image_t img(v.dimensions());

	if(step>1) {
            //    resize_view(any_color_converted_view<gray32F_pixel_t>(v, dummy_color_converter()), view(img),  bilinear_sampler());
	} else {
		boost::gil::copy_and_convert_pixels(v,view(img),dummy_color_converter());
	}

	unsigned int half_size = (unsigned int) (3* sigmaD) ;
	const size_t kws = 2 * half_size + 1;
	kernel_1d<float> ksmooth(kws, kws / 2);
	kernel_1d<float> kderiv(kws, kws / 2);
	initKernelGaussian1D(ksmooth, sigmaD);
	initKernelGaussianDeriv1D(kderiv, sigmaD);

	convolve_cols<gray32F_pixel_t> (const_view(img), ksmooth, kth_channel_view<0> (view(g)), convolve_option_extend_constant);
	convolve_rows<gray32F_pixel_t> (kth_channel_view<0> (view(g)), kderiv, kth_channel_view<0> (view(g)), convolve_option_extend_constant);

	convolve_rows<gray32F_pixel_t> (const_view(img), ksmooth, kth_channel_view<1> (view(g)), convolve_option_extend_constant);
	convolve_cols<gray32F_pixel_t> (kth_channel_view<1> (view(g)), kderiv, kth_channel_view<1> (view(g)), convolve_option_extend_constant);

	tiff_write_view("v.tif", v);
	tiff_write_view("i.tif", view(img));
	tiff_write_view("x.tif", kth_channel_view<0> (view(g)));
	tiff_write_view("y.tif", kth_channel_view<1> (view(g)));
}

template<typename View>
void rjmcmc::gradient_image::load(const View& view, const Iso_Rectangle_2& bbox, double sigmaD, unsigned int step)
{
	int x0 = 0;
	int y0 = 0;
	int x1 = (int) view.width();
	int y1 = (int) view.height();
	View v = view;
	if(!(bbox.is_degenerate ())) {
		x0 = std::max(x0,(int) bbox.min().x());
		y0 = std::max(y0,(int) bbox.min().y());
		x1 = std::min(x1,(int) bbox.max().x());
		y1 = std::min(y1,(int) bbox.max().y());
		v = subimage_view(v,x0,y0,x1-x0,y1-y0);
	}
	m_bbox = Iso_Rectangle_2(x0,y0,x1,y1);
	init(m_gradients, v, sigmaD);
}


void rjmcmc::gradient_image::load(const std::string &file, const Iso_Rectangle_2& bbox, double sigmaD, unsigned int step)
{
	boost::gil::gray32F_image_t img;
	tiff_read_image(file.c_str(), img);
	load(const_view(img), bbox, sigmaD, step);
}


#endif // GRADIENT_IMAGE_INC_HPP
