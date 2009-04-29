#ifndef GEOMETRIC_NODE_HPP
#define GEOMETRIC_NODE_HPP

#include "BBox.hpp"
#include "Random.hpp"
#include "cgal_types.h"
#include "BuildingsDetectorParameters.hpp"

inline void RandomModify(const BBox &box, Cercle_2 &c)
{
	boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > die(GetRandom(), boost::uniform_real<>(-10, 10));
	c = Cercle_2(Point_2(c.center().x() + die(), c.center().y() + die()), c.radius() + die());
}

inline void RandomModify(const BBox &box, Rectangle_2 &r)
{
	std::vector<Point_2> pts;
	pts.reserve(3);
	pts.push_back( r.point(0) );
	pts.push_back( r.point(1) );
	pts.push_back( r.point(2) );

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

	r = Rectangle_2(pts[0], pts[1], pts[2]);
}

inline void RandomInit(const BBox &box, Cercle_2 &c)
{
	boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > diex(GetRandom(), 
		boost::uniform_real<>(box.Min()[0], box.Max()[0]));
	boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > diey(GetRandom(), 
		boost::uniform_real<>(box.Min()[1], box.Max()[1]));

	Point_2 p(diex(), diey());

	boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > dieradius( GetRandom(), 
			boost::uniform_real<>(BuildingsDetectorParametersSingleton::Instance()->MinimalSize(), BuildingsDetectorParametersSingleton::Instance()->MaximalSize()));

	c = Cercle_2(p, dieradius());
}

inline void RandomInit(const BBox &box, Rectangle_2 &rect)
{
	boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > diex(GetRandom(), 
		boost::uniform_real<>(box.Min()[0], box.Max()[0]));
	boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > diey(GetRandom(), 
		boost::uniform_real<>(box.Min()[1], box.Max()[1]));

	boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > dievec( GetRandom(), 
			boost::uniform_real<>(BuildingsDetectorParametersSingleton::Instance()->MinimalSize(), BuildingsDetectorParametersSingleton::Instance()->MaximalSize()));

	Point_2 p(diex(), diey());
	Point_2 q(p.x() + dievec(), p.y() + dievec());
	Point_2 r(q.x() + dievec(), q.y() + dievec());
	while (p == q)
		q = Point_2(p.x() + dievec(), p.y() + dievec());

	rect = Rectangle_2(p, q, r);
}

inline bool IsValid(const BBox &box, const Rectangle_2 &r)
{
	Vector_2 n = r.normal();
	float anx = std::abs(n.x());
	float any = std::abs(n.y());
	float dx = anx + r.ratio()*any;
	float dy = any + r.ratio()*anx;
	Point_2 c = r.center();
	if ((c.x()-dx < box.Min()[0]) || (c.y()-dy < box.Min()[1]) || (c.x()+dx > box.Max()[0]) || (c.y()+dy > box.Max()[1]))
		return false;

	float length0 = r.normal_length();
	float length1 = r.ratio()*length0;
	if (length0 > length1)
		std::swap(length0, length1);

	float minSize = (float)BuildingsDetectorParametersSingleton::Instance()->MinimalSize();
	if ( length0 < minSize)
		return false;

	float maxSize = (float)BuildingsDetectorParametersSingleton::Instance()->MaximalSize();
	if (length1 > maxSize)
		return false;

	if ((r.ratio() > BuildingsDetectorParametersSingleton::Instance()->RectangleMaximalRatio()) ||
		(1./r.ratio() > BuildingsDetectorParametersSingleton::Instance()->RectangleMaximalRatio()))
		return false;

	return true;
}

inline bool IsValid(const BBox &box, const Cercle_2 &ce)
{
	float minSize = (float)BuildingsDetectorParametersSingleton::Instance()->MinimalSize();
	if ( ce.radius() < minSize)
		return false;
	float maxSize = (float)BuildingsDetectorParametersSingleton::Instance()->MaximalSize();
	if ( ce.radius() > maxSize)
		return false;

	Point_2 c = ce.center();
	if ((c.x()-ce.radius() < box.Min()[0]) || (c.y()-ce.radius() < box.Min()[1]) || (c.x()+ce.radius() > box.Max()[0]) || (c.y()+ce.radius() > box.Max()[1]))
		return false;

	return true;
}

template<class NodeGeometry>
class GeometricNode
{
public :
	GeometricNode() : m_weight(0) {;}
	GeometricNode(const GeometricNode &n) : m_geometry(n.m_geometry), m_weight(n.m_weight)
	{}

	void RandomModify(const BBox &box)
	{
		NodeGeometry old = m_geometry;
		::RandomModify(box, m_geometry);

		if (!IsValid(box, m_geometry))
		{
			m_geometry = old;
			RandomModify(box);
		}
	}

	void RandomInit(const BBox &box)
	{
		::RandomInit(box, m_geometry);

		if (!IsValid(box, m_geometry))
			RandomInit(box);
	}

	const double Weight() const { return m_weight; }
	void Weight( double w ) { m_weight = w; }

	const NodeGeometry & Geometry() const { return m_geometry; }

private :
	NodeGeometry m_geometry;
	double m_weight;
};

class IntersectionPriorEnergyPolicy
{
	double m_coefSurface;
public :
	inline IntersectionPriorEnergyPolicy(): m_coefSurface(BuildingsDetectorParametersSingleton::Instance()->IntersectionSurfacePonderation()) {;}

	template<class NodeGeometry>
	inline double ComputePriorEnergy(const GeometricNode<NodeGeometry> & n1, const GeometricNode<NodeGeometry> & n2) const
	{
		if (n1.Geometry().do_intersect(n2.Geometry()))
		{
			return m_coefSurface * n1.Geometry().intersection_area(n2.Geometry());
		}
		return 0.;
	}
};

class SurfaceDataEnergyPolicy
{
public :
	template<class NodeGeometry>
	double ComputeDataEnergy(const GeometricNode<NodeGeometry> & n) const
	{
			return -::log(1. + std::abs(CGAL::to_double(n.Geometry().area())));
	}
};

#endif //#ifndef GEOMETRIC_NODE_HPP
