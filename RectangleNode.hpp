#ifndef RECTANGLENODE_HPP_
#define RECTANGLENODE_HPP_

#include "BBox.hpp"
#include "cgal_types.h"

class RectangleNode
{
public :
	RectangleNode() : m_weight(0) {;}
	RectangleNode(const RectangleNode &rec) : m_geometry(rec.m_geometry), m_weight(rec.m_weight) {}

	void RandomInit(const BBox &box);
	void RandomModify(const BBox &box);

	const double Weight() const { return m_weight; }
	void Weight( double w ) { m_weight = w; }

	const CRectangle_2 &Geometry() const { return m_geometry; }

private :
	bool IsValid(const BBox &box) const;

	CRectangle_2 m_geometry;
	double m_weight;
};

std::ostream & operator<<(std::ostream &o, const RectangleNode &node);

class RectanglePriorEnergyPolicy
{
public :
	RectanglePriorEnergyPolicy();

	inline bool AreNeighbor(const RectangleNode &n1, const RectangleNode &n2)
	{
		return n1.Geometry().do_intersect(n2.Geometry());
	}
	
	inline double ComputePriorEnergy(const RectangleNode &n1, const RectangleNode &n2)
	{
		return m_coefSurface * std::abs(CGAL::to_double(n1.Geometry().intersection_area(n2.Geometry())));
	}

private :
	double m_coefSurface;
};

class RectanglePointsPriorEnergyPolicy
{
public :
	RectanglePointsPriorEnergyPolicy();
	bool AreNeighbor(const RectangleNode &n1, const RectangleNode &n2);
	double ComputePriorEnergy(const RectangleNode &n1, const RectangleNode &n2);

private :
	inline double GetDistMax() const { return m_distMax; }
	inline double GetDistMax2() const { return m_distMax2; }
	double m_coefSurface;
	double m_coefDistance;
	double m_distMax, m_distMax2;
};

class SurfaceRectangleDataEnergyPolicy
{
public :
	double ComputeDataEnergy(const RectangleNode &n) const;
};

#endif /*RECTANGLENODE_HPP_*/
