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

#ifndef CIRCLE_TRANSLATION_KERNEL_HPP
#define CIRCLE_TRANSLATION_KERNEL_HPP

#include "geometry/Circle_2.hpp"
#include "rjmcmc/random.hpp"

namespace geometry {

    template<typename T, typename U = typename T::FT>
    class circle_center_translation_transform
    {
    public:
        enum { dimension = 5 };
        typedef T value_type;
        typedef U FT;

        circle_center_translation_transform(FT d=10.) : m_d(d) {}
        template<typename Iterator>
        inline double abs_jacobian(Iterator it) const { return 1.; }

        template<typename IteratorIn,typename IteratorOut>
        inline double apply  (IteratorIn in, IteratorOut out) const {
            double res = abs_jacobian(in);
            FT x  = *in++;
            FT y  = *in++;
            FT r  = *in++;
            FT u = *in++;
            FT v = *in++;
            FT dx = m_d*(u-0.5);
            FT dy = m_d*(v-0.5);
            *out++ = x+dx;
            *out++ = y+dy;
            *out++ = r;
            *out++ = 1.-u;
            *out++ = 1.-v;
            return res;
        }

        template<typename IteratorIn,typename IteratorOut>
        inline double inverse(IteratorIn in, IteratorOut out) const { return apply(in,out); }

    private:
        FT m_d;
    };


    template<typename T, typename U = typename T::FT>
    class circle_edge_translation_transform
    {
    public:
        enum { dimension = 5 };
        typedef T value_type;
        typedef U FT;

        circle_edge_translation_transform(FT d=10.) : m_d(d) {}

        template<typename Iterator>
        inline double abs_jacobian(Iterator it) const { return 1.; }

        template<typename IteratorIn,typename IteratorOut>
        inline double apply  (IteratorIn in, IteratorOut out) const {
            double res = abs_jacobian(in);
            FT x = *in++;
            FT y = *in++;
            FT r = *in++;
            FT u = *in++;
            FT v = *in++;
            FT dx = m_d*(u-0.5);
            FT dy = m_d*(v-0.5);
            *out++ = x+dx;
            *out++ = y+dy;
            *out++ = r+sqrt(dx*dx+dy*dy); // forward is increasing the radius
            *out++ = 1.-u;
            *out++ = 1.-v;
            return res;
        }

        template<typename IteratorIn,typename IteratorOut>
        inline double inverse(IteratorIn in, IteratorOut out) const {
            double res = abs_jacobian(in);
            FT x = *in++;
            FT y = *in++;
            FT r = *in++;
            FT u = *in++;
            FT v = *in++;
            FT dx = m_d*(u-0.5);
            FT dy = m_d*(v-0.5);
            *out++ = x+dx;
            *out++ = y+dy;
            *out++ = r-sqrt(dx*dx+dy*dy); // backward is decreasing the radius
            *out++ = 1.-u;
            *out++ = 1.-v;
            return res;
        }
    private:
        FT m_d;
    };

    template<typename T, typename U = typename T::FT>
    class circle_radius_transform
    {
    public:
        enum { dimension = 4 };
        typedef T value_type;
        typedef U FT;

        circle_radius_transform(FT d=10.) : m_d(d) {}

        template<typename Iterator>
        inline double abs_jacobian(Iterator it) const { return 1.; }

        template<typename IteratorIn,typename IteratorOut>
        inline double apply  (IteratorIn in, IteratorOut out) const {
            double res = abs_jacobian(in);
            FT x = *in++;
            FT y = *in++;
            FT r = *in++;
            FT u = *in++;
            FT dr = m_d*(u-0.5);
            *out++ = x;
            *out++ = y;
            *out++ = r+dr; // forward is increasing the radius
            *out++ = 1.-u;
            return res;
        }

        template<typename IteratorIn,typename IteratorOut>
        inline double inverse(IteratorIn in, IteratorOut out) const {
            return apply(in,out);
        }
    private:
        FT m_d;
    };
}

#endif // CIRCLE_TRANSLATION_KERNEL_HPP
