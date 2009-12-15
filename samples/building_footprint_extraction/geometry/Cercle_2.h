#ifndef CGAL_CERCLE_2_H
#define CGAL_CERCLE_2_H

#include <CGAL/basic.h>

CGAL_BEGIN_NAMESPACE

template<class R_> class Cercle_2
{
public:
	typedef R_ R;
	typedef typename R_::RT RT;
	typedef typename R_::FT FT;
	typedef typename R_::Point_2 Point_2;
	typedef typename R_::Vector_2 Vector_2;
	typedef typename R_::Line_2 Line_2;
	typedef Cercle_2 Self;

	Cercle_2()
	{}
	Cercle_2(const Point_2 &center, double radius) : m_center(center), m_radius(radius), m_squared_radius(radius*radius)
	{}

	inline Point_2 center() const
	{	return m_center;}
	inline FT radius() const
	{	return m_radius;}
	inline FT squared_radius() const
	{	return m_squared_radius;}

	inline FT area() const
	{	return M_PI*squared_radius();}
	inline FT perimeter() const
	{	return 2*M_PI*radius();}
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
		return area;
	}

private :
	Point_2 m_center;
	FT m_radius, m_squared_radius;
};

template < class R >
std::ostream &
operator<<(std::ostream &os, const Cercle_2<R> &c)
{
	switch(os.iword(IO::mode))
	{
		case IO::ASCII :
		return os << c.center() << ' ' << c.radius();
		case IO::BINARY :
		return os << c.center() << c.radius();
		default:
		return os << "Cercle_2(" << c.center() << ", " << c.radius() << ")";
	}
}

template < class R >
std::istream &
operator>>(std::istream &is, Cercle_2<R> &b)
{
	typename R::Point_2 p;
	typename R::FT r;

	is >> p >> r;

	if (is)
	b = Cercle_2<R>(p, r);
	return is;
}

CGAL_END_NAMESPACE

#endif //#ifndef CGAL_CERCLE_2_H
