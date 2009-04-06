// Copyright (c) 2002  Utrecht University (The Netherlands).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.3-branch/Polygon/demo/Polygon/cgal_types.h $
// $Id: cgal_types.h 37003 2007-03-10 16:55:12Z spion $
//
//
// Author(s)     : Radu Ursu
#ifndef CGAL_TYPES_H
#define CGAL_TYPES_H

#ifdef WIN32
#define CGAL_CFG_NO_NEXTAFTER
#undef min
#undef max
#endif //#ifdef WIN32


#include <CGAL/basic.h>
#include <CGAL/Simple_cartesian.h>
//#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
//#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
//#include <CGAL/point_generators_2.h>

#include "Rectangle_2.h"
#include "Cercle_2.h"

typedef CGAL::Simple_cartesian<float>              K;
//typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
//typedef CGAL::Exact_predicates_exact_constructions_kernel K;

typedef K::Point_2                               Point_2;
typedef K::Segment_2                             Segment_2;
typedef K::Vector_2                              Vector_2;
typedef K::Circle_2                              Circle_2;
typedef K::Iso_rectangle_2                       Iso_rectangle_2;
typedef K::Aff_transformation_2                  Aff_transformation_2;
typedef std::vector<Point_2>                     Container;
typedef CGAL::Polygon_2<K, Container>            CPolygon_2;
//typedef CGAL::Random_points_in_square_2<Point_2> Point_generator;
typedef CGAL::Rectangle_2<K>                     CRectangle_2;
typedef CGAL::Cercle_2<K>                        Cercle_2;

#ifdef WIN32 
#ifndef M_PI
	const double M_PI = 4.0 * atan(1.0);
#endif // #ifndef M_PI
#endif // WIN32
#endif //#ifndef CGAL_TYPES_H
