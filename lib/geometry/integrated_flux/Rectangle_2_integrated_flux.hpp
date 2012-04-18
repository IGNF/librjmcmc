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

#ifndef GEOMETRY_RECTANGLE_2_INTEGRATED_FLUX_HPP
#define GEOMETRY_RECTANGLE_2_INTEGRATED_FLUX_HPP

#include "geometry/Rectangle_2.hpp"
#include "geometry/Segment_2_iterator.hpp"
#include "geometry/Iso_rectangle_2_Segment_2_clip.hpp"
#include <boost/gil/image.hpp>
#include <boost/gil/extension/matis/float_images.hpp>

template<typename K, typename OrientedView, typename Segment>
double integrated_flux(const OrientedView& v, const Segment& s0)
{
	typedef typename OrientedView::view_t view_t;
	typedef typename view_t::xy_locator xy_locator;

	view_t view = v.view();
	int x0 = v.x0();
	int y0 = v.y0();

//    typedef view_t::pixel_t pixel_t;
    typedef boost::gil::dev2n32F_pixel_t pixel_t;

	int x1 = x0+view.width();
	int y1 = y0+view.height();
	Segment s(s0);
	typename K::Iso_rectangle_2 bbox(x0,y0,x1,y1);
	if(!clip(bbox,s)) return 0;
	geometry::Segment_2_iterator<K> it(s);

	xy_locator loc_grad = view.xy_at(
		(typename xy_locator::x_coord_t) (it.x()-x0),
		(typename xy_locator::y_coord_t) (it.y()-y0)
	);

	boost::gil::point2<std::ptrdiff_t> movement[2] = {
		boost::gil::point2<std::ptrdiff_t> (it.step(0), 0),
		boost::gil::point2<std::ptrdiff_t> (0, it.step(1))
	};
	float gradient_sum[2] =	{ 0., 0. };
	for (; !it.end() ; ++it)
	{
		if(it.x()>=x0 && it.x()<x1 && it.y()>=y0 && it.y()<y1) {
            typename K::RT length = it.length();
			const pixel_t& grad = *loc_grad;
			gradient_sum[0] += length * boost::gil::at_c<0> (grad);
			gradient_sum[1] += length * boost::gil::at_c<1> (grad);
		}
		loc_grad += movement[it.axis()];
	}

	typename K::Vector_2 arete(s.target()-s.source());
	typename K::Vector_2 normale = arete.perpendicular(geometry::NEGATIVE);
	typename K::Vector_2 sum(gradient_sum[0], gradient_sum[1]);
	return geometry::to_double(normale * sum);
}

template<typename OrientedView, typename K>
double integrated_flux(const OrientedView& view, const geometry::Rectangle_2<K>& r)
{
	return std::max(0.,integrated_flux<K>(view,r.segment(0)))
		 + std::max(0.,integrated_flux<K>(view,r.segment(1)))
		 + std::max(0.,integrated_flux<K>(view,r.segment(2)))
		 + std::max(0.,integrated_flux<K>(view,r.segment(3)));
}

/*
template<typename View, typename K> double inverted_integrated_flux(const View& view, int x0, int y0, const geometry::Rectangle_2<K>& r)
{
        return 	  std::max(0.,-integrated_flux(view,x0,y0,r.segment(0)))
                + std::max(0.,-integrated_flux(view,x0,y0,r.segment(1)))
                + std::max(0.,-integrated_flux(view,x0,y0,r.segment(2)))
                + std::max(0.,-integrated_flux(view,x0,y0,r.segment(3)));
}
*/

#endif // GEOMETRY_RECTANGLE_2_INTEGRATED_FLUX_HPP
