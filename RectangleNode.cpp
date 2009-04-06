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

	boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > dievec( GetRandom(), 
			boost::uniform_real<>(BuildingsDetectorParametersSingleton::Instance()->RectangleMinimalSize(), BuildingsDetectorParametersSingleton::Instance()->RectangleMaximalSize()));

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
