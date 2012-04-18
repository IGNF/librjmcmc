/***********************************************************************
This file is part of the librjmcmc project source files.

Copyright : Institut Geographique National (2008-2012)
Contributors : Mathieu Brédif, Olivier Tournaire, Didier Boldo
email : librjmcmc@ign.fr

This software is a generic C++ library for stochastic optimization.

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software. You can use,
modify and/or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty and the software's author, the holder of the
economic rights, and the successive licensors have only limited liability.

In this respect, the user's attention is drawn to the risks associated
with loading, using, modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean that it is complicated to manipulate, and that also
therefore means that it is reserved for developers and experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and, more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

***********************************************************************/

#ifndef GEOMETRY_CIRCLE_2_HPP
#define GEOMETRY_CIRCLE_2_HPP

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

        inline const Point_2& center() const
	{	return m_center;}
	inline bool is_degenerate() const
	{	return m_radius==0;}
        inline const FT& radius() const
	{	return m_radius;}
        inline const FT& squared_radius() const
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

#endif // GEOMETRY_CIRCLE_2_HPP
