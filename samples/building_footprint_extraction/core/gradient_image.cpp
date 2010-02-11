#include "core/gradient_image.hpp"

#include <boost/gil/extension/io/tiff_dynamic_io.hpp>
#include <boost/gil/extension/matis/float_images.hpp>

template<typename T> void Add1CirclePoints(const T& view, double cx, double cy, double dx, double dy, double d, double & res, double & w)
{
	int i = (int) (cx + dx);
	int j = (int) (cy + dy);
	if(i<0 || j<0 || i>=view.width() || j>=view.height()) return;
	rjmcmc::gradient_image::pixel_t grad = view(i,j);
	res += boost::gil::at_c<0>(grad) * dx + boost::gil::at_c<1>(grad) * dy;
	w   += d;
}

template<typename T> void Add4CirclePoints(const T& view, double cx, double cy, double d, double & res, double & w)
{
	Add1CirclePoints(view, cx, cy, 0, d, d, res, w);
	Add1CirclePoints(view, cx, cy, 0,-d, d, res, w);
	Add1CirclePoints(view, cx, cy, d, 0, d, res, w);
	Add1CirclePoints(view, cx, cy,-d, 0, d, res, w);
}

template<typename T> void Add8CirclePoints(const T& view, double cx, double cy, double dx, double dy, double & res, double & w)
{
	double d = sqrt(dx*dx+dy*dy);
	Add1CirclePoints(view, cx, cy, dx, dy, d, res, w);
	Add1CirclePoints(view, cx, cy,-dx, dy, d, res, w);
	Add1CirclePoints(view, cx, cy,-dx,-dy, d, res, w);
	Add1CirclePoints(view, cx, cy, dx,-dy, d, res, w);
	Add1CirclePoints(view, cx, cy, dy, dx, d, res, w);
	Add1CirclePoints(view, cx, cy,-dy, dx, d, res, w);
	Add1CirclePoints(view, cx, cy,-dy,-dx, d, res, w);
	Add1CirclePoints(view, cx, cy, dy,-dx, d, res, w);
}

double rjmcmc::gradient_image::integrated_flux(const Circle_2 &c) const
{
	double cx = c.center().x() - m_bbox.min().x();
	double cy = c.center().y() - m_bbox.min().y();
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
	if(w==0) return 0.;
	return (res * c.perimeter()) / w;
}

#include "geometry/Segment_2_iterator.h"
double rjmcmc::gradient_image::integrated_flux(const Segment_2& s0) const
{
	using namespace boost::gil;
	Segment_2 s(s0);
	if(!CGAL::clip(m_bbox,s)) return 0;
	CGAL::Segment_2_iterator<K> it(s);

	int x0 = (int)m_bbox.min().x();
	int x1 = (int)m_bbox.max().x();
	int y0 = (int)m_bbox.min().y();
	int y1 = (int)m_bbox.max().y();

	typedef image_t::view_t::xy_locator xy_locator;
	xy_locator loc_grad = m_gradients._view.xy_at(
		(xy_locator::x_coord_t) (it.x()-x0),
		(xy_locator::y_coord_t) (it.y()-y0)
	);

	point2<std::ptrdiff_t> movement[2] = {
		point2<std::ptrdiff_t> (it.step(0), 0),
		point2<std::ptrdiff_t> (0, it.step(1))
	};
	float gradient_sum[2] =	{ 0., 0. };
	for (; !it.end() ; ++it)
	{
		if(it.x()>=x0 && it.x()<x1 && it.y()>=y0 && it.y()<y1) {
			float length = it.length();
			const dev2n32F_pixel_t& grad = *loc_grad;
			gradient_sum[0] += length * at_c<0> (grad);
			gradient_sum[1] += length * at_c<1> (grad);
		}
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
	boost::gil::gray16s_image_t,
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
	m_bbox = Iso_Rectangle_2(0,0,img.width(),img.height());
	my_any_const_view_t v = const_view(img);
	init(m_gradients, v, sigmaD);
}

void rjmcmc::gradient_image::load(const std::string &file, const Iso_Rectangle_2& bbox, double sigmaD, unsigned int step)
{
	my_any_image_t img;
	tiff_read_image(file, img);

	int x0 = std::max((int) bbox.min().x(),0);
	int x1 = std::min((int) bbox.max().x(),(int) img.width());
	int y0 = std::max((int) bbox.min().y(),0);
	int y1 = std::min((int) bbox.max().y(),(int) img.height());
	m_bbox = Iso_Rectangle_2(x0,y0,x1,y1);
	my_any_const_view_t v = subimage_view(const_view(img),x0,y0,x1-x0,y1-y0);
	init(m_gradients, v, sigmaD);
}



