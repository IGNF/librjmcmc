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

#ifndef RJMCMC_TUPLE_HPP
#define RJMCMC_TUPLE_HPP

#include <tuple> 

#define RJMCMC_TUPLE_ARGS      const RJMCMC_TUPLE_T&... rjmcmc_tuple_t
#define RJMCMC_TUPLE_TYPENAMES typename... RJMCMC_TUPLE_T
#define RJMCMC_TUPLE_TYPES     RJMCMC_TUPLE_T...
#define RJMCMC_TUPLE_PARAMS    rjmcmc_tuple_t...

namespace rjmcmc {
    using std::tuple;
    using std::get;
    using std::tuple_size;
    using std::tuple_element;

}; // namespace rjmcmc

namespace rjmcmc {
    namespace internal {
        template<unsigned int I, unsigned int N>
        struct for_each_impl
        {
            template<typename T, typename F> inline void operator()(T& t, F& f) const
            {
                f(get<I>(t));
                for_each_impl<I+1,N>()(t,f);
            }
        };

        template<unsigned int N>
        struct for_each_impl<N,N>
        {
            template<typename T, typename F> inline void operator()(T& t, F& f) const {}
        };
    } // namespace internal

    template<typename T, typename F>
    inline void for_each(T& t, F& f)
    {
        internal::for_each_impl<0,tuple_size<T>::value>()(t,f);
    }
};


#endif // RJMCMC_TUPLE_HPP
