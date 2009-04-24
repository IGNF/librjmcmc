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

struct GILEnergyPolicy::mask_image_t: public gray8_image_t
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

GILEnergyPolicy::GILEnergyPolicy() : m_defaultEnergy(0.), m_gradients(new gradients_image_t), m_mask (new mask_image_t)
{}

void GILEnergyPolicy::Init(const std::string &nomIm, const std::string &nomMask, double defaultEnergy)
{
	m_defaultEnergy = defaultEnergy;
	double sigmaD = 1.;
	unsigned int half_size = 3*sigmaD;
	const size_t kws = 2 * half_size+1;
	kernel_1d<float> ksmooth(kws,kws/2);
	kernel_1d<float> kderiv(kws,kws/2);
	initKernelGaussian1D(ksmooth,sigmaD);
	initKernelGaussianDeriv1D(kderiv,sigmaD);

    gray16_image_t img;
    tiff_read_image(nomIm ,img);
	m_gradients->recreate(img.dimensions());

	tiff_read_image(nomMask, *m_mask);

	convolve_cols<gray32F_pixel_t>(const_view(img),ksmooth,kth_channel_view<0>(m_gradients->_view), convolve_option_extend_constant);
	convolve_rows<gray32F_pixel_t>(kth_channel_view<0>(m_gradients->_view),kderiv,kth_channel_view<0>(m_gradients->_view), convolve_option_extend_constant);

	convolve_rows<gray32F_pixel_t>(const_view(img),ksmooth,kth_channel_view<1>(m_gradients->_view), convolve_option_extend_constant);
    convolve_cols<gray32F_pixel_t>(kth_channel_view<1>(m_gradients->_view),kderiv,kth_channel_view<1>(m_gradients->_view), convolve_option_extend_constant);
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
//	tiff_write_view("gradients_x.tif", kth_channel_view<0>(m_gradients->_view));
//	tiff_write_view("gradients_y.tif", kth_channel_view<1>(m_gradients->_view));
}

double GILEnergyPolicy::ComputeDataEnergy(const Rectangle_2 &n) const
{
	double res = m_defaultEnergy;
	for (unsigned int i = 0; i < 4; ++i)
	{
		double delta = -ComputeSegmentDataEnergy(n.point(i),n.point(i + 1));
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
	dev2n32F_view_t::xy_locator loc_grad = m_gradients->_view.xy_at(gridIn.x(), gridIn.y());
	gray8_view_t::xy_locator loc_mask = m_mask->_view.xy_at(gridIn.x(), gridIn.y());
	point2<std::ptrdiff_t> mvt[2] = { point2<std::ptrdiff_t>(it.Step(0),0), point2<std::ptrdiff_t>(0,it.Step(1)) };
	for (; !it.end(); ++it)
	{
		const float length = it.length();
		dev2n32F_pixel_t grad = (*loc_grad);
		float mask = 1 - *loc_mask;
		gradient_sum[0] += length * at_c<0>(grad) * mask;
		gradient_sum[1] += length * at_c<1>(grad) * mask;
		loc_grad += mvt[it.axis()];
		loc_mask += mvt[it.axis()];
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

void GILEnergyPolicy::Add8CirclePoints(float xCenter, float yCenter, float dx, float dy, double & res) const
{
	Vector_2 vgrad, vnorm;
	dev2n32F_pixel_t grad;
	float mask;
	grad = m_gradients->_view(xCenter + dx, yCenter + dy);
	mask = m_mask->_view(xCenter + dx, yCenter + dy);
	vgrad = Vector_2(at_c<0>(grad) * mask, at_c<1>(grad) * mask);
	vnorm = Vector_2( dx, dy);
	res -= vgrad*vnorm;

	grad = m_gradients->_view(xCenter - dx, yCenter + dy);
	mask = m_mask->_view(xCenter - dx, yCenter + dy);
	vgrad = Vector_2(at_c<0>(grad) * mask, at_c<1>(grad) * mask);
	vnorm = Vector_2(-dx, dy);
	res -= vgrad*vnorm;

	grad = m_gradients->_view(xCenter - dx, yCenter - dy);
	mask = m_mask->_view(xCenter - dx, yCenter - dy);
	vgrad = Vector_2(at_c<0>(grad) * mask, at_c<1>(grad) * mask);
	vnorm = Vector_2(-dx,-dy);
	res -= vgrad*vnorm;

	grad = m_gradients->_view(xCenter + dx, yCenter - dy);
	mask = m_mask->_view(xCenter + dx, yCenter - dy);
	vgrad = Vector_2(at_c<0>(grad) * mask, at_c<1>(grad) * mask);
	vnorm = Vector_2( dx,-dy);
	res -= vgrad*vnorm;

	grad = m_gradients->_view(xCenter + dy, yCenter + dx);
	mask = m_mask->_view(xCenter + dy, yCenter + dx);
	vgrad = Vector_2(at_c<0>(grad) * mask, at_c<1>(grad) * mask);
	vnorm = Vector_2( dy, dx);
	res -= vgrad*vnorm;

	grad = m_gradients->_view(xCenter - dy, yCenter + dx);
	mask = m_mask->_view(xCenter - dy, yCenter + dx);
	vgrad = Vector_2(at_c<0>(grad) * mask, at_c<1>(grad) * mask);
	vnorm = Vector_2(-dy, dx);
	res -= vgrad*vnorm;

	grad = m_gradients->_view(xCenter - dy, yCenter - dx);
	mask = m_mask->_view(xCenter - dy, yCenter - dx);
	vgrad = Vector_2(at_c<0>(grad) * mask, at_c<1>(grad) * mask);
	vnorm = Vector_2(-dy,-dx);
	res -= vgrad*vnorm;

	grad = m_gradients->_view(xCenter + dy, yCenter - dx);
	mask = m_mask->_view(xCenter + dy, yCenter - dx);
	vgrad = Vector_2(at_c<0>(grad) * mask, at_c<1>(grad) * mask);
	vnorm = Vector_2( dy,-dx);
	res -= vgrad*vnorm;
}

// Pour ca, tout vient de 
// http://escience.anu.edu.au/lecture/cg/Circle/
// Corrige par :
// http://www.cplusplus.happycodings.com/Computer_Graphics/code16.html
double GILEnergyPolicy::ComputeDataEnergy(const Cercle_2 &n) const
{
	double res = 0.;
	float x = 0;
	float y = n.radius();
	float p = 3 - 2*n.radius();
	float xCenter = n.center().x(), yCenter = n.center().y();
    Add8CirclePoints(xCenter, yCenter, x, y, res);
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
        Add8CirclePoints(xCenter, yCenter, x, y, res);
    } 
    
	return (res / n.radius()) + m_defaultEnergy;
}

struct ImageExporter::export_image_t : public gray8_image_t
{
};

ImageExporter::ImageExporter() : m_img(new export_image_t)
{}

void ImageExporter::InitExport(const char *filename) const
{
	if (std::string(filename).empty())
	{
		m_img->recreate(700, 700);
		fill_pixels(m_img->_view, 0);
	}
	else
	{
		m_img->recreate(tiff_read_dimensions(filename));
		tiff_read_and_convert_view(filename, m_img->_view, dummy_color_converter());
	}
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

void ImageExporter::ExportNode(const Segment_2 &s) const
{
	CGAL::Segment_2_iterator<Segment_2> it(s);
	for (; !it.end(); ++it)
	{
		m_img->_view(it.x(), it.y()) = 255;
	}
}

void ImageExporter::ExportNode(const Rectangle_2 &n) const
{
	for (unsigned int i = 0; i < 4; ++i)
	{
		ExportNode(Segment_2(n.point(i),n.point(i + 1)));
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
	ExportCercleTestImage("test_1cercle_r025.tif", 25);
	ExportCercleTestImage("test_1cercle_r050.tif", 50);
	ExportCercleTestImage("test_1cercle_r075.tif", 75);
	ExportCercleTestImage("test_1cercle_r100.tif",100);
	
	ExportRectangleTestImage("test_1rectangle_l050h020.tif", 25, 10);
	ExportRectangleTestImage("test_1rectangle_l100h100.tif", 50, 50);
}
