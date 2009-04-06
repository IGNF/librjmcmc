#ifndef CIRCLE_NODE_HPP
#define CIRCLE_NODE_HPP

#include "BBox.hpp"
#include "cgal_types.h"

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

#endif //#ifndef CIRCLE_NODE_HPP
