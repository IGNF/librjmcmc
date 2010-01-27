#include "core/gradient_image.hpp"

#include <boost/gil/extension/io/tiff_dynamic_io.hpp>
#include <boost/gil/extension/matis/float_images.hpp>

template<typename T> inline double Add1CirclePoints(const T& view, double cx, double cy, double dx, double dy)
{
	rjmcmc::gradient_image::pixel_t grad = view((int) (cx + dx), (int) (cy + dy) );
	return boost::gil::at_c<0>(grad) * dx + boost::gil::at_c<1>(grad) * dy;
}

template<typename T> void Add4CirclePoints(const T& view, double cx, double cy, double d, double & res, double & w)
{
	w   += 4*d;

	res += Add1CirclePoints(view, cx, cy, 0, d);
	res += Add1CirclePoints(view, cx, cy, 0,-d);
	res += Add1CirclePoints(view, cx, cy, d, 0);
	res += Add1CirclePoints(view, cx, cy,-d, 0);
}

template<typename T> void Add8CirclePoints(const T& view, double cx, double cy, double dx, double dy, double & res, double & w)
{
	w   += 8*sqrt(dx*dx+dy*dy);

	res += Add1CirclePoints(view, cx, cy, dx, dy);
	res += Add1CirclePoints(view, cx, cy,-dx, dy);
	res += Add1CirclePoints(view, cx, cy,-dx,-dy);
	res += Add1CirclePoints(view, cx, cy, dx,-dy);

	res += Add1CirclePoints(view, cx, cy, dy, dx);
	res += Add1CirclePoints(view, cx, cy,-dy, dx);
	res += Add1CirclePoints(view, cx, cy,-dy,-dx);
	res += Add1CirclePoints(view, cx, cy, dy,-dx);
}

double rjmcmc::gradient_image::integrated_flux(const Circle_2 &c) const
{
	double cx = c.center().x();
	double cy = c.center().y();
	double r  = c.radius();
	double res = 0., w = 0.;
	double dx = 0;
	double dy = r;
	double p = 3 - 2*r;
	Add4CirclePoints(m_gradients._view, cx, cy, dy, res, w);
	while (dx < dy) {
		if (p < 0) {
			p += 4*dx+6;
		} else {
			--dy;
			Add8CirclePoints(m_gradients._view, cx, cy, dx, dy, res, w);
			p += 4*(dx-dy)+10;
		}
		++dx;
		Add8CirclePoints(m_gradients._view, cx, cy, dx, dy, res, w);
	}
	return (res * c.perimeter()) / w;
}

#include "geometry/Segment_2_iterator.h"
double rjmcmc::gradient_image::integrated_flux(const Segment_2& s) const
{
	using namespace boost::gil;
	Segment_2_iterator it(s);

	typedef image_t::view_t::xy_locator xy_locator;
	xy_locator loc_grad = m_gradients._view.xy_at(
		(xy_locator::x_coord_t) s.source().x(),
		(xy_locator::y_coord_t) s.source().y()
	);
	point2<std::ptrdiff_t> movement[2] = {
		point2<std::ptrdiff_t> (it.step(0), 0),
		point2<std::ptrdiff_t> (0, it.step(1))
	};
	float gradient_sum[2] =	{ 0., 0. };
	for (; !it.end() ; ++it)
	{
		float length = it.length();
		const dev2n32F_pixel_t& grad = *loc_grad;
		gradient_sum[0] += length * at_c<0> (grad);
		gradient_sum[1] += length * at_c<1> (grad);
		loc_grad += movement[it.axis()];
	}

	Vector_2 arete(s.target()-s.source());
	Vector_2 normale = arete.perpendicular(CGAL::NEGATIVE);
	Vector_2 sum(gradient_sum[0], gradient_sum[1]);
	return CGAL::to_double(normale * sum);
}


#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>
#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>

typedef boost::mpl::vector<
	boost::gil::gray8_image_t, 
	boost::gil::gray16_image_t,
	boost::gil::gray32_image_t,
	boost::gil::gray32F_image_t,
	boost::gil::gray64F_image_t > my_images_t;
typedef boost::gil::any_image<my_images_t> my_any_image_t;
typedef my_any_image_t::const_view_t my_any_const_view_t;

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


void rjmcmc::gradient_image::load(const std::string &file, double sigmaD, unsigned int step)
{
	my_any_image_t img;
	tiff_read_image(file, img);

	bbox_2::point_type pmin, pmax;
	pmin[0] = pmin[1] = 0;
	pmax[0] = img.width();
	pmax[1] = img.height();
	m_bbox = bbox_2(pmin, pmax);

	my_any_const_view_t v = const_view(img);
	init(m_gradients, v, sigmaD);
}

void rjmcmc::gradient_image::load(const std::string &file, const bbox_2& bbox, double sigmaD, unsigned int step)
{
	my_any_image_t img;
	tiff_read_image(file, img);

	int width  = (int) (bbox.max_point()[0]-bbox.min_point()[0]);
	int height = (int) (bbox.max_point()[1]-bbox.min_point()[1]);
	m_bbox = bbox;

	my_any_const_view_t v = subimage_view(const_view(img),
		(int) bbox.min_point()[0], (int) bbox.min_point()[1], width, height );
	init(m_gradients, v, sigmaD);
}



