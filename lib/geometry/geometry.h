#ifndef CGAL_TYPES_H
#define CGAL_TYPES_H

#include "geometry/cgalite.hpp"

typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_2 Point_2;
typedef K::Vector_2 Vector_2;
typedef K::Segment_2 Segment_2;
typedef K::FT FT;


#include "Rectangle_2_intersection.h"
#include "Circle_2.h"
#include "Circle_2_intersection.h"
#include "Circle_2_rectangle_2_intersection.h"

typedef CGAL::Iso_rectangle_2_traits<K>::type Iso_rectangle_2;
typedef CGAL::Rectangle_2<K> Rectangle_2;
typedef CGAL::Circle_2<K> Circle_2;

#endif //#ifndef CGAL_TYPES_H
