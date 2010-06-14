// Author(s)     : Mathieu Bredif

#ifndef GEOMETRY_RECTANGLE_2_IMAGE_ERROR_H
#define GEOMETRY_RECTANGLE_2_IMAGE_ERROR_H

#include "Rectangle_2.h"

template<typename OrientedView, typename K>
double image_error(const OrientedView&, const geometry::Rectangle_2<K> &) {
	return 0;
}

#endif // GEOMETRY_RECTANGLE_2_IMAGE_ERROR_H
