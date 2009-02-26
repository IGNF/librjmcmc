#ifndef RECTANGLENODE_HPP_
#define RECTANGLENODE_HPP_

#include "Random.hpp"

#include "BBox.hpp"
#include "cgal_types.h"

#include <boost/shared_ptr.hpp>

class RectangleNode
{
public :
	RectangleNode() : m_weight(0) {;}
	RectangleNode(const RectangleNode &rec) : m_rect(rec.m_rect), m_weight(rec.m_weight) {}

	void RandomModify(const BBox &box);
	void RandomInit(const BBox &box);

	const double Weight() const { return m_weight; }
	void Weight( double w ) { m_weight = w; }

	const CRectangle_2 &Rect() const { return m_rect; }
	typedef CRectangle_2::Point_2 Point_2;

private :

	bool IsValid(const BBox &box) /*const*/;

	CRectangle_2 m_rect;
	double m_weight;
};

std::ostream & operator<<(std::ostream &o, const RectangleNode &node);

class RectanglePriorEnergyPolicy
{
public :
	RectanglePriorEnergyPolicy();
	bool AreNeighbor(const RectangleNode &n1, const RectangleNode &n2);
	double ComputePriorEnergy(const RectangleNode &n1, const RectangleNode &n2);

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

class GILPolicyImage;
class ImageGradientEnergyPolicy
{
public :

	ImageGradientEnergyPolicy();
	~ImageGradientEnergyPolicy();
	double ComputeDataEnergy(const RectangleNode &n) const;

private:
	boost::shared_ptr<GILPolicyImage> m_policy;
};

#endif /*RECTANGLENODE_HPP_*/
