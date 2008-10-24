#ifndef SIMPLE2DNODE_HPP_
#define SIMPLE2DNODE_HPP_

#include <cmath>

#include "Random.hpp"
#include "BBox.hpp"

class Simple2DNode
{
public:
	Simple2DNode() : m_weight(0){;}
	~Simple2DNode() {;}

	float X() const {return m_x;}
	float Y() const {return m_y;}
	void X(float x) {m_x = x;}
	void Y(float y) {m_y = y;}

	const double Weight() const { return m_weight; }
	void Weight( double w ) { m_weight = w; }

	void RandomModify(const BBox &box)
	{
		boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > die(GetRandom(), boost::uniform_real<>(-1, 1));
		float oldx = X();
		float oldy = Y();
		X( X() + die());
		Y( Y() + die());
		if (!box.IsInside(X(), Y()))
		{
			X(oldx);
			Y(oldy);
			RandomModify(box);
		}
	}

	void RandomInit(const BBox &box)
	{
		boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > diex(GetRandom(), boost::uniform_real<>(box.Xmin(), box.Xmax()));
		boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > diey(GetRandom(), boost::uniform_real<>(box.Ymin(), box.Ymax()));

		X( diex() );
		Y( diey() );
	}

private:
	float m_x;
	float m_y;
	double m_weight;
};

inline std::ostream & operator<<(std::ostream &o, const Simple2DNode &node)
{
	o << node.X() << "\t" << node.Y();
	return o;
}

#endif /*SIMPLE2DNODE_HPP_*/
