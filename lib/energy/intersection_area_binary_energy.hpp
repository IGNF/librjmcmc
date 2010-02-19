#ifndef INTERSECTION_AREA_BINARY_ENERGY_HPP_
#define INTERSECTION_AREA_BINARY_ENERGY_HPP_

#include "geometry/geometry.h"

class intersection_area_binary_energy
{
public:
	typedef double result_type;

	template<typename T, typename U>
	inline bool interact(const T &t, const U &u) const {
		return do_intersect_interior(t, u);
	}

	template<typename T, typename U>
	inline result_type operator()(const T &t, const U &u) const {
		return m_coefSurface * std::abs(CGAL::to_double(intersection_area(t,u)));
	}

	intersection_area_binary_energy(double d) : m_coefSurface(d) { }
private:
	double m_coefSurface;
};

#endif /*INTERSECTION_AREA_BINARY_ENERGY_HPP_*/
