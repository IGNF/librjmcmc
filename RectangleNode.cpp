#include "stdafx.h"

#include "RectangleNode.hpp"
#include "BuildingsDetectorParameters.hpp"

#include <boost/gil/extension/matis/deriche.hpp>

bool RectangleNode::IsValid(const BBox &box) /*const*/
{
	for (unsigned int i=0; i<4; ++i)
	{
		BBox::PointType ptb;
		m_rect.point(i, ptb);

		if ( !box.IsInside(ptb) )
		{
			return false;
		}
	}
	float minSize = (float)BuildingsDetectorParametersSingleton::Instance()->RectangleMinimalSize();
	if ( m_rect.squared_length(0) < minSize || m_rect.squared_length(1) < minSize)
		return false;
	// Finalement, on le met dans le "bon" sens ...
	if ( m_rect.orientation() == CGAL::CLOCKWISE )
		m_rect = m_rect.opposite();

	float ratio = (float)std::max(m_rect.ratio(), 1./m_rect.ratio());
	if (ratio > BuildingsDetectorParametersSingleton::Instance()->RectangleMaximalRatio())
		return false;

	return true;
}

void RectangleNode::RandomModify(const BBox &box)
{
	CRectangle_2 oldRect = Rect();
	boost::variate_generator<RJMCMCRandom&, boost::uniform_smallint<> >
			diechoix(GetRandom(), boost::uniform_smallint<>(0, 2));
	int choix = diechoix();

	// On suffgle les points afin de pouvoir varier dans toutes les dimensions (venir voir Olivier si pas compris ...)
	boost::variate_generator<RJMCMCRandom&, boost::uniform_smallint<> >
			doshuffle(GetRandom(), boost::uniform_smallint<>(0, 1));

	bool shuffle = false;
	if ( doshuffle() )
		shuffle = true;

	boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > die(GetRandom(), boost::uniform_real<>(-10, 10));
	std::vector<Point_2> pts;
	pts.push_back( Rect().point(0) );
	pts.push_back( Rect().point(1) );
	pts.push_back( Rect().point(2) );

	pts[choix] = Point_2(pts[choix].x() + die(), pts[choix].y() + die());

	if ( shuffle )
		std::random_shuffle( pts.begin() , pts.end() );
	m_rect = CRectangle_2(pts[0], pts[1], pts[2]);

	if (!IsValid(box))
	{
		m_rect = oldRect;
		RandomModify(box);
	}
}

void RectangleNode::RandomInit(const BBox &box)
{
	boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > diex(
			GetRandom(), boost::uniform_real<>(box.Min()[0], box.Max()[0]));
	boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > diey(
			GetRandom(), boost::uniform_real<>(box.Min()[1], box.Max()[1]));

	Point_2 p(diex(), diey()), q(diex(), diey()), r(diex(), diey());
	while (p == q)
		q = Point_2(diex(), diey());

	m_rect = CRectangle_2(p, q, r);

	if (!IsValid(box))
		RandomInit(box);
}

std::ostream & operator<<(std::ostream &o, const RectangleNode &node)
{
	//	o << node.Rect().center().x() << "\t" << node.Rect().center().y() << "\t" << node.Rect().ratio() << "\t" << node.Rect().normal().x() << "\t" << node.Rect().normal().y();
	for (unsigned int i = 0; i < 4; ++i)
		o << node.Rect().point(i).x() << ";" << node.Rect().point(i).y()
				<< "\t";
	return o;
}

RectanglePriorEnergyPolicy::RectanglePriorEnergyPolicy() : m_coefSurface(BuildingsDetectorParametersSingleton::Instance()->IntersectionSurfacePonderation())
{}

bool RectanglePriorEnergyPolicy::AreNeighbor(const RectangleNode &n1,
		const RectangleNode &n2)
{
	return n1.Rect().do_intersect(n2.Rect());
}

double RectanglePriorEnergyPolicy::ComputePriorEnergy(const RectangleNode &n1,
		const RectangleNode &n2)
{
	double inter = std::abs(CGAL::to_double(n1.Rect().intersection_area(n2.Rect())));
	return m_coefSurface * inter;
	/*if ( !n1.Rect().do_intersect(n2.Rect()) )
	 {
	 // return 0.;
	 double cos2 = std::abs(CGAL::to_double(n1.Rect().squared_cos(n2.Rect())));
	 return -5.*cos2*cos2*cos2;
	 }*/
	/*
	 double surf1 = CGAL::to_double(CGAL::abs(n1.Rect().area()));;
	 double surf2 = CGAL::to_double(CGAL::abs(n2.Rect().area()));;
	 double combined = std::abs(CGAL::to_double( n1.Rect().combination_area(n2.Rect()) / std::min(surf1, surf2)));
	 return - 100. * exp(-combined );
	 */
	//exp( - combined / 1. );
	/*
	 double dx = n1.X() - n2.X();
	 double dy = n1.Y() - n2.Y();
	 double d2 = dx * dx + dy * dy ;
	 if(d2==0)
	 return 10000; // valeur bidon
	 double d = ::sqrt(d2);
	 double x = d-m_mean;
	 if ( d > (m_mean + m_sigma))
	 return 0.;
	 return (x-m_sigma)*(x+m_sigma)/(d2*d);
	 */
}

RectanglePointsPriorEnergyPolicy::RectanglePointsPriorEnergyPolicy() :
	m_coefSurface(BuildingsDetectorParametersSingleton::Instance()->IntersectionSurfacePonderation()),
	m_coefDistance(BuildingsDetectorParametersSingleton::Instance()->PointsDistancePonderation()),
	m_distMax(BuildingsDetectorParametersSingleton::Instance()->PointsDistanceMax())
{
	m_distMax2 = m_distMax * m_distMax;
}


bool RectanglePointsPriorEnergyPolicy::AreNeighbor(const RectangleNode &n1, const RectangleNode &n2)
{
	double distmin2 = (n1.Rect().center() - n2.Rect().center()).squared_length();
	if (distmin2 < GetDistMax2())
		return true;
	double distmin = ::sqrt(distmin2);
	distmin -= ::sqrt( ( 1. + n1.Rect().ratio() * n1.Rect().ratio()) * n1.Rect().normal().squared_length() );
	if (distmin < GetDistMax())
		return true;
	distmin -= ::sqrt( ( 1. + n2.Rect().ratio() * n2.Rect().ratio()) * n2.Rect().normal().squared_length() );
	return (distmin < GetDistMax2());
}

double RectanglePointsPriorEnergyPolicy::ComputePriorEnergy(const RectangleNode &n1, const RectangleNode &n2)
{
	double inter = std::abs(CGAL::to_double(n1.Rect().intersection_area(n2.Rect())));
	if (inter > 0 )
		return m_coefSurface * inter;
	if ( std::abs(m_coefDistance) < 1E-6 )
		return 0.;
	double distmin2 = GetDistMax2();
	unsigned int imin = 5, jmin = 5;
	for (unsigned int i=0; i<4;++i)
	{
		Point_2 p1 = n1.Rect().point(i);
		for (unsigned int j=0; j<4;++j)
		{
			Point_2 p2 = n2.Rect().point(j);
			double dist2 = (p2 - p1).squared_length();
			if (dist2 < distmin2)
			{
				distmin2 = dist2;
				imin = i;
				jmin = j;
			}
		}
	}
	if (distmin2 < GetDistMax2())
	{
		double energy = -m_coefDistance * ::sqrt(GetDistMax2() - distmin2);
		distmin2 = GetDistMax2();
		for (unsigned int i=0; i<4;++i)
		{
			if (i == imin)
				continue;
			Point_2 p1 = n1.Rect().point(i);
			for (unsigned int j=0; j<4;++j)
			{
				if (j == jmin)
					continue;
				Point_2 p2 = n2.Rect().point(j);
				double dist2 = (p2 - p1).squared_length();
				if (dist2 < distmin2)
				{
					distmin2 = dist2;
				}
			}
		}
		if (distmin2 < GetDistMax2())
			energy += -m_coefDistance * ::sqrt(GetDistMax2() - distmin2);
		return energy;
	}
	else
		return 0.;
}

double SurfaceRectangleDataEnergyPolicy::ComputeDataEnergy(const RectangleNode &n) const
{
	//	return -std::abs(CGAL::to_double(n.Rect().area()));
	return -::log(1. + std::abs(CGAL::to_double(n.Rect().area())));
}

class GILPolicyImage
{
public:

	double ComputeSegmentDataEnergy(const Point_2 &gridIn,const Point_2 &gridOut) const;

	void LoadFile(const std::string &str);
	double ComputeDataEnergy(const RectangleNode &n) const;

	int PosToOffset(const Point_2 &pt) const;
	Point_2 OffsetToPos(int off) const;

	dev2n32F_image_t m_gradients;
	unsigned int m_imageWidth;
	unsigned int m_imageHeight;

	double m_defaultEnergy;
};

ImageGradientEnergyPolicy::ImageGradientEnergyPolicy() :
	m_policy( boost::shared_ptr<GILPolicyImage>(new GILPolicyImage) )
{
	m_policy->LoadFile(BuildingsDetectorParametersSingleton::Instance()-> InputDataFilePath());
	m_policy->m_defaultEnergy = BuildingsDetectorParametersSingleton::Instance()->IndividualEnergy();
}

ImageGradientEnergyPolicy::~ImageGradientEnergyPolicy()
{
}

double ImageGradientEnergyPolicy::ComputeDataEnergy(const RectangleNode &n) const
{
	return m_policy->ComputeDataEnergy(n);
}

#ifdef WIN32
	const double M_PI = 4.0 * atan(1.0);
#endif // WIN32

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

double GILPolicyImage::ComputeDataEnergy(const RectangleNode &n) const
{
	double res = m_defaultEnergy;
	for (unsigned int i = 0; i < 4; ++i)
	{
		double delta = -ComputeSegmentDataEnergy(n.Rect().point(i),n.Rect().point(i + 1));
		if ( delta <= 0. )
			res += delta;
	}
	return res;
}

int GILPolicyImage::PosToOffset(const Point_2 &pt) const
{
	return m_imageWidth * std::floor(pt.y()) + std::floor(pt.x());
}

Point_2 GILPolicyImage::OffsetToPos(int off) const
{
	return Point_2(off % m_imageWidth, off / m_imageWidth);
}

double GILPolicyImage::ComputeSegmentDataEnergy(const Point_2 &gridIn,
		const Point_2 &gridOut) const
{
	float gradient_sum[2] = {0.,0.};

	Segment_2 s(gridIn, gridOut);
	CGAL::Segment_2_iterator<Segment_2> it(s);
	for (; !it.end(); ++it)
	{
		// Calcul de la direction du gradient ...
		const float length = it.length();
		const dev2n32F_pixel_t grad = m_gradients._view(it.x(), it.y());
		gradient_sum[0] += length * at_c<0>(grad);
		gradient_sum[1] += length * at_c<1>(grad);
	}

	Vector_2 arete(gridIn, gridOut);
	// TODO : NEGATIVE or POSITIVE ???
	Vector_2 normale = arete.perpendicular(CGAL::NEGATIVE);
	Vector_2 sum(gradient_sum[0], gradient_sum[1]);
	return CGAL::to_double(normale * sum);
}
