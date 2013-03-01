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

#ifndef RECTANGLE_SCALED_EDGE_KERNEL_HPP
#define RECTANGLE_SCALED_EDGE_KERNEL_HPP

#include "geometry/Rectangle_2.hpp"
#include "rjmcmc/random.hpp"
#include <boost/random/variate_generator.hpp>

namespace geometry {

    template<typename T, typename U = typename T::FT>
    struct rectangle_edge_translation_transform
    {
        enum { dimension = 6 };
        typedef T value_type;
        typedef U FT;

        template<typename Iterator>
        inline double abs_jacobian(Iterator it) const { return exp(4.0*(it[5]-0.5)); }

        template<typename IteratorIn,typename IteratorOut>
        inline double apply  (IteratorIn in, IteratorOut out) const {
            FT x = *in++;
            FT y = *in++;
            FT u = *in++;
            FT v = *in++;
            FT r = *in++;
            FT s = *in++;
            FT f = exp(4.0*(s-0.5));
            FT g = 1-f;
            //   res = Rectangle_2(c+m*(1-f), n,f*r);
            *out++ = x-g*r*v;
            *out++ = y+g*r*u;
            *out++ = u;
            *out++ = v;
            *out++ = f*r;
            *out++ = 1.0-s;
            return f;
        }

        template<typename IteratorIn,typename IteratorOut>
        inline double inverse(IteratorIn in, IteratorOut out) const { return apply(in,out); }
    };

}


#endif // RECTANGLE_SCALED_EDGE_KERNEL_HPP
