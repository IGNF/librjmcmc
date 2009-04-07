#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/matis/float_images.hpp>
#include <boost/gil/extension/io/tiff_dynamic_io.hpp>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>

#include <boost/gil/extension/matis/deriche.hpp>

#include "BuildingsDetectorParameters.hpp"

#include "GILEnergyPolicy.hpp"

#include "ImageGradientEnergyPolicy.hpp"


ImageGradientEnergyPolicy::ImageGradientEnergyPolicy() :
	m_policy( boost::shared_ptr<GILPolicyImage>(new GILPolicyImage) )
{
	m_policy->LoadFile(BuildingsDetectorParametersSingleton::Instance()-> InputDataFilePath());
	m_policy->m_defaultEnergy = BuildingsDetectorParametersSingleton::Instance()->IndividualEnergy();
}

ImageGradientEnergyPolicy::~ImageGradientEnergyPolicy()
{
}

double ImageGradientEnergyPolicy::ComputeDataEnergy(const Rectangle_2 &n) const
{
	return m_policy->ComputeDataEnergy(n);
}

double ImageGradientEnergyPolicy::ComputeDataEnergy(const Cercle_2 &n) const
{
	return m_policy->ComputeDataEnergy(n);
}

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

void GILPolicyImage::LoadFile(const std::string &str)
{
	double sigmaD = 1.;
	unsigned int half_size = 3*sigmaD;
	const size_t kws = 2 * half_size+1;
	kernel_1d<float> ksmooth(kws,kws/2);
	kernel_1d<float> kderiv(kws,kws/2);
	initKernelGaussian1D(ksmooth,sigmaD);
	initKernelGaussianDeriv1D(kderiv,sigmaD);

    gray16_image_t img;
    tiff_read_image(BuildingsDetectorParametersSingleton::Instance()->InputDataFilePath(),img);

	m_gradients.recreate(img.dimensions());
	m_imageWidth = img.dimensions().x;
	m_imageHeight = img.dimensions().y;


	convolve_cols<gray32F_pixel_t>(const_view(img),ksmooth,kth_channel_view<0>(m_gradients._view), convolve_option_extend_constant);
	convolve_rows<gray32F_pixel_t>(kth_channel_view<0>(m_gradients._view),kderiv,kth_channel_view<0>(m_gradients._view), convolve_option_extend_constant);

	convolve_rows<gray32F_pixel_t>(const_view(img),ksmooth,kth_channel_view<1>(m_gradients._view), convolve_option_extend_constant);
    convolve_cols<gray32F_pixel_t>(kth_channel_view<1>(m_gradients._view),kderiv,kth_channel_view<1>(m_gradients._view), convolve_option_extend_constant);

    gray32F_image_t module(img.dimensions());
    gray8_image_t filtered_module(img.dimensions());
    ModuleGradient( kth_channel_view<0>(m_gradients._view), kth_channel_view<1>(m_gradients._view), view(module));
    NonMaximaLocauxGradient (
							  kth_channel_view<0>(m_gradients._view) ,
							  kth_channel_view<1>(m_gradients._view) ,
							  view(module) ,
							  view(filtered_module) ,
							  5.);

	gray8_view_t::iterator it_filtered = view(filtered_module).begin();
	dev2n32F_view_t::iterator it_gradients = m_gradients._view.begin();

	for( ; it_filtered!=view(filtered_module).begin() ; ++it_filtered , ++it_gradients )
	{
		if ( *it_filtered==0 )
		{
			dev2n32F_pixel_t &grad = m_gradients._view(it_gradients.x_pos(), it_gradients.y_pos());
			at_c<0>(grad) = 0.;
			at_c<1>(grad) = 0.;
		}
	}
}

double GILPolicyImage::ComputeDataEnergy(const Rectangle_2 &n) const
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
double GILPolicyImage::ComputeSegmentDataEnergy(const Point_2 &gridIn, const Point_2 &gridOut) const
{
	float gradient_sum[2] = {0.,0.};

	Segment_2 s(gridIn, gridOut);
	CGAL::Segment_2_iterator<Segment_2> it(s);
	dev2n32F_view_t::xy_locator loc_grad = m_gradients._view.xy_at(gridIn.x(), gridIn.y());
	point2<std::ptrdiff_t> movement[2] = { point2<std::ptrdiff_t>(it.Step(0),0), point2<std::ptrdiff_t>(0,it.Step(1)) };
	for (; !it.end(); ++it)
	{
		const float length = it.length();
		const dev2n32F_pixel_t &grad = *loc_grad;
		gradient_sum[0] += length * at_c<0>(grad);
		gradient_sum[1] += length * at_c<1>(grad);
		loc_grad += movement[it.axis()];
//		std::cout << "new : " << at_c<0>(grad) << "\t" << at_c<1>(grad) << std::endl;
//		const dev2n32F_pixel_t old_grad = m_gradients._view(it.x(), it.y());
//		std::cout << "old : " << at_c<0>(old_grad) << "\t" << at_c<1>(old_grad) << std::endl;
	}

	Vector_2 arete(gridIn, gridOut);
	// TODO : NEGATIVE or POSITIVE ???
	Vector_2 normale = arete.perpendicular(CGAL::NEGATIVE);
	Vector_2 sum(gradient_sum[0], gradient_sum[1]);
	return CGAL::to_double(normale * sum);
}

void GILPolicyImage::Add8CirclePoints(int xCenter, int yCenter, int dx, int dy, double & res) const
{
	Vector_2 vgrad, vnorm;
	dev2n32F_pixel_t grad;
	grad = m_gradients._view(xCenter + dx, yCenter + dy);
	vgrad = Vector_2(at_c<0>(grad), at_c<1>(grad));
	vnorm = Vector_2( dx, dy);
	res -= vgrad*vnorm;

	grad = m_gradients._view(xCenter - dx, yCenter + dy);
	vgrad = Vector_2(at_c<0>(grad), at_c<1>(grad));
	vnorm = Vector_2(-dx, dy);
	res -= vgrad*vnorm;

	grad = m_gradients._view(xCenter - dx, yCenter - dy);
	vgrad = Vector_2(at_c<0>(grad), at_c<1>(grad));
	vnorm = Vector_2(-dx,-dy);
	res -= vgrad*vnorm;

	grad = m_gradients._view(xCenter + dx, yCenter - dy);
	vgrad = Vector_2(at_c<0>(grad), at_c<1>(grad));
	vnorm = Vector_2( dx,-dy);
	res -= vgrad*vnorm;

	grad = m_gradients._view(xCenter + dy, yCenter + dx);
	vgrad = Vector_2(at_c<0>(grad), at_c<1>(grad));
	vnorm = Vector_2( dy, dx);
	res -= vgrad*vnorm;

	grad = m_gradients._view(xCenter - dy, yCenter + dx);
	vgrad = Vector_2(at_c<0>(grad), at_c<1>(grad));
	vnorm = Vector_2(-dy, dx);
	res -= vgrad*vnorm;

	grad = m_gradients._view(xCenter - dy, yCenter - dx);
	vgrad = Vector_2(at_c<0>(grad), at_c<1>(grad));
	vnorm = Vector_2(-dy,-dx);
	res -= vgrad*vnorm;

	grad = m_gradients._view(xCenter + dy, yCenter - dx);
	vgrad = Vector_2(at_c<0>(grad), at_c<1>(grad));
	vnorm = Vector_2( dy,-dx);
	res -= vgrad*vnorm;
}

// Pour ca, tout vient de 
// http://escience.anu.edu.au/lecture/cg/Circle/
// Corrige par :
// http://www.cplusplus.happycodings.com/Computer_Graphics/code16.html
double GILPolicyImage::ComputeDataEnergy(const Cercle_2 &n) const
{
	double res = m_defaultEnergy;
	int x = 0;
	int y = n.radius();
	int p = 3 - 2*n.radius();
	int xCenter = n.center().x(), yCenter = n.center().y();
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
	return res / n.radius();
}

gray8_image_t img;

void GILPolicyImage::InitExport() const
{
	img.recreate(m_gradients.dimensions());
	fill_pixels(img._view, 0);
}

void GILPolicyImage::EndExport(const char *filename) const
{
	tiff_write_view(filename, img._view);
}

void GILPolicyImage::Export8Points(int xCenter, int yCenter, int dx, int dy) const
{
	img._view(xCenter + dx, yCenter + dy) = 255;
	img._view(xCenter - dx, yCenter + dy) = 255;
	img._view(xCenter - dx, yCenter - dy) = 255;
	img._view(xCenter + dx, yCenter - dy) = 255;
	img._view(xCenter + dy, yCenter + dx) = 255;
	img._view(xCenter - dy, yCenter + dx) = 255;
	img._view(xCenter - dy, yCenter - dx) = 255;
	img._view(xCenter + dy, yCenter - dx) = 255;
}

void GILPolicyImage::ExportNode(const Cercle_2 &n) const
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

void ImageGradientEnergyPolicy::InitExport() const { m_policy->InitExport(); }
void ImageGradientEnergyPolicy::ExportNode(const Cercle_2 &n) const {m_policy->ExportNode(n); }
void ImageGradientEnergyPolicy::EndExport(const char *filename) const { m_policy->EndExport(filename); }
