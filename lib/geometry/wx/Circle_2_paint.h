#ifndef CIRCLE_2_PAINT_H
#define CIRCLE_2_PAINT_H

#include <GilViewer/layers/layer.hpp>
#include "geometry/geometry.hpp"
#include "geometry/Circle_2.h"

template<typename K> void paint(
        layer::ptrLayerType& l,
	const std::string& s,
	const geometry::Circle_2<K>& c)
{
  double x = geometry::to_double(c.center().x());
  double y = geometry::to_double(c.center().y());
  double r = geometry::to_double(geometry::radius(c));
  l->add_circle(x,y,r);
  l->add_text(x, y, s , wxColour(255,0,0) );
}

#endif // CIRCLE_2_PAINT_H
