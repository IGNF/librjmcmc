#ifndef CGAL_TYPES_H
#define CGAL_TYPES_H

#include "geometry/geometry.hpp"
#include "geometry/Rectangle_2.h"


typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_2 Point_2;
typedef K::Vector_2 Vector_2;
typedef K::Segment_2 Segment_2;
typedef K::FT FT;
typedef CGAL::Iso_rectangle_2_traits<K>::type Iso_rectangle_2;
typedef CGAL::Rectangle_2<K> Rectangle_2;


#include "geometry/Rectangle_2_intersection.h"



#endif //#ifndef CGAL_TYPES_H
