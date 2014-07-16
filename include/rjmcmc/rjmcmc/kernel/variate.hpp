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

#ifndef RJMCMC_VARIATE_HPP
#define RJMCMC_VARIATE_HPP

#include <boost/random/uniform_real.hpp>
#include "null_variate.hpp"

namespace rjmcmc {

    template<int N>
    class variate
    {
        typedef boost::uniform_real<> rand_type;
    protected:
        mutable rand_type m_rand; // rand_type::operator()(Engine&) is non-const...
    public:
        typedef double value_type;
        enum { dimension = N };
        template<typename Engine, typename OutputIterator>
        inline double operator()(Engine& e, OutputIterator it) const {
            for(unsigned int i=0; i<N; ++i) *it++ = m_rand(e);
            return 1.;
        }
        template<typename InputIterator>
        inline double pdf(InputIterator it) const {
            for(unsigned int i=0; i<N; ++i, ++it) if(*it<0 || *it>1) return 0;
            return 1.;
        }
        variate() : m_rand(0,1) {}
    };


    template<> class variate<0> : public null_variate {};

}; // namespace rjmcmc

#endif // RJMCMC_VARIATE_HPP
