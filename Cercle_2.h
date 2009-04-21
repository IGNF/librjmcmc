#ifndef CGAL_CERCLE_2_H
#define CGAL_CERCLE_2_H

#include <CGAL/basic.h>

CGAL_BEGIN_NAMESPACE

template <class R_>
class Cercle_2
{
public:
	typedef  R_                                R;
	typedef typename R_::RT                    RT;
	typedef typename R_::FT                    FT;
	typedef typename R_::Point_2               Point_2;
	typedef typename R_::Vector_2              Vector_2;
	typedef typename R_::Line_2                Line_2;
	typedef Cercle_2                           Self;

	Cercle_2() {}
	Cercle_2(const Point_2 &center, double radius) : m_center(center), m_radius(radius), m_squared_radius(radius*radius)
	{}

	inline Point_2 center() const { return m_center; }
	inline FT radius() const { return m_radius; }
	inline FT squared_radius() const { return m_squared_radius; }

	inline FT area() const {return M_PI*squared_radius(); }
	inline bool do_intersect(const Cercle_2 &c) const 
	{
		Vector_2 diff = c.center() - center();
		FT delta2 = diff.squared_length();
		return (delta2 < c.squared_radius() + squared_radius() + 2 * c.radius()*radius()); 
	}
	
	// Tout vient de :
	// Weisstein, Eric W. "Circle-Circle Intersection." From MathWorld--A Wolfram Web Resource. 
	// http://mathworld.wolfram.com/Circle-CircleIntersection.html 
	inline FT intersection_area(const Cercle_2 &C) const 
	{
		Vector_2 diff = C.center() - center();
		FT d2 = diff.squared_length();
		FT d = ::sqrt(d2);
		FT r = radius(), r2 = squared_radius(), R = C.radius(), R2 = C.squared_radius();
		if (d < std::abs(r-R))
		{
			return M_PI * std::min(r2,R2);
		}
		FT area = r2*acos((d2+r2-R2)/(2.*d*r)) + R2* acos((d2+R2-r2)/(2.*d*R)) - 0.5 * ::sqrt((-d+r+R)*(d+r-R)*(d-r+R)*(d+r+R));
		/*
		if (area < 0)
		{
			std::cout << "area : " << area << std::endl;
			std::cout << center() << "\t" << radius() << std::endl;
			std::cout << C.center() << "\t" << C.radius() << std::endl;
			std::cout << d << "\t" << r+R  <<"\t" << d-(r+R) << std::endl;
			FT tdysq = R2 + r2 + 2 * R*r; 
			std::cout << d2 << "\t" << tdysq << "\t" << d2 - tdysq << std::endl;
			return -area;
		}
		*/
		return area;
	}

private :
	Point_2 m_center;
	FT m_radius, m_squared_radius;
};


CGAL_END_NAMESPACE

#endif //#ifndef CGAL_CERCLE_2_H
