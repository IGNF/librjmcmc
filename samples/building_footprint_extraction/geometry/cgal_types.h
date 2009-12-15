#ifndef CGAL_TYPES_H
#define CGAL_TYPES_H

#ifdef _WINDOWS
#define CGAL_CFG_NO_NEXTAFTER
#undef min
#undef max
#endif //#ifdef _WINDOWS

#include <CGAL/basic.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polygon_2.h>

#include "Rectangle_2_all.h"
#include "Cercle_2.h"

typedef CGAL::Simple_cartesian<double> K;

typedef K::Point_2 Point_2;
typedef K::Segment_2 Segment_2;
typedef K::Vector_2 Vector_2;
typedef K::Circle_2 Circle_2;
typedef K::Iso_rectangle_2 Iso_rectangle_2;
typedef K::Aff_transformation_2 Aff_transformation_2;
typedef CGAL::Rectangle_2<K> Rectangle_2;
typedef CGAL::Cercle_2<K> Cercle_2;

#ifdef _WINDOWS
#	ifndef M_PI
const double M_PI = 4.0 * atan(1.0);
#	endif // #ifndef M_PI
#endif // _WINDOWS
#endif //#ifndef CGAL_TYPES_H
