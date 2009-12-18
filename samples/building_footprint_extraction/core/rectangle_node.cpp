#include "rectangle_node.hpp"


bool BoxIsValid::operator()(const Rectangle_2 &n) const
{
	for (unsigned int i = 0; i < 4; ++i)
	{
		bbox_2::point_type ptb;
		ptb[0] = n[i].x();
		ptb[1] = n[i].y();

		if (!m_box.is_inside(ptb)) return false;
	}
	if (n.squared_length(0) < m_squared_min_size || n.squared_length(1) < m_squared_min_size)
		return false;

	float ratio = std::abs(n.ratio());
	return (ratio <= m_max_ratio && 1 <= m_max_ratio*ratio );
}

bool BoxIsValid::operator()(const Circle_2 &n) const
{
	bbox_2::point_type pmin = m_box.min_point();
	bbox_2::point_type pmax = m_box.max_point();
	if(n.center().x()<pmin[0]+n.radius() || n.center().x()>pmax[0]-n.radius()) return false;
	if(n.center().y()<pmin[1]+n.radius() || n.center().y()>pmax[1]-n.radius()) return false;
	if (n.radius()*n.radius() < m_squared_min_size)	return false;
	return true;
}



double image_gradient_unary_energy::operator()(const Circle_2 &c) const
{
	return operator()(CGAL::Rectangle_2<K>(c.center(),Vector_2(c.radius(),0),1)); // todo : circle !!!
}


double image_gradient_unary_energy::operator()(const Rectangle_2 &n) const
{
	double res = m_defaultEnergy;
	for (unsigned int i = 0; i < 4; ++i)
	{
		double delta = m_image->ComputeSegmentDataEnergy(n.point(i), n.point(i + 1));
		if (delta >= 0.)
			res -= delta;
	}
	return res;
}

#include "geometry/Segment_2_iterator.h"

using namespace boost::gil;

image_gradient_unary_energy::image_gradient_unary_energy(double default_energy, const std::string& file, const bbox_2& bbox, double sigmaD, unsigned int step) :
	m_defaultEnergy(default_energy), 
	m_image(boost::shared_ptr<gil_image>(new gil_image))
{
	m_image->load(file,bbox,sigmaD,step);
}

image_gradient_unary_energy::image_gradient_unary_energy(double default_energy, const std::string& file, double sigmaD, unsigned int step) :
	m_defaultEnergy(default_energy), 
	m_image(boost::shared_ptr<gil_image>(new gil_image))
{
	m_image->load(file,sigmaD,step);
}

image_gradient_unary_energy::~image_gradient_unary_energy()
{
}

double gil_image::ComputeSegmentDataEnergy(const Point_2 &gridIn, const Point_2 &gridOut) const
{
	Segment_2 s(gridIn, gridOut);
	Segment_2_iterator it(s);

	typedef image_t::view_t::xy_locator xy_locator;
	xy_locator loc_grad = m_gradients._view.xy_at(
		(xy_locator::x_coord_t) gridIn.x(),
		(xy_locator::y_coord_t) gridIn.y()
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

	Vector_2 arete(gridOut-gridIn);
	Vector_2 normale = arete.perpendicular(CGAL::NEGATIVE);
	Vector_2 sum(gradient_sum[0], gradient_sum[1]);
	return CGAL::to_double(normale * sum);
}


#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>
typedef boost::mpl::vector<gray8_image_t , gray16_image_t , gray32_image_t, gray32F_image_t , gray64F_image_t > my_images_t;
typedef any_image<my_images_t> my_any_image_t;
typedef my_any_image_t::const_view_t my_any_const_view_t;

void gil_image::load(const std::string &file, double sigmaD, unsigned int step)
{
	my_any_image_t img;
	tiff_read_image(file, img);

	bbox_2::point_type pmin, pmax;
	pmin[0] = pmin[1] = 0;
	pmax[0] = img.width();
	pmax[1] = img.height();
	m_bbox = bbox_2(pmin, pmax);

	my_any_const_view_t v = const_view(img);
	init(v, sigmaD);
}

void gil_image::load(const std::string &file, const bbox_2& bbox, double sigmaD, unsigned int step)
{
	my_any_image_t img;
	tiff_read_image(file, img);

	int width  = (int) (bbox.max_point()[0]-bbox.min_point()[0]);
	int height = (int) (bbox.max_point()[1]-bbox.min_point()[1]);
	m_bbox = bbox;

	my_any_const_view_t v = subimage_view(const_view(img),
		(int) bbox.min_point()[0], (int) bbox.min_point()[1], width, height );
	init(v, sigmaD);
}

//#include "gil/extension/numeric/sampler.hpp"
//#include "gil/extension/numeric/resample.hpp"

template<typename View> void gil_image::init(const View& v, double sigmaD, unsigned int step) {
	m_gradients.recreate(v.dimensions());
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

	convolve_cols<gray32F_pixel_t> (const_view(img), ksmooth, kth_channel_view<0> (m_gradients._view), convolve_option_extend_constant);
	convolve_rows<gray32F_pixel_t> (kth_channel_view<0> (m_gradients._view), kderiv, kth_channel_view<0> (m_gradients._view), convolve_option_extend_constant);

	convolve_rows<gray32F_pixel_t> (const_view(img), ksmooth, kth_channel_view<1> (m_gradients._view), convolve_option_extend_constant);
	convolve_cols<gray32F_pixel_t> (kth_channel_view<1> (m_gradients._view), kderiv, kth_channel_view<1> (m_gradients._view), convolve_option_extend_constant);

	tiff_write_view("v.tif", v);
	tiff_write_view("i.tif", view(img));
	tiff_write_view("x.tif", kth_channel_view<0> (m_gradients._view));
	tiff_write_view("y.tif", kth_channel_view<1> (m_gradients._view));
}

