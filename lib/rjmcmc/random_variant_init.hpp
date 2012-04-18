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

#ifndef RJMCMC_RANDOM_VARIANT_INIT_HPP
#define RJMCMC_RANDOM_VARIANT_INIT_HPP

#include "rjmcmc/variant.hpp"
#include "rjmcmc/random.hpp"
#include <boost/mpl/at.hpp>

namespace rjmcmc {

    template<typename T>
    inline void random_variant_init(T &)
    {
    }

#include <boost/preprocessor/iteration/local.hpp>
    template<BOOST_VARIANT_ENUM_PARAMS(typename T)>
    inline void random_variant_init(boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> &v)
    {
        typedef boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> T;
        typedef typename T::types types;
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_smallint<> > die_type;
        static const int N = variant_size<T>::value;
        die_type vdie(random(), boost::uniform_smallint<>(0,N-1));
        switch(vdie())        {
#define BOOST_PP_LOCAL_LIMITS (0,BOOST_VARIANT_LIMIT_TYPES-1)
#define BOOST_PP_LOCAL_MACRO(n) case n : v = typename boost::mpl::at_c<types,n%N>::type(); break;
#include BOOST_PP_LOCAL_ITERATE()
        default : break; // should not get here //TODO: throw !
    }
    }

}

#endif // RJMCMC_RANDOM_VARIANT_INIT_HPP
