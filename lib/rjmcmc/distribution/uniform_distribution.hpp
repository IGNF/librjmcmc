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

#ifndef UNIFORM_DISTRIBUTION_HPP
#define UNIFORM_DISTRIBUTION_HPP

#include "rjmcmc/random.hpp"
#include <boost/random/uniform_smallint.hpp>
// boost::math::uniform is not used as it is linked to real values rather than discrete integral values

namespace rjmcmc {

    // 1_(n\in[a,b])/(b-a+1)
    class uniform_distribution {
    public:
        typedef double real_type;
        typedef int    int_type;
        typedef boost::uniform_smallint<int_type> rand_distribution_type;
        typedef boost::variate_generator<rjmcmc::mt19937_generator&,rand_distribution_type> variate_generator_type;

        uniform_distribution(int_type a, int_type b)
            : m_variate(rjmcmc::random(),rand_distribution_type(a,b))
            , m_pdf(real_type(1)/(b-a+1)) {}

        // new/old
        real_type pdf_ratio(int_type n0, int_type n1) const
        {
            assert(pdf(n0)>0);
            return (m_variate.distribution().min() <= n1 && n1 <= m_variate.distribution().max() );
        }

        real_type pdf(int_type n) const
        {
            return m_pdf * (m_variate.distribution().min() <= n && n <= m_variate.distribution().max() );
        }

        inline int_type operator()() const { return m_variate(); }

    private:
        mutable // because boost::poisson_distribution::operator()(Engine&) is non-const
        variate_generator_type m_variate;
        real_type m_pdf;
    };

}; // namespace rjmcmc

#endif // UNIFORM_DISTRIBUTION_HPP
