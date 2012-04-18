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

#ifndef __POISSON_DISTRIBUTION_HPP__
#define __POISSON_DENSITY_HPP__

#include <boost/random/poisson_distribution.hpp>
#include <boost/math/distributions/poisson.hpp>
#include <boost/random/variate_generator.hpp>
#include "rjmcmc/random.hpp"

namespace rjmcmc {

    // exp(-mean) * (mean^n) / n!
    class poisson_distribution {
    public:
        typedef double real_type;
        typedef int    int_type;
        typedef boost::poisson_distribution<int_type,real_type> rand_distribution_type;
        typedef boost::math::poisson_distribution<real_type>    math_distribution_type;
        typedef boost::variate_generator<rjmcmc::mt19937_generator&,rand_distribution_type> variate_generator_type;

        poisson_distribution(real_type mean)
            : m_variate(rjmcmc::random(),rand_distribution_type(mean))
            , m_distribution(mean)
        {}

        // new/old: (mean^(n1-n0) * n0! / n1!
        real_type pdf_ratio(int_type n0, int_type n1) const
        {
            real_type res = 1.;
            for(;n1>n0;--n1) res *= m_distribution.mean()/n1;
            for(;n0>n1;--n0) res *= n0/m_distribution.mean();
            return res;
        }

        real_type pdf(int_type n) const
        {
            return boost::math::pdf(m_distribution, n);
        }

        inline int_type operator()() const { return m_variate(); }

    private:
        mutable // because boost::poisson_distribution::operator()(Engine&) is non-const
        variate_generator_type m_variate;
        math_distribution_type m_distribution;
    };

}; // namespace rjmcmc

#endif // __POISSON_DENSITY_HPP__
