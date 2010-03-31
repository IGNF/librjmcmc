#ifndef CIRCLE_2_PAINT_H
#define CIRCLE_2_PAINT_H

#include <GilViewer/layers/Layer.hpp>
#include "geometry/geometry.hpp"
#include "geometry/Circle_2.h"

template<typename K> void paint(
	Layer::ptrLayerType& l,
	const std::string& s,
	const geometry::Circle_2<K>& c)
{
  double x = geometry::to_double(c.center().x());
  double y = geometry::to_double(c.center().y());
  double r = geometry::to_double(geometry::radius(c));
  l->AddCircle(x,y,r);
  l->AddText(x, y, s , wxColour(255,0,0) );
}

#endif // CIRCLE_2_PAINT_H
