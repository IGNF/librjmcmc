#include "RectangleNode.hpp"

#include "BuildingsDetectorParameters.hpp"

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

	boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > die(
			GetRandom(), boost::uniform_real<>(-10, 10));
	Point_2 pts[] =
	{ Rect().point(0), Rect().point(1), Rect().point(2) };
	pts[choix] = Point_2(pts[choix].x() + die(), pts[choix].y() + die());;
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
	if (inter > 0)
		return m_coefSurface * inter;
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

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkDiscreteGaussianImageFilter.h>
#include <itkGradientImageFilter.h>
#include <itkImageRegionIterator.h>
#include <itkImageFileWriter.h>
#include <itkLineConstIterator.h>

class itkPolicyImage
{
public:

	typedef signed short PixelType;
	typedef itk::CovariantVector<float,2> CovariantPixelType;
	typedef itk::Image<CovariantPixelType> CovariantImageType;
	double ComputeSegmentDataEnergy(const Point_2 &gridIn,const Point_2 &gridOut) const;

	void LoadFile(const std::string &str);
	double ComputeDataEnergy(const RectangleNode &n) const;

	int PosToOffset(const Point_2 &pt) const;
	Point_2 OffsetToPos(int off) const;

	CovariantImageType::Pointer m_gradients;
	unsigned int m_imageWidth;
	unsigned int m_imageHeight;

	double m_defaultEnergy;
};

ImageGradientEnergyPolicy::ImageGradientEnergyPolicy() :
	m_policy( boost::shared_ptr<itkPolicyImage>(new itkPolicyImage) )
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

void itkPolicyImage::LoadFile(const std::string &str)
{
	typedef itk::Image<PixelType,2> InputImageType;
	typedef itk::ImageFileReader<InputImageType> ImageReaderType;
	ImageReaderType::Pointer reader = ImageReaderType::New();
	reader->SetFileName(str);

	typedef itk::DiscreteGaussianImageFilter<InputImageType,InputImageType>
			GaussianFilterType;
	GaussianFilterType::Pointer gaussianFilter = GaussianFilterType::New();
	gaussianFilter->SetInput(reader->GetOutput());
	gaussianFilter->SetVariance(BuildingsDetectorParametersSingleton::Instance()->VarianceGaussianFilter());

	typedef itk::GradientImageFilter<InputImageType/*,float,float*/>
			GradientFilterType;
	GradientFilterType::Pointer gradientFilter = GradientFilterType::New();
	gradientFilter->SetInput(gaussianFilter->GetOutput());

	try
	{
		gradientFilter->Update();
		m_gradients = gradientFilter->GetOutput();

		m_imageWidth = m_gradients->GetLargestPossibleRegion().GetSize()[0];
		m_imageHeight = m_gradients->GetLargestPossibleRegion().GetSize()[1];
/*
		typedef itk::Image<itk::Vector<float,3>,2> ImageOut;
		typedef itk::ImageFileWriter<ImageOut> WriterType;

		ImageOut::Pointer monimage = ImageOut::New();
		ImageOut::RegionType region = m_gradients->GetLargestPossibleRegion();
		monimage->SetRegions(region);
		monimage->Allocate();

		typedef itk::ImageRegionIterator<ImageOut> iterator;
		iterator it_out(monimage, monimage->GetLargestPossibleRegion());
		it_out.GoToBegin();

		typedef itk::ImageRegionIterator<CovariantImageType> InIterator;
		InIterator it_in(m_gradients, m_gradients->GetLargestPossibleRegion());
		it_in.GoToBegin();

		for (; !it_out.IsAtEnd(); ++it_in, ++it_out)
		{
			it_out.Value()[0] = it_in.Value()[0];
			it_out.Value()[1] = it_in.Value()[1];
			it_out.Value()[2] = 0;
		}

		WriterType::Pointer w = WriterType::New();
		w->SetInput(monimage);
		w->SetFileName("Gradients.tif");
		w->Update();
*/
	}
	catch (const itk::ExceptionObject &e)
	{
		std::cout << e.what() << std::endl;
	}
}

double itkPolicyImage::ComputeDataEnergy(const RectangleNode &n) const
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

int itkPolicyImage::PosToOffset(const Point_2 &pt) const
{
	return m_imageWidth * std::floor(pt.y()) + std::floor(pt.x());
}

Point_2 itkPolicyImage::OffsetToPos(int off) const
{
	return Point_2(off % m_imageWidth, off / m_imageWidth);
}

double itkPolicyImage::ComputeSegmentDataEnergy(const Point_2 &gridIn,
		const Point_2 &gridOut) const
{
	/*
	 int Pos = PosToOffset(gridIn);
	 int Out = PosToOffset(gridOut);

	 const Point_2 next[] = {OffsetToPos(Pos),OffsetToPos(Pos+m_imageWidth+1)};

	 // Set up 2D DDA for ray
	 float NextCrossingT[2], DeltaT[2];
	 int Step[2];
	 Vector_2 arete(gridIn,gridOut);
	 float length = ::sqrt(arete.squared_length());
	 for (int axis = 0; axis < 2; ++axis)
	 {
	 int dir = (arete[axis] >= 0);
	 float invrayd = 1.0f / arete[axis];
	 NextCrossingT[axis] = (next[dir][axis] - gridIn[axis])*invrayd;
	 if(NextCrossingT[axis]>1.f) NextCrossingT[axis]=1.f;
	 Step [axis] = 2*dir-1;
	 DeltaT[axis] = Step[axis] * invrayd;
	 }
	 Step[1] *= m_imageWidth;
	 int stepAxis = NextCrossingT[1] < NextCrossingT[0];
	 float t = 0; // cell entering time

	 // Calcul de la normale normalisee a l'arete
	 Vector_2 normale_normalisee = arete.perpendicular( CGAL::POSITIVE ) / length;

	 //double maxcos=0;
	 double res = 0.;
	 // Walk grid
	 for (;;)
	 {
	 // test current cell (Pos)
	 Point_2 cur_pos = OffsetToPos(Pos);

	 // Calcul de la direction du gradient ...
	 CovariantImageType::IndexType index;
	 index[0] = cur_pos.x();
	 index[1] = cur_pos.y();
	 //std::cout << "index = " << index << std::endl;


	 CovariantPixelType pixel = m_gradients->GetPixel( index );
	 Vector_2 gradient((double) pixel[0],(double) pixel[1]);
	 res += (NextCrossingT[stepAxis]-t) * normale_normalisee * gradient;
	 //std::cout << index << "\t" << cur_pos << "\t" << pixel << "\t" << gradient * gradient << std::endl;
	 //if(maxcos<index[1])	maxcos=index[1];

	 //res += m_img->GetRed( cur_pos.x(), cur_pos.y() );
	 //std::cout << "res = " << res << std::endl;


	 // Advance to next cell
	 t = NextCrossingT[stepAxis];
	 if (Pos==Out)
	 return res*length;
	 Pos += Step[stepAxis];
	 NextCrossingT[stepAxis] += DeltaT[stepAxis];
	 stepAxis = NextCrossingT[1] < NextCrossingT[0];
	 if(NextCrossingT[stepAxis]>1.f) NextCrossingT[stepAxis]=1.f;
	 }
	 return res*length;
	 */
	//float gradient_sum[]={0, 0};
	float gradient_sum[2];
	gradient_sum[0] = 0.;
	gradient_sum[1] = 0.;

	double res = 0.;
/*
	CovariantImageType::IndexType index_b, index_e;
	index_b[0]=gridIn.x();
	index_b[1]=gridIn.y();
	index_e[0]=gridOut.x();
	index_e[1]=gridOut.y();
	itk::LineConstIterator<CovariantImageType> iit(m_gradients, index_b, index_e);
	for (;!iit.IsAtEnd(); ++iit)
	{
		gradient_sum[0] += iit.Get()[0];
		gradient_sum[1] += iit.Get()[1];
	}
*/
	/*
	Segment_2 s(gridIn, gridOut);
	itk::ImageRegionConstIterator< CovariantImageType > iit(m_gradients, m_gradients->GetLargestPossibleRegion());
	CGAL::Segment_2_iterator<Segment_2> it(s);
	{
		CovariantImageType::IndexType index;
		index[0]=it.x();
		index[1]=it.y();
		iit.SetIndex(index);
	}

	for (; !it.end(); ++it)
	{
		if (it.axis() == 0)
			if (it.step() >0)
				++iit;
			else
				--iit;
		else
		{
			CovariantImageType::IndexType index;
			index[0]=it.x();
			index[1]=it.y();
			iit.SetIndex(index);
		}
		// Calcul de la direction du gradient ...
		float length = it.length();
		gradient_sum[0] += length * iit.Value()[0];
		gradient_sum[1] += length * iit.Value()[1];
	}
	*/

	Segment_2 s(gridIn, gridOut);
	for (CGAL::Segment_2_iterator<Segment_2> it(s); !it.end(); ++it)
	{
		CovariantImageType::IndexType index;
		index[0] = it.x();
		index[1] = it.y();
		// Calcul de la direction du gradient ...
		const CovariantPixelType &pixel = m_gradients->GetPixel(index);
		float length = it.length();
		gradient_sum[0] += length * pixel[0];
		gradient_sum[1] += length * pixel[1];
	}

	Vector_2 arete(gridIn, gridOut);
	Vector_2 normale = arete.perpendicular(CGAL::POSITIVE);
	Vector_2 sum(gradient_sum[0], gradient_sum[1]);
	res = CGAL::to_double(normale * sum);
	return res;
}
