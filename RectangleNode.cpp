#include "Random.hpp"
#include "BuildingsDetectorParameters.hpp"
#include "RectangleNode.hpp"

bool RectangleNode::IsValid(const BBox &box) const
{
	Vector_2 n = m_geometry.normal();
	float anx = std::abs(n.x());
	float any = std::abs(n.y());
	float dx = anx + m_geometry.ratio()*any;
	float dy = any + m_geometry.ratio()*anx;
	Point_2 c = m_geometry.center();
	if ((c.x()-dx < box.Min()[0]) || (c.y()-dy < box.Min()[1]) || (c.x()+dx > box.Max()[0]) || (c.y()+dy > box.Max()[1]))
		return false;

	// TODO : optimize le carre est calcule a chaque fois !!!
	float minSize = (float)BuildingsDetectorParametersSingleton::Instance()->RectangleMinimalSize();
	minSize *= minSize;
	float length0 = 4*m_geometry.normal().squared_length();
	float length1 = m_geometry.ratio()*m_geometry.ratio()*length0;
	if ( length0 < minSize || length1 < minSize)
		return false;

	if ((m_geometry.ratio() > BuildingsDetectorParametersSingleton::Instance()->RectangleMaximalRatio()) ||
		(1./m_geometry.ratio() > BuildingsDetectorParametersSingleton::Instance()->RectangleMaximalRatio()))
		return false;

	return true;
}

void RectangleNode::RandomInit(const BBox &box)
{
	boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > diex(GetRandom(), 
		boost::uniform_real<>(box.Min()[0], box.Max()[0]));
	boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > diey(GetRandom(), 
		boost::uniform_real<>(box.Min()[1], box.Max()[1]));

<<<<<<< .mine
	boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > dievec( GetRandom(), 
			boost::uniform_real<>(BuildingsDetectorParametersSingleton::Instance()->RectangleMinimalSize(), BuildingsDetectorParametersSingleton::Instance()->RectangleMaximalSize()));
=======
	// On suffle les points afin de pouvoir varier dans toutes les dimensions (venir voir Olivier si pas compris ...)
	boost::variate_generator<RJMCMCRandom&, boost::uniform_smallint<> >
			doshuffle(GetRandom(), boost::uniform_smallint<>(0, 1));
>>>>>>> .r31

	Point_2 p(diex(), diey());
	Point_2 q(p.x() + dievec(), p.y() + dievec());
	Point_2 r(q.x() + dievec(), q.y() + dievec());
	while (p == q)
		q = Point_2(p.x() + dievec(), p.y() + dievec());

	m_geometry = CRectangle_2(p, q, r);

	// On le met dans le "bon" sens ...
	if ( m_geometry.orientation() == CGAL::CLOCKWISE )
		m_geometry.reverse_orientation ();

	if (!IsValid(box))
		RandomInit(box);
}

void RectangleNode::RandomModify(const BBox &box)
{
	CRectangle_2 oldRect = Geometry();
	std::vector<Point_2> pts;
	pts.reserve(3);
	pts.push_back( Geometry().point(0) );
	pts.push_back( Geometry().point(1) );
	pts.push_back( Geometry().point(2) );

	boost::variate_generator<RJMCMCRandom&, boost::uniform_smallint<> >
			diechoix(GetRandom(), boost::uniform_smallint<>(0, 2));
	int choix = diechoix();
	boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > die(GetRandom(), boost::uniform_real<>(-10, 10));
	pts[choix] = Point_2(pts[choix].x() + die(), pts[choix].y() + die());

	// On shuffle les points afin de pouvoir varier dans toutes les dimensions (venir voir Olivier si pas compris ...)
	boost::variate_generator<RJMCMCRandom&, boost::uniform_smallint<> >
			doshuffle(GetRandom(), boost::uniform_smallint<>(0, 1));
	bool shuffle = doshuffle();	
	if ( shuffle )
		std::random_shuffle( pts.begin() , pts.end() );

	m_geometry = CRectangle_2(pts[0], pts[1], pts[2]);

	// On le met dans le "bon" sens ...
	if ( m_geometry.orientation() == CGAL::CLOCKWISE )
		m_geometry.reverse_orientation ();
	if (!IsValid(box))
	{
		m_geometry = oldRect;
		RandomModify(box);
	}
}


std::ostream & operator<<(std::ostream &o, const RectangleNode &node)
{
	//	o << node.Geometry().center().x() << "\t" << node.Geometry().center().y() << "\t" << node.Geometry().ratio() << "\t" << node.Geometry().normal().x() << "\t" << node.Geometry().normal().y();
	for (unsigned int i = 0; i < 4; ++i)
		o << node.Geometry().point(i).x() << ";" << node.Geometry().point(i).y()
				<< "\t";
	return o;
}

RectanglePriorEnergyPolicy::RectanglePriorEnergyPolicy() : m_coefSurface(BuildingsDetectorParametersSingleton::Instance()->IntersectionSurfacePonderation())
{}

RectanglePointsPriorEnergyPolicy::RectanglePointsPriorEnergyPolicy() :
	m_coefSurface(BuildingsDetectorParametersSingleton::Instance()->IntersectionSurfacePonderation()),
	m_coefDistance(BuildingsDetectorParametersSingleton::Instance()->PointsDistancePonderation()),
	m_distMax(BuildingsDetectorParametersSingleton::Instance()->PointsDistanceMax())
{
	m_distMax2 = m_distMax * m_distMax;
}

bool RectanglePointsPriorEnergyPolicy::AreNeighbor(const RectangleNode &n1, const RectangleNode &n2)
{
	double distmin2 = (n1.Geometry().center() - n2.Geometry().center()).squared_length();
	if (distmin2 < GetDistMax2())
		return true;
	double distmin = ::sqrt(distmin2);
	distmin -= ::sqrt( ( 1. + n1.Geometry().ratio() * n1.Geometry().ratio()) * n1.Geometry().normal().squared_length() );
	if (distmin < GetDistMax())
		return true;
	distmin -= ::sqrt( ( 1. + n2.Geometry().ratio() * n2.Geometry().ratio()) * n2.Geometry().normal().squared_length() );
	return (distmin < GetDistMax2());
}

double RectanglePointsPriorEnergyPolicy::ComputePriorEnergy(const RectangleNode &n1, const RectangleNode &n2)
{
	double inter = std::abs(CGAL::to_double(n1.Geometry().intersection_area(n2.Geometry())));
	if (inter > 0 )
		return m_coefSurface * inter;
	if ( std::abs(m_coefDistance) < 1E-6 )
		return 0.;
	double distmin2 = GetDistMax2();
	unsigned int imin = 5, jmin = 5;
	for (unsigned int i=0; i<4;++i)
	{
		Point_2 p1 = n1.Geometry().point(i);
		for (unsigned int j=0; j<4;++j)
		{
			Point_2 p2 = n2.Geometry().point(j);
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
			Point_2 p1 = n1.Geometry().point(i);
			for (unsigned int j=0; j<4;++j)
			{
				if (j == jmin)
					continue;
				Point_2 p2 = n2.Geometry().point(j);
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
	//	return -std::abs(CGAL::to_double(n.Geometry().area()));
	return -::log(1. + std::abs(CGAL::to_double(n.Geometry().area())));
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

//TODO: improve --> 40%
double GILPolicyImage::ComputeSegmentDataEnergy(const Point_2 &gridIn,
		const Point_2 &gridOut) const
{
	float gradient_sum[2] = {0.,0.};

	Segment_2 s(gridIn, gridOut);
	CGAL::Segment_2_iterator<Segment_2> it(s);
	dev2n32F_view_t::xy_locator loc_grad = m_gradients._view.xy_at(gridIn.x(), gridIn.y());
	dev2n32F_view_t::xy_locator::cached_location_t movement[2] = {loc_grad.cache_location(it.Step(0),0), loc_grad.cache_location(0,it.Step(1)) };
//	point2<std::ptrdiff_t> movement[2] = { point2<std::ptrdiff_t>(it.Step(0),0), point2<std::ptrdiff_t>(0,it.Step(1)) };
	for (; !it.end(); ++it)
	{
		const float length = it.length();
		const dev2n32F_pixel_t grad = *loc_grad;
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
