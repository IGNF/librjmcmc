#ifndef GEOMETRY_CIRCLE_2_H
#define GEOMETRY_CIRCLE_2_H

#if USE_CGAL

#include <CGAL/Circle_2.h>

namespace geometry {
using namespace CGAL;

template<class K>
inline typename K::FT radius(const Circle_2<K>& c) {
	return CGAL::sqrt(c.squared_radius());
}

#else

namespace geometry {
/**
 * \ingroup GroupGeometry
 */
template<class K_> class Circle_2
{
public:
	typedef K_ K;
	typedef typename K::RT RT;
	typedef typename K::FT FT;
	typedef typename K::Point_2 Point_2;
	typedef typename K::Vector_2 Vector_2;
	typedef typename K::Line_2 Line_2;
	typedef Circle_2 Self;

	Circle_2()
	{}
	Circle_2(const Point_2 &center, double radius) : m_center(center), m_radius(radius), m_squared_radius(radius*radius)
	{}

	inline Point_2 center() const
	{	return m_center;}
	inline bool is_degenerate() const
	{	return m_radius==0;}
	inline FT radius() const
	{	return m_radius;}
	inline FT squared_radius() const
	{	return m_squared_radius;}

private :
	Point_2 m_center;
	FT m_radius, m_squared_radius;
};


template < class K >
std::ostream &
operator<<(std::ostream &os, const Circle_2<K> &c)
{
        return os << c.center() << ' ' << c.radius();
}

template < class K >
std::istream &
operator>>(std::istream &is, Circle_2<K> &b)
{
	typename K::Point_2 p;
	typename K::FT r;

	is >> p >> r;

	if (is)
	b = Circle_2<K>(p, r);
	return is;
}

template<class K>
typename K::FT radius(const Circle_2<K>& c) {
	return c.radius();
}

#endif // USE_CGAL

template<class K>
inline typename K::FT perimeter(const Circle_2<K>& c) {
	return 2*M_PI*radius(c);
}


template<class K>
inline typename K::FT area(const Circle_2<K>& c) {
	return M_PI*c.squared_radius();
}

}; // namespace geometry

#endif // GEOMETRY_CIRCLE_2_H
