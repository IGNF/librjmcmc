#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include "geometry/geometry.hpp"
#include "geometry/Rectangle_2.hpp"
#include "geometry/Circle_2.hpp"

typedef geometry::Simple_cartesian<double> K;
typedef geometry::Iso_rectangle_2_traits<K>::type Iso_rectangle_2;
typedef geometry::Rectangle_2<K> Rectangle_2;
typedef geometry::Circle_2<K> Circle_2;

#include "geometry/coordinates/all.hpp"
#endif // GEOMETRY_HPP

