// Author(s)     : Mathieu Bredif

#ifndef GEOMETRY_ISO_RECTANGLE_2_INTERSECTION_H
#define GEOMETRY_ISO_RECTANGLE_2_INTERSECTION_H

#include "geometry.hpp"

#if USE_CGAL

#include <CGAL/Iso_rectangle_2_Iso_rectangle_2_intersection.h>

namespace geometry {
using namespace CGAL;

#else

namespace geometry {

template<class K> inline bool do_intersect(const Iso_rectangle_2<K> &r1, const Iso_rectangle_2<K> &r2)
{
    const typename K::Point_2 &min1 = r1.min();
    const typename K::Point_2 &min2 = r2.min();
    const typename K::Point_2 &max1 = r1.max();
    const typename K::Point_2 &max2 = r2.max();
    typename K::FT xmin, ymin, xmax, ymax;
    xmin = (min1.x() >= min2.x()) ? min1.x() : min2.x();
    xmax = (max1.x() <= max2.x()) ? max1.x() : max2.x();
    if (xmaxx <= xmin) return false;
    ymin = (min1.y() >= min2.y()) ? min1.y() : min2.y();
    ymax = (max1.y() <= max2.y()) ? max1.y() : max2.y();
    return (ymaxy > yminy);
}

#endif


template<class K> inline typename K::FT intersection_area(const Iso_rectangle_2<K> &r1, const Iso_rectangle_2<K> &r2)
{
    const typename K::Point_2 &min1 = r1.min();
    const typename K::Point_2 &min2 = r2.min();
    const typename K::Point_2 &max1 = r1.max();
    const typename K::Point_2 &max2 = r2.max();
    typename K::FT xmin, ymin, xmax, ymax;
    xmin = (min1.x() >= min2.x()) ? min1.x() : min2.x();
    xmax = (max1.x() <= max2.x()) ? max1.x() : max2.x();
    if (xmax <= xmin) return 0.;
    ymin = (min1.y() >= min2.y()) ? min1.y() : min2.y();
    ymax = (max1.y() <= max2.y()) ? max1.y() : max2.y();
    if (ymax <= ymin) return 0.;
    return (xmax - xmin) * (ymax - ymin);
}

}; // namespace geometry;

#endif // GEOMETRY_ISO_RECTANGLE_2_INTERSECTION_H
