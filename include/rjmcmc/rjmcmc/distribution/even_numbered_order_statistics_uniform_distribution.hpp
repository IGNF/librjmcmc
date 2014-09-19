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

#ifndef EVEN_NUMBERED_ORDER_STATISTICS_UNIFORM_DISTRIBUTION_HPP
#define EVEN_NUMBERED_ORDER_STATISTICS_UNIFORM_DISTRIBUTION_HPP

#include <boost/random/uniform_real_distribution.hpp>
// boost::math::uniform is not used as it is linked to real values rather than discrete integral values

namespace rjmcmc {

    // 1_(n\in[a,b])/(b-a+1)
    class even_numbered_order_statistics_uniform_distribution {
    public:
        typedef double real_type;
        typedef int    int_type;
        typedef boost::random::uniform_real_distribution<real_type> rand_distribution_type;

        even_numbered_order_statistics_uniform_distribution(int_type k , real_type a, real_type b)
            : m_rand(a,b)
            , m_size(k) {}

        // new/old
        template<typename Iterator> real_type pdf_ratio(Iterator begin0, Iterator end0, Iterator begin1, Iterator end1) const
        {
            return pdf(begin1,end1)/pdf(begin0,end0);
        }

        template<typename Iterator> real_type pdf(Iterator begin, Iterator end) const
        {
            typedef typename std::iterator_traits<Iterator>::value_type T;
            const T a = m_rand.min();
            const T b = m_rand.max();
            const T s = b - a;
            T x = a;
            T res(1);
            int i = 2;
            for(Iterator it=begin; it != end; ++it, i+=2)
            {
                const T y = *it;
                if(y<x) return 0;
                res*=(y-x)*i*(i+1)/(s*s);
                x=y;
            }
            if(b<x) return 0;
            res *= (b - x) / s;
            return res;
        }

        template<typename Engine, typename Iterator>
        inline void operator()(Engine& e, Iterator out) const {
            std::vector<real_type> v(2*m_size+1);
            for(std::vector<real_type>::iterator it = v.begin(); it!=v.end(); ++it)
                *it = m_rand(e);
            std::sort(v.begin(),v.end());
            for(std::vector<real_type>::iterator it = ++v.begin(); it!=v.end(); it+=2)
                *out++ = *it;
        }

    private:
        mutable rand_distribution_type m_rand;
        int_type m_size;
    };

}; // namespace rjmcmc

#endif // EVEN_NUMBERED_ORDER_STATISTICS_UNIFORM_DISTRIBUTION_HPP
