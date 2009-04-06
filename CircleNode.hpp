#include "BBox.hpp"
#include "cgal_types.h"


#ifdef WIN32
	const double M_PI = 4.0 * atan(1.0);
#endif // WIN32

class Cercle_2
{
public :
	Cercle_2() {}
	Cercle_2(const Point_2 &center, double radius) : m_center(center), m_radius(radius), m_squared_radius(radius*radius)
	{}

	inline Point_2 center() const { return m_center; }
	inline double radius() const { return m_radius; }
	inline double squared_radius() const { return m_squared_radius; }

	inline double area() const {return M_PI*squared_radius(); }
	inline bool do_intersect(const Cercle_2 &c) const 
	{
		Vector_2 diff = c.center() - center();
		double delta2 = diff.squared_length();
		return (delta2 < c.squared_radius() + squared_radius() + 2 * c.radius()*radius()); 
	}
	
	// Tout vient de :
	// Weisstein, Eric W. "Circle-Circle Intersection." From MathWorld--A Wolfram Web Resource. 
	// http://mathworld.wolfram.com/Circle-CircleIntersection.html 
	inline double intersection_area(const Cercle_2 &C) const 
	{
		Vector_2 diff = C.center() - center();
		double d2 = diff.squared_length();
		double d = ::sqrt(d2);
		double r = radius(), r2 = squared_radius(), R = C.radius(), R2 = C.squared_radius();
		if (d < std::abs(r-R))
		{
			double rmin = std::min(r,R);
			return M_PI * rmin * rmin;
		}
		double area = r2*acos((d2+r2-R2)/(2*d*r));
		area += R2* acos((d2+R2-r2)/(2*d*R));
		area -= 0.5 * ::sqrt((-d+r+R)*(d+r-R)*(d-r+R)*(d+r+R));
		return area;
	}
private :
	Point_2 m_center;
	double m_radius, m_squared_radius;
};

class CircleNode
{
public :
	CircleNode() : m_weight(0) {;}
	CircleNode(const CircleNode &circle) : m_geometry(circle.m_geometry), m_weight(circle.m_weight) {}

	void RandomModify(const BBox &box);
	void RandomInit(const BBox &box);

	const double Weight() const { return m_weight; }
	void Weight( double w ) { m_weight = w; }

	const Cercle_2 & Geometry() const { return m_geometry; }

private :
	bool IsValid(const BBox &box) const;

	Cercle_2 m_geometry;
	double m_weight;
};

std::ostream & operator<<(std::ostream &o, const CircleNode &node);

class CirclePriorEnergyPolicy
{
public :
	inline CirclePriorEnergyPolicy():m_coefSurface(100.) {;}

	inline bool AreNeighbor(const CircleNode &n1, const CircleNode &n2)
	{
		return n1.Geometry().do_intersect(n2.Geometry());
	}
	
	inline double ComputePriorEnergy(const CircleNode &n1, const CircleNode &n2)
	{
		return m_coefSurface * n1.Geometry().intersection_area(n2.Geometry());
	}

private :
	double m_coefSurface;
};

class SurfaceCircleDataEnergyPolicy
{
public :
	double ComputeDataEnergy(const CircleNode &n) const
	{
			return -::log(1. + std::abs(CGAL::to_double(n.Geometry().area())));
	}
};

