#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "geometry/geometry.hpp"
#include "geometry/Rectangle_2.h"
#include "geometry/Circle_2.h"

typedef geometry::Simple_cartesian<double> K;
typedef geometry::Iso_rectangle_2_traits<K>::type Iso_rectangle_2;
typedef geometry::Rectangle_2<K> Rectangle_2;
typedef geometry::Circle_2<K> Circle_2;

#include "geometry/Rectangle_2_coordinates.hpp"
#include "geometry/Circle_2_coordinates.hpp"
#endif // GEOMETRY_H

