#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include "rjmcmc/geometry/geometry.hpp"
#include "rjmcmc/geometry/Rectangle_2.hpp"
#include "rjmcmc/geometry/Circle_2.hpp"

typedef geometry::Simple_cartesian<double> K;
typedef geometry::Iso_rectangle_2_traits<K>::type Iso_rectangle_2;
typedef geometry::Rectangle_2<K> Rectangle_2;
typedef geometry::Circle_2<K> Circle_2;

#include "rjmcmc/geometry/coordinates/all.hpp"
#endif // GEOMETRY_HPP

