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

namespace geometry {

    template<unsigned int N>
    struct rectangle_edge_translation_transform
    {
        double m_rmin, m_rrange;

        rectangle_edge_translation_transform(double minratio, double maxratio)
            : m_rmin(minratio), m_rrange(maxratio-minratio) {}

        enum { dimension = 6 };

        template<typename Iterator>
        inline double abs_jacobian(Iterator it) const { return 1.; }

        template<typename IteratorIn,typename IteratorOut>
        inline double apply  (IteratorIn in, IteratorOut out) const {
            typedef typename std::iterator_traits<IteratorIn>::value_type FT;
            FT x = *in++;
            FT y = *in++;
            FT u = *in++;
            FT v = *in++;
            FT r = *in++;
            FT p = m_rmin+m_rrange*(*in++);

            if(N<2) {
                // maxima
                // abs(determinant(jacobian([x-v*(r-p)*e,y+u*(r-p)*e,u,v,p,r],[x,y,u,v,r,p]))) = 1;
                // ratsimp(sublis([x=x-v*(r-p)*e,y=y+u*(r-p)*e,u=u,v=v,p=r,r=p],[x-v*(r-p)*e,y+u*(r-p)*e,u,v,p,r])) = [x, y, u, v, r, p];
                FT d = r-p;
                FT dx  = -v*d;
                FT dy  =  u*d;
                switch(N)
                {
                case 0 : *out++ = x+dx; *out++ = y+dy; break;
                case 1 : *out++ = x-dx; *out++ = y-dy; break;
                }
                *out++ = u;
                *out++ = v;
                *out++ = p;
                *out++ = (r-m_rmin)/m_rrange;
            } else {
                // maxima
                // abs(determinant(jacobian([x-(u-p*r*u)*e,y-(v-p*r*v)*e,p*r*u,p*r*v,1/p,1/r],[x,y,u,v,r,p]))) = 1;
                // ratsimp(sublis([x=x-(u-p*r*u)*e,y=y-(v-p*r*v)*e,u=p*r*u,v=p*r*v,r=1/p,p=1/r],[x-(u-p*r*u)*e,y-(v-p*r*v)*e,p*r*u,p*r*v,1/p,1/r])) = [x, y, u, v, r, p];

                FT pr  = p*r;
                FT pru = pr*u;
                FT prv = pr*v;
                FT dx  = u-pru;
                FT dy  = v-prv;
                switch(N)
                {
                case 2 : *out++ = x+dx; *out++ = y+dy; break;
                case 3 : *out++ = x-dx; *out++ = y-dy; break;
                }
                *out++ = pru;
                *out++ = prv;
                *out++ = 1./p;
                *out++ = (1.-r*m_rmin)/(r*m_rrange);
            }
            // maxima rot90: abs(determinant(jacobian([x,y,-r*v,r*u,p,1/r],[x,y,u,v,r,p]))) = 1;
            return 1;
        }

        template<typename IteratorIn,typename IteratorOut>
        inline double inverse(IteratorIn in, IteratorOut out) const { return apply(in,out); }
    };

}


#endif // RECTANGLE_SCALED_EDGE_KERNEL_HPP
