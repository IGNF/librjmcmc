#ifndef CGAL_TYPES_H
#define CGAL_TYPES_H

#include "cgalite.hpp"

typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_2 Point_2;
typedef K::Vector_2 Vector_2;
typedef K::Segment_2 Segment_2;

#include "Rectangle_2.h"
#include "Rectangle_2_intersection.h"
typedef CGAL::Rectangle_2<K> Rectangle_2;

//#include "Cercle_2.h"
//typedef CGAL::Circle_2<K> Circle_2;

#include "Segment_2_iterator.h"
typedef CGAL::Segment_2_iterator<K> Segment_2_iterator;

#endif //#ifndef CGAL_TYPES_H
