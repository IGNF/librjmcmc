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

#ifndef RECTANGLE_ROTATION_SCALED_CORNER_KERNEL_HPP
#define RECTANGLE_ROTATION_SCALED_CORNER_KERNEL_HPP

#include <iterator>

namespace geometry {

    template<unsigned int N>
    struct rectangle_corner_translation_transform
    {
        enum { dimension = 7 };
        rectangle_corner_translation_transform(double d=10.) : m_d(d) {}

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
            FT s = *in++;
            FT t = *in;
            FT dx = m_d*(s-0.5);
            FT dy = m_d*(t-0.5);
            // maxima
            // abs(determinant(jacobian([x+e*dx-f*r*dy,y+e*dy+f*r*dx,u+dx,v+dy,r,1-s,1-t],[x,y,u,v,r,s,t]))) = 1;
            // ratsimp(sublis([x=x+e*d*(s-0.5)-f*r*d*(t-0.5),y=y+e*d*(t-0.5)+f*r*d*(s-0.5),u=u+d*(s-0.5),v=v+d*(t-0.5),r=r,s=1-s,t=1-t],[x+e*d*(s-0.5)-f*r*d*(t-0.5),y+e*d*(t-0.5)+f*r*d*(s-0.5),u+d*(s-0.5),v+d*(t-0.5),r,1-s,1-t])) = [x, y, u, v, r, s, t];
            switch(N)
            {
            case 0 : *out++ = x+dx-r*dy; *out++ = y+dy+r*dx; break;
            case 1 : *out++ = x+dx+r*dy; *out++ = y+dy-r*dx; break;
            case 2 : *out++ = x-dx-r*dy; *out++ = y-dy+r*dx; break;
            case 3 : *out++ = x-dx+r*dy; *out++ = y-dy-r*dx; break;
            }
            *out++ = u+dx;
            *out++ = v+dy;
            *out++ = r;
            *out++ = 1.-s;
            *out++ = 1.-t;
            return 1.;
        }

        template<typename IteratorIn,typename IteratorOut>
        inline double inverse(IteratorIn in, IteratorOut out) const { return apply(in,out); }
    private:
        double m_d;
    };

}
/*
  generated using boost::lambda ??

 _1+_6-_5*_7,
 _2+_7-_5*_6,
 _3+_6,
 _4+_7,
 _5,
-_6,
-_7

with some aliases :

 _x+_s-_r*_t,
 _y+_t+_r*_s,
 _u+_s,
 _v+_t,
 _r,
-_s,
-_t,

*/



#endif // RECTANGLE_ROTATION_SCALED_CORNER_KERNEL_HPP
