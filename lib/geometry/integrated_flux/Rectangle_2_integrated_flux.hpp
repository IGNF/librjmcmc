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

template<typename K, typename OrientedImage, typename Segment, typename Functor>
void integrated_flux(const OrientedImage& v, const Segment& s0, Functor& f)
{
    typedef typename OrientedImage::view_t view_t;
    typedef typename view_t::xy_locator xy_locator;

    view_t view = v.view();
    int x0 = v.x0();
    int y0 = v.y0();

    int x1 = x0+view.width();
    int y1 = y0+view.height();
    Segment s(s0);
    typename K::Iso_rectangle_2 bbox(x0,y0,x1,y1);
    if(!clip(bbox,s)) return;

    geometry::Segment_2_iterator<K> it(s);

    xy_locator loc = view.xy_at(
            (typename xy_locator::x_coord_t) (it.x()-x0),
            (typename xy_locator::y_coord_t) (it.y()-y0)
            );

    boost::gil::point2<std::ptrdiff_t> movement[2] = {
        boost::gil::point2<std::ptrdiff_t> (it.step(0), 0),
        boost::gil::point2<std::ptrdiff_t> (0, it.step(1))
    };
    typename K::Vector_2 edge(s.target()-s.source());
    typename K::Vector_2 normal(edge.y(),-edge.x());
    for (; !it.end() ; ++it)
    {
        if(it.x()>=x0 && it.x()<x1 && it.y()>=y0 && it.y()<y1) {
            typename K::RT length = it.length();
            f(length,normal,loc);
        }
        loc += movement[it.axis()];
    }
}


class Flux_functor
{
public:
    template<typename RT, typename Vector, typename Locator>
    void operator()(RT length, const Vector& normal, const Locator& loc)
    {
        Vector g(boost::gil::at_c<0> (*loc), boost::gil::at_c<1> (*loc));
        double dot = length*geometry::to_double(normal * g);
        // res += std::abs(dot)-2.;
        m_value += std::max(0.,dot);
    }
    Flux_functor() : m_value(0) {}
    double value() const { return m_value; }
private:
    double m_value;
};


template<typename OrientedImage, typename K>
double integrated_flux(const OrientedImage& view, const geometry::Rectangle_2<K>& r)
{
    Flux_functor f;
    integrated_flux<K>(view,r.segment(0),f);
    integrated_flux<K>(view,r.segment(1),f);
    integrated_flux<K>(view,r.segment(2),f);
    integrated_flux<K>(view,r.segment(3),f);
    return f.value();
}

#endif // GEOMETRY_RECTANGLE_2_INTEGRATED_FLUX_HPP
