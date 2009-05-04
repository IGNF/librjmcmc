#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/matis/float_images.hpp>
#include <boost/gil/extension/io/tiff_dynamic_io.hpp>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>

#include <boost/gil/extension/matis/deriche.hpp>
#include <boost/gil/extension/matis/float_images.hpp>

using namespace boost::gil;

typedef pixel<float,devicen_layout_t<2> >		dev2n32F_pixel_t;
typedef dev2n32F_pixel_t*						dev2n32F_pixel_ptr_t;
typedef image_type<float,devicen_layout_t<2> >::type	dev2n32F_image_t;
typedef dev2n32F_image_t::view_t				dev2n32F_view_t;

#include "GILEnergyPolicy.hpp"

struct GILEnergyPolicy::gradients_image_t: public dev2n32F_image_t
{
};

template<typename Kernel1D>
void initKernelGaussian1D(Kernel1D& kernel, double sigma)
{
	// Gaussian smoothing
	typedef typename Kernel1D::value_type vt;
	const vt z = 1.0 / (std::sqrt(2 * M_PI) * sigma);
	const vt sigmasquared = sigma * sigma;
	vt x = -1.0 * kernel.center();
	vt sum =0.;
	for(typename Kernel1D::iterator i = kernel.begin();i!=kernel.end(); ++i, ++x)
	{
		*i = z * (std::exp(-0.5 * (x * x / sigmasquared)));
		sum += *i;
	}
	for (typename Kernel1D::iterator i = kernel.begin(); i!=kernel.end(); ++i)
		*i /= sum;
}

template<typename Kernel1D>
void initKernelGaussianDeriv1D(Kernel1D& kernel, double sigma)
{
	// Gaussian derivative smoothing
	typedef typename Kernel1D::value_type vt;
	const vt z = 1.0 / (std::sqrt(2 * M_PI) * sigma);
	const vt sigmasquared = sigma * sigma;
	vt x = -1.0 * kernel.center();
	typename Kernel1D::iterator i;
	vt sum = 0.;
	for (i = kernel.begin(); i!=kernel.end(); ++i, ++x)
	{
		*i = - (x / sigmasquared) * z * (std::exp(-0.5 * (x * x / sigmasquared)));
		sum += *i * x;
	}
	for (i=kernel.begin(); i!=kernel.end(); ++i)
		*i /= sum;
}

GILEnergyPolicy::GILEnergyPolicy() : m_defaultEnergy(0.), m_coefDefaultEnergy(0.), m_gradients_cercle(new gradients_image_t), m_gradients_rectangle(new gradients_image_t)
{}

void GILEnergyPolicy::Init(double defaultEnergy, double coefDefaultEnergy, const std::string &nomIm, const std::string &nomMask)
{
	m_defaultEnergy = defaultEnergy;
	m_coefDefaultEnergy = coefDefaultEnergy;
	double sigmaD = 1.;
	unsigned int half_size = 3*sigmaD;
	const size_t kws = 2 * half_size+1;
	kernel_1d<float> ksmooth(kws,kws/2);
	kernel_1d<float> kderiv(kws,kws/2);
	initKernelGaussian1D(ksmooth,sigmaD);
	initKernelGaussianDeriv1D(kderiv,sigmaD);

    gray16_image_t img;
    tiff_read_image(nomIm ,img);
	m_gradients_rectangle->recreate(img.dimensions());

	convolve_cols<gray32F_pixel_t>(const_view(img),ksmooth,kth_channel_view<0>(m_gradients_rectangle->_view), convolve_option_extend_constant);
	convolve_rows<gray32F_pixel_t>(kth_channel_view<0>(m_gradients_rectangle->_view),kderiv,kth_channel_view<0>(m_gradients_rectangle->_view), convolve_option_extend_constant);

	convolve_rows<gray32F_pixel_t>(const_view(img),ksmooth,kth_channel_view<1>(m_gradients_rectangle->_view), convolve_option_extend_constant);
    convolve_cols<gray32F_pixel_t>(kth_channel_view<1>(m_gradients_rectangle->_view),kderiv,kth_channel_view<1>(m_gradients_rectangle->_view), convolve_option_extend_constant);
/*
    gray32F_image_t module(img.dimensions());
    gray8_image_t filtered_module(img.dimensions());
    ModuleGradient( kth_channel_view<0>(m_gradients->_view), kth_channel_view<1>(m_gradients->_view), view(module));
    NonMaximaLocauxGradient (
							  kth_channel_view<0>(m_gradients->_view) ,
							  kth_channel_view<1>(m_gradients->_view) ,
							  view(module) ,
							  view(filtered_module) ,
							  5.);

	gray8_view_t::iterator it_filtered = view(filtered_module).begin();
	dev2n32F_view_t::iterator it_gradients = m_gradients->_view.begin();

	for( ; it_filtered!=view(filtered_module).begin() ; ++it_filtered , ++it_gradients )
	{
		if ( *it_filtered==0 )
		{
			dev2n32F_pixel_t &grad = m_gradients->_view(it_gradients.x_pos(), it_gradients.y_pos());
			at_c<0>(grad) = 0.;
			at_c<1>(grad) = 0.;
		}
	}
	*/ 
	if (nomMask == "")
		m_gradients_cercle = m_gradients_rectangle;
	else
	{
		gray8_image_t mask;
		tiff_read_image(nomMask, mask);
		m_gradients_cercle->recreate(m_gradients_rectangle->dimensions());
		gray8_view_t::iterator it_mask = view(mask).begin();
		dev2n32F_view_t::iterator it_rectangle = m_gradients_rectangle->_view.begin();
		dev2n32F_view_t::iterator it_cercle = m_gradients_cercle->_view.begin();
		dev2n32F_view_t::iterator fin_cercle = m_gradients_cercle->_view.end();
		for (; it_cercle != fin_cercle; ++it_mask, ++it_rectangle, ++it_cercle)
		{
			dev2n32F_pixel_t grad = *it_rectangle;
			dev2n32F_pixel_t zero (0);
			if (*it_mask == 255)
			{
				*it_cercle = grad;
				*it_rectangle = zero;
			}
			else
			{
				*it_cercle = zero;
				*it_rectangle = grad;
			}
		}
	}

//	tiff_write_view("gradients_rec_x.tif", kth_channel_view<0>(m_gradients_rectangle->_view));
//	tiff_write_view("gradients_rec_y.tif", kth_channel_view<1>(m_gradients_rectangle->_view));
//	tiff_write_view("gradients_cer_x.tif", kth_channel_view<0>(m_gradients_cercle->_view));
//	tiff_write_view("gradients_cer_y.tif", kth_channel_view<1>(m_gradients_cercle->_view));
}

double GILEnergyPolicy::ComputeDataEnergy(const Rectangle_2 &n) const
{
	double res = m_defaultEnergy + m_coefDefaultEnergy * /*::sqrt*/(n.perimeter());
	for (unsigned int i = 0; i < 4; ++i)
	{
		double delta = -ComputeSegmentDataEnergy(n.point(i),n.point(i + 1));
//		res += delta;
		if ( delta <= 0. )
			res += delta;
	}
	return res;
}

//TODO: improve --> 40%
double GILEnergyPolicy::ComputeSegmentDataEnergy(const Point_2 &gridIn, const Point_2 &gridOut) const
{
	float gradient_sum[2] = {0.,0.};

	Segment_2 s(gridIn, gridOut);
	CGAL::Segment_2_iterator<Segment_2> it(s);
	dev2n32F_view_t::xy_locator loc_grad = m_gradients_rectangle->_view.xy_at(gridIn.x(), gridIn.y());
	point2<std::ptrdiff_t> mvt[2] = { point2<std::ptrdiff_t>(it.Step(0),0), point2<std::ptrdiff_t>(0,it.Step(1)) };
	for (; !it.end(); ++it)
	{
		const float length = it.length();
		dev2n32F_pixel_t grad = (*loc_grad);
		gradient_sum[0] += length * at_c<0>(grad);
		gradient_sum[1] += length * at_c<1>(grad);
		loc_grad += mvt[it.axis()];
//		std::cout << "new : " << at_c<0>(grad) << "\t" << at_c<1>(grad) << std::endl;
//		const dev2n32F_pixel_t old_grad = m_gradients->_view(it.x(), it.y());
//		std::cout << "old : " << at_c<0>(old_grad) << "\t" << at_c<1>(old_grad) << std::endl;
	}

	Vector_2 arete(gridIn, gridOut);
	// TODO : NEGATIVE or POSITIVE ???
	Vector_2 normale = arete.perpendicular(CGAL::NEGATIVE);
	Vector_2 sum(gradient_sum[0], gradient_sum[1]);
	return CGAL::to_double(normale * sum);
}

double GILEnergyPolicy::Add1CirclePoints(double xCenter, double yCenter, double dx, double dy) const
{
	dev2n32F_pixel_t grad = m_gradients_cercle->_view(xCenter + dx, yCenter + dy);
	Vector_2 vgrad = Vector_2(at_c<0>(grad), at_c<1>(grad));
	Vector_2 vnorm = Vector_2( dx, dy);
	return vgrad * vnorm;
}

const double sqrt_2 = ::sqrt(2.);

void GILEnergyPolicy::Add8CirclePoints(double xCenter, double yCenter, double dx, double dy, double radius, double & res) const
{
	const double coef = 1. - sqrt_2 * std::abs(::sqrt(dx*dx+dy*dy) - radius);
//	std::cout << radius << "\t" << ::sqrt(dx*dx+dy*dy) << "\t" << coef << std::endl;
//	getchar();
	res -= coef * Add1CirclePoints(xCenter, yCenter, dx, dy);
	res -= coef * Add1CirclePoints(xCenter, yCenter,-dx, dy);
	res -= coef * Add1CirclePoints(xCenter, yCenter,-dx,-dy);
	res -= coef * Add1CirclePoints(xCenter, yCenter, dx,-dy);

	res -= coef * Add1CirclePoints(xCenter, yCenter, dy, dx);
	res -= coef * Add1CirclePoints(xCenter, yCenter,-dy, dx);
	res -= coef * Add1CirclePoints(xCenter, yCenter,-dy,-dx);
	res -= coef * Add1CirclePoints(xCenter, yCenter, dy,-dx);
}

void GILEnergyPolicy::Add4CirclePoints(double xCenter, double yCenter, double dy, double radius, double & res) const
{
//	const double coef = 1. - sqrt_2 * std::abs(::sqrt(dx*dx+dy*dy) - radius);
	const double coef = 1.;
//	std::cout << radius << "\t" << ::sqrt(dx*dx+dy*dy) << "\t" << coef << std::endl;
//	getchar();
	res -= coef * Add1CirclePoints(xCenter, yCenter, 0, dy);
	res -= coef * Add1CirclePoints(xCenter, yCenter, 0,-dy);
	res -= coef * Add1CirclePoints(xCenter, yCenter, dy, 0);
	res -= coef * Add1CirclePoints(xCenter, yCenter,-dy, 0);
}

// Pour ca, tout vient de 
// http://escience.anu.edu.au/lecture/cg/Circle/
// Corrige par :
// http://www.cplusplus.happycodings.com/Computer_Graphics/code16.html
double GILEnergyPolicy::ComputeDataEnergy(const Cercle_2 &n) const
{
	double res = 0.;
	double dx = 0;
	double dy = n.radius();
	double p = 3 - 2*n.radius();
	double xCenter = n.center().x(), yCenter = n.center().y();
	Add4CirclePoints(xCenter, yCenter, dy, n.radius(), res);
    while (dx < dy)
	{
        if (p < 0) 
		{
            p += 4*dx+6;
        }
		else 
		{
            dy--;
            p += 4*(dx-dy)+10;
        }
        dx++;
		Add8CirclePoints(xCenter, yCenter, dx, dy, n.radius(), res);
    } 
    
	return (res / n.radius()) + m_defaultEnergy + m_coefDefaultEnergy * /*::sqrt*/(n.perimeter());
}

struct ImageExporter::export_image_t : public gray8_image_t
{
};

ImageExporter::ImageExporter(bool fill) : m_fill(fill), m_img(new export_image_t)
{}

struct local_cc
{
	template<class SrcType, class DstType>
	void operator()(const SrcType &src, DstType &dst) const
	{
		dst = src;
	}

};
	
template<>
void local_cc::operator()<gray16_pixel_t, gray8_pixel_t>(const gray16_pixel_t &src, gray8_pixel_t &dst) const
{
	dst = src - 900;
}

void ImageExporter::InitExport(const char *filename) const
{
	m_img->recreate(tiff_read_dimensions(filename));
	tiff_read_and_convert_view(filename, m_img->_view, local_cc());
}

void ImageExporter::InitExport(int width, int height) const
{
	m_img->recreate(width, height);
	fill_pixels(m_img->_view, 0);
}

void ImageExporter::EndExport(const char *filename) const
{
	tiff_write_view(filename, m_img->_view);
}

void ImageExporter::Export8Points(int xCenter, int yCenter, int dx, int dy) const
{
	m_img->_view(xCenter + dx, yCenter + dy) = 255;
	m_img->_view(xCenter - dx, yCenter + dy) = 255;
	m_img->_view(xCenter - dx, yCenter - dy) = 255;
	m_img->_view(xCenter + dx, yCenter - dy) = 255;
	m_img->_view(xCenter + dy, yCenter + dx) = 255;
	m_img->_view(xCenter - dy, yCenter + dx) = 255;
	m_img->_view(xCenter - dy, yCenter - dx) = 255;
	m_img->_view(xCenter + dy, yCenter - dx) = 255;

	if (m_fill)
	{
		for (int x = (xCenter - dx); x < (xCenter + dx); ++x)
		{
			m_img->_view(x, yCenter + dy) = 255;
			m_img->_view(x, yCenter - dy) = 255;
		}
		for (int x = (xCenter - dy); x < (xCenter + dy); ++x)
		{
			m_img->_view(x, yCenter + dx) = 255;
			m_img->_view(x, yCenter - dx) = 255;
		}
	}
}

void ImageExporter::ExportNode(const Cercle_2 &n) const
{
	int x = 0;
	int y = n.radius();
	int p = 3 - 2*n.radius();
	int xCenter = n.center().x(), yCenter = n.center().y();
    Export8Points(xCenter, yCenter, x, y);
    while (x < y) 
	{
        if (p < 0) 
		{
            p += 4*x+6;
        }
		else 
		{
            y--;
            p += 4*(x-y)+10;
        }
        x++;
        Export8Points(xCenter, yCenter, x, y);
    } 
}

void ImageExporter::ExportNode(const Rectangle_2 &n) const
{
	for (unsigned int i = 0; i < 4; ++i)
	{
		CGAL::Segment_2_iterator<Segment_2> it(Segment_2(n.point(i),n.point(i + 1)));
		for (; !it.end(); ++it)
		{
			m_img->_view(it.x(), it.y()) = 255;
		}
	}
	if (m_fill)
	{
		CGAL::Rectangle_2_point_iterator<Rectangle_2> it(n);
		for (; !it.end(); ++it)
		{
			m_img->_view(it.x(), it.y()) = 255;
		}
	}
}


void ExportCercleTestImage(const char *nomOut, int rayon)
{
	gray16_image_t img;
	img.recreate(251,251);
	fill_pixels(img._view, 0);
	
	unsigned int centrex = 120, centrey=120;
	float r2 = rayon * rayon;
	
	for (unsigned int l=0; l<img.dimensions().y; ++l)
		for (unsigned int c=0; c<img.dimensions().x; ++c)
		{
			float dc = c;
			dc -= centrex;
			float dl = l;
			dl -= centrey;
			float d2 = dc*dc+dl*dl;
			if (d2 < r2)
				img._view(c,l) = 100;
		}

	tiff_write_view(nomOut, img._view);
}

void ExportRectangleTestImage(const char *nomOut, int demilargeur, int demihauteur)
{
	gray16_image_t img;
	img.recreate(251,251);
	fill_pixels(img._view, 0);
	
	unsigned int centrex = 120, centrey=120;
	
	for (unsigned int l=0; l<img.dimensions().y; ++l)
		for (unsigned int c=0; c<img.dimensions().x; ++c)
		{
			float dc = c;
			dc -= centrex;
			dc = abs(dc);
			float dl = l;
			dl -= centrey;
			dl = abs(dl);
			if ((dc < demilargeur) && (dl < demihauteur))
				img._view(c,l) = 100;
		}

	tiff_write_view(nomOut, img._view);
}

void generate_test_images ()
{
/*
 * 	gray16_image_t in;
	gray16_image_t out;
	tiff_read_image ("data/MNS-veget/ZCarto-SaintMichel-50.crop.reechground.tif", in);
	out.recreate(in.dimensions());

	gray16_view_t::iterator it_in = view(in).begin(), fin_in = view(in).end();
	gray16_view_t::iterator it_out = view(out).begin();
	for (; it_in != fin_in; ++it_in, ++it_out)
		*it_out = *it_in + 1000;
	
	tiff_write_view("out.tif", out._view);
	return;
*/
	ExportCercleTestImage("test_1cercle_r025.tif", 25);
	ExportCercleTestImage("test_1cercle_r050.tif", 50);
	ExportCercleTestImage("test_1cercle_r075.tif", 75);
	ExportCercleTestImage("test_1cercle_r100.tif",100);
	
	ExportRectangleTestImage("test_1rectangle_l050h020.tif", 25, 10);
	ExportRectangleTestImage("test_1rectangle_l100h100.tif", 50, 50);
}
