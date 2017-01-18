/***********************************************************************
This file is part of the librjmcmc project source files.

Copyright : Institut Geographique National (2008-2012)
Contributors : Mathieu Brédif, Olivier Tournaire, Didier Boldo
email : librjmcmc@ign.fr

This software is a generic C++ library for stochastic optimization.

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software. You can use,
modify and/or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty and the software's author, the holder of the
economic rights, and the successive licensors have only limited liability.

In this respect, the user's attention is drawn to the risks associated
with loading, using, modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean that it is complicated to manipulate, and that also
therefore means that it is reserved for developers and experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and, more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

***********************************************************************/

#ifndef CIRCLE_2_PAINT_HPP
#define CIRCLE_2_PAINT_HPP

#include <GilViewer/layers/layer.hpp>
#include "rjmcmc/geometry/geometry.hpp"
#include "rjmcmc/geometry/Circle_2.hpp"

template<typename K, typename LayerPainter> void paint(
        layer::ptrLayerType& l,
	const std::string& s,
    const geometry::Circle_2<K>& c,
    LayerPainter * const)
{
  double x = geometry::to_double(c.center().x());
  double y = geometry::to_double(c.center().y());
  double r = geometry::to_double(geometry::radius(c));
  l->add_circle(x,y,r);
  l->add_text(x, y, s , wxColour(255,0,0) );
}

#endif // CIRCLE_2_PAINT_HPP
