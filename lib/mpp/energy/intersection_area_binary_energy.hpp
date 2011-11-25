#ifndef INTERSECTION_AREA_BINARY_ENERGY_HPP_
#define INTERSECTION_AREA_BINARY_ENERGY_HPP_

#include "geometry/intersection.h"

template<typename Value = double>
class intersection_area_binary_energy
{
public:
    typedef Value result_type;

    template<typename T, typename U>
    inline result_type operator()(const T &t, const U &u) const
    {
        return std::abs(geometry::to_double(geometry::intersection_area(t,u)));
    }

    // optimized version of the expression "operator()(t,u)!=0"
    template<typename T, typename U>
    inline bool interact(const T &t, const U &u) const {
        return geometry::do_intersect(t, u);
    }

};

#endif /*INTERSECTION_AREA_BINARY_ENERGY_HPP_*/

