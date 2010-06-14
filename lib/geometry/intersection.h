#ifndef INTERSECTION_H
#define INTERSECTION_H

#ifdef GEOMETRY_RECTANGLE_2_H
#include "Rectangle_2_intersection.h"
#endif // GEOMETRY_RECTANGLE_2_H

#ifdef GEOMETRY_CIRCLE_2_H
#include "Circle_2_intersection.h"
#endif // GEOMETRY_CIRCLE_2_H

#if defined(GEOMETRY_CIRCLE_2_H) && defined(GEOMETRY_RECTANGLE_2_H)
#include "Circle_2_rectangle_2_intersection.h"
#endif // GEOMETRY_CIRCLE_2_H && GEOMETRY_RECTANGLE_2_H

#endif // INTERSECTION_H
