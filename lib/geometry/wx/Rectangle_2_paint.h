#ifndef RECTANGLE_2_PAINT_H
#define RECTANGLE_2_PAINT_H

#include <GilViewer/layers/Layer.hpp>
#include "geometry/geometry.hpp"
#include "geometry/Rectangle_2.h"

template<typename K> void paint(
        layer::ptrLayerType& l,
	const std::string& s,
	const geometry::Rectangle_2<K>& r)
{
  std::vector<double> vectx, vecty;
  for (unsigned int i=0; i<4;++i)
  {
    vectx.push_back(geometry::to_double(r[i].x()));
    vecty.push_back(geometry::to_double(r[i].y()));
  }
  double x = geometry::to_double(r.center().x());
  double y = geometry::to_double(r.center().y());
  l->add_polygon(vectx, vecty );
  l->add_text(x, y, s , wxColour(255,0,0) );
}

#endif // RECTANGLE_2_PAINT_H
