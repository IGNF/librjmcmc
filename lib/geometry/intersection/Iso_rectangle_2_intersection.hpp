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

#ifndef GEOMETRY_ISO_RECTANGLE_2_INTERSECTION_HPP
#define GEOMETRY_ISO_RECTANGLE_2_INTERSECTION_HPP

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

#endif // GEOMETRY_ISO_RECTANGLE_2_INTERSECTION_HPP
