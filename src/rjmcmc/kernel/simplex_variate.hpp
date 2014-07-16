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

#ifndef RJMCMC_SIMPLEX_VARIATE_HPP
#define RJMCMC_SIMPLEX_VARIATE_HPP

#include <boost/random/uniform_real.hpp>

namespace rjmcmc {

    template<int N> struct factorial    { enum { value = factorial<N-1>::value * N };};
    template<     > struct factorial<0> { enum { value = 1 }; };


    struct simplex_variate_log_policy
    {
        template<int N, typename Engine, typename OutputIterator, typename Rand>
        void apply(Engine& e, OutputIterator it, Rand& rand) const
        {
            double sum = 0;
            double x[N+1];
            for(unsigned int i=0; i<=N; ++i)
            {
                x[i] = -log(rand(e));
                sum += x[i];
            }
            for(unsigned int i=0; i<N; ++i, ++it)
            {
                *it = x[i]/sum;
            }
        }
    };


    struct simplex_variate_sort_policy
    {
        template<int N, typename Engine, typename OutputIterator, typename Rand>
        void apply(Engine& e, OutputIterator it, Rand& rand) const
        {
            double x[N];
            for(unsigned int i=0; i<N; ++i) x[i] = rand(e);
            std::sort(x,x+N);
            double prev = 0;
            for(unsigned int i=0; i<N; ++i, ++it)
            {
                *it = x[i]-prev;
                prev = x[i];
            }
        }
    };


    struct simplex_variate_power_policy
    {
        template<int N, typename Engine, typename OutputIterator, typename Rand>
        void apply(Engine& e, OutputIterator it, Rand& rand) const
        {
            static_assert(N>1,"simplex_variate_power_policy is only valid for 2D variates or more !");
            double prev = pow(rand(e),1./N);
            for(unsigned int i=N-1; i>1; --i)
            {
                double next = prev*pow(rand(e),1./i);
                *it++ = prev-next;
                prev = next;
            }
            double next = prev*rand(e);
            *it++ = prev-next;
            *it++ = next;
        }
    };


    struct simplex_variate_rejection_policy
    {
        template<int N, typename Engine, typename OutputIterator, typename Rand>
        void apply(Engine& e, OutputIterator it, Rand& rand) const
        {
            for(;;)
            {
                double x[N], sum;
                for(unsigned int i=0; i<N; ++i)
                {
                    x[i] = rand(e);
                    sum += x[i];
                }
                if(sum<=1.)
                {
                    for(unsigned int i=0; i<N; ++i)  *it++ = x[i];
                    return;
                }
            }
        }
    };

    struct simplex_variate_reflect_policy
    {
        template<int N, typename Engine, typename OutputIterator, typename Rand>
        void apply(Engine& e, OutputIterator it, Rand& rand) const
        {
            static_assert(N==2,"simplex_variate_reflect_policy is only valid for 2D variates !");
            double x = rand(e);
            double y = rand(e);
            if(x+y>1.)
            {
                x = 1.-x;
                y = 1.-y;
            }
            *it++ = x;
            *it++ = y;
        }
    };

    struct simplex_variate_sqrt_policy
    {
        template<int N, typename Engine, typename OutputIterator, typename Rand>
        void apply(Engine& e, OutputIterator it, Rand& rand) const
        {
            static_assert(N==2,"simplex_variate_sqrt_policy is only valid for 2D variates, use the generalized simplex_variate_power_policy instead !");
            double x = sqrt(rand(e));
            double y = x*rand(e);
            *it++ = x-y;
            *it++ = y;
        }
    };


    template<int N, typename Policy = simplex_variate_power_policy >
    class simplex_variate
    {
        typedef boost::uniform_real<> rand_type;
        mutable rand_type m_rand;
        const double m_pdf;
        Policy m_policy;
    public:
        typedef double value_type;
        enum { dimension = N };
        template<typename InputIterator>
        inline double pdf(InputIterator it) const {
            double sum = 0;
            for(unsigned int i=0; i<N; ++i, ++it)
            {
                if(*it<0) return 0;
                sum += *it;
            }
            return double(sum <= 1.)*m_pdf;
        }
        template<typename Engine, typename OutputIterator>
        inline double operator()(Engine& e, OutputIterator it) const {
            m_policy.template apply<N>(e,it,m_rand);
            return m_pdf;
        }
        simplex_variate() : m_rand(0,1), m_pdf(1./factorial<N>::value) {}

    };

}; // namespace rjmcmc

#endif // RJMCMC_SIMPLEX_VARIATE_HPP
