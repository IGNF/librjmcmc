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

#ifndef GEOMETRY_CIRCLE_2_IMAGE_ERROR_HPP
#define GEOMETRY_CIRCLE_2_IMAGE_ERROR_HPP

#include "geometry/Circle_2.hpp"


template<typename V, typename F>
inline typename F::result_type apply_operation(const V& v, const F &f) {
    return f(v);
}

template<typename T, typename F>
inline typename F::result_type apply_operation(const boost::gil::any_image_view<T>& v, const F &f) {
    return boost::gil::apply_operation(v, f);
}

struct Circle_2_image_error_functor
{
    double r, x, y;
    template<typename K>
    Circle_2_image_error_functor(const geometry::Circle_2<K> &c, int x0, int y0) {
        r = geometry::radius(c);
        x = c.center().x()-x0;
        y = c.center().y()-y0;
    }

    typedef double result_type;
    template <typename T> result_type operator()(const T& v) const {

        // TODO: optimize with locators
        double res = 0;
        int i0 = std::max(0,(int) (x-r));
        int i1 = std::min((int) (v.width()-1),(int) (x+r+1));
        int j0 = std::max(0,(int) (y-r));
        int j1 = std::min((int) (v.height()-1),(int) (y+r+1));

        for ( int i = i0 ; i<=i1 ; ++i)
        {
            for ( int j = j0 ; j<=j1 ; ++j)
            {
                if ((x-i)*(x-i)+(y-j)*(y-j)>r*r) continue;
                double ndvi = v(i,j);
                res +=(140-ndvi) ;
            }
        }

        return res;
    }
};

template<typename OrientedView, typename K>
double image_error(const OrientedView& v, const geometry::Circle_2<K> &c) {
    Circle_2_image_error_functor f(c,v.x0(), v.y0());
    return apply_operation(v.view(), f);
}

#endif // GEOMETRY_CIRCLE_2_IMAGE_ERROR_HPP

