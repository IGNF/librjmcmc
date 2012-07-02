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

#ifndef RECTANGLE_SPLIT_MERGE_KERNEL_HPP
#define RECTANGLE_SPLIT_MERGE_KERNEL_HPP

#include "geometry/Rectangle_2.hpp"

namespace geometry {


    // size = 10

    template<typename K>
    class rectangle_split_kernel
    {
    private:
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_real<> > double_die_type;
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_smallint<> > int_die_type;
        mutable double_die_type m_dief;
        mutable int_die_type   m_die4;
    public:

        rectangle_split_kernel() :
                m_dief(rjmcmc::random(), boost::uniform_real<>(0,1)),
                m_die4(rjmcmc::random(), boost::uniform_smallint<>(0,3)) {}

        typedef double result_type;
        typedef geometry::Rectangle_2<K> input_type;
        typedef std::pair<geometry::Rectangle_2<K>,geometry::Rectangle_2<K> > output_type;

        result_type operator()(const input_type &in, output_type &out) const
        {
            int i = m_die4();
            double f = m_dief();
            double g = m_dief()*(1-f);
            out.first  = in.scaled_edge(i  ,f);
            out.second = in.scaled_edge(i+2,g);
            return 1.; // TODO
        }
    };

    template<typename K>
    class rectangle_merge_kernel
    {
    private:
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_smallint<> > int_die_type;
        mutable int_die_type   m_die4;
    public:

        rectangle_merge_kernel() :
                m_die4(rjmcmc::random(), boost::uniform_smallint<>(0,3)) {}

        typedef double result_type;
        typedef std::pair<geometry::Rectangle_2<K>,geometry::Rectangle_2<K> > input_type;
        typedef geometry::Rectangle_2<K> output_type;

        result_type operator()(const input_type &in, output_type &out) const
        {
            out = in.first.rotate(m_die4()).merge(in.second.rotate(m_die4()));
            return 1.; // TODO
        }
    };
}



/*
struct rectangle_split_transform
{
    enum { dimension = 10 };

    template<typename Iterator>
    inline double abs_jacobian(Iterator it) const { return 1.; }

    template<typename IteratorIn,typename IteratorOut>
    inline double apply  (IteratorIn in, IteratorOut out) const {
        double res = abs_jacobian(in);
        typedef typename K::FT FT;
        FT x0 = *in++;
        FT y0 = *in++;
        FT u0 = *in++;
        FT v0 = *in++;
        FT r0 = *in++;
        FT x1 = *in++;
        FT y1 = *in++;
        FT u1 = *in++;
        FT v1 = *in++;
        FT r1 = *in++;
        //   res = Rectangle_2(c+v+u, n+v,r);
        *out++ = x+s-r*t;
        *out++ = y+t+r*s;
        *out++ = u+s;
        *out++ = v+t;
        *out++ = r;
        *out++ =-s;
        *out++ =-t;
        return res;
    }

    template<typename IteratorIn,typename IteratorOut>
    inline double inverse(IteratorIn in, IteratorOut out) const {
        typedef typename K::FT FT;
        FT x = *in++;
        FT y = *in++;
        FT u = *in++;
        FT v = *in++;
        FT r = *in++;
        FT s = *in++;
        FT c0 = *in++;
        FT c1 = *in++;
        FT s = *in++;
        FT s = *in++;
        //   res = Rectangle_2(c+m*(1-f), n,f*r);
        *out++ = x-g*r*v;
        *out++ = y+g*r*u;
        *out++ = u;
        *out++ = v;
        *out++ = f*r;

        return res;
    }
};
*/

#endif // RECTANGLE_SPLIT_MERGE_KERNEL_HPP
