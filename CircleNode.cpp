#include "stdafx.h"

#include "Random.hpp"
#include "CircleNode.hpp"

#include "BuildingsDetectorParameters.hpp"


bool CircleNode::IsValid(const BBox &box) const
{
	Point_2 c = m_geometry.center();
	double r = m_geometry.radius();

	// TODO : Mettre un CircleMinimalSize
	float minSize = (float)BuildingsDetectorParametersSingleton::Instance()->RectangleMinimalSize();
	if ( r < minSize)
		return false;

	if ((c.x()-r < box.Min()[0]) || (c.y()-r < box.Min()[1]) || (c.x()+r > box.Max()[0]) || (c.y()+r > box.Max()[1]))
		return false;


	return true;
}

void CircleNode::RandomInit(const BBox &box)
{
	boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > diex(GetRandom(), 
		boost::uniform_real<>(box.Min()[0], box.Max()[0]));
	boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > diey(GetRandom(), 
		boost::uniform_real<>(box.Min()[1], box.Max()[1]));

	Point_2 p(diex(), diey());

	boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > dieradius( GetRandom(), 
			boost::uniform_real<>(BuildingsDetectorParametersSingleton::Instance()->RectangleMinimalSize(), BuildingsDetectorParametersSingleton::Instance()->RectangleMaximalSize()));

	m_geometry = Cercle_2(p, dieradius());

	if (!IsValid(box))
		RandomInit(box);
}

void CircleNode::RandomModify(const BBox &box)
{
	Cercle_2 oldCircle = Geometry();

	boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > die(GetRandom(), boost::uniform_real<>(-10, 10));
	Point_2 center = Point_2(m_geometry.center().x() + die(), m_geometry.center().y() + die());
	double r = m_geometry.radius() + die();

	m_geometry = Cercle_2(center, r);

	if (!IsValid(box))
	{
		m_geometry = oldCircle;
		RandomModify(box);
	}
}

std::ostream & operator<<(std::ostream &o, const CircleNode &node)
{
	o << node.Geometry().center().x()<<";" << node.Geometry().center().y()<<"\t"<<node.Geometry().radius()<<"\t";
	return o;
}

