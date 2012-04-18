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

#if USE_VARIADIC_TEMPLATES

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

#else // USE_VARIADIC_TEMPLATES

#include <boost/tuple/tuple.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_params_with_a_default.hpp>

#ifndef RJMCMC_TUPLE_LIMIT_TYPES
#  define RJMCMC_TUPLE_LIMIT_TYPES 5
#endif // RJMCMC_TUPLE_LIMIT_TYPES
#define RJMCMC_TUPLE_ENUM_PARAMS(x) \
BOOST_PP_ENUM_PARAMS(RJMCMC_TUPLE_LIMIT_TYPES,x)
#define RJMCMC_TUPLE_ENUM_PARAMS_WITH_A_DEFAULT(x) \
        BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(RJMCMC_TUPLE_LIMIT_TYPES,x,boost::tuples::null_type)
#define RJMCMC_TUPLE_ENUM(x) \
        BOOST_PP_ENUM(RJMCMC_TUPLE_LIMIT_TYPES,x,nil)
#define RJMCMC_TUPLE_ARG(z,n,_) const RJMCMC_TUPLE_T##n& rjmcmc_tuple_t##n = RJMCMC_TUPLE_T##n()

#define RJMCMC_TUPLE_ARGS      RJMCMC_TUPLE_ENUM(RJMCMC_TUPLE_ARG)
#define RJMCMC_TUPLE_TYPENAMES RJMCMC_TUPLE_ENUM_PARAMS_WITH_A_DEFAULT(typename RJMCMC_TUPLE_T)
#define RJMCMC_TUPLE_TYPES     RJMCMC_TUPLE_ENUM_PARAMS(RJMCMC_TUPLE_T)
#define RJMCMC_TUPLE_PARAMS    RJMCMC_TUPLE_ENUM_PARAMS(rjmcmc_tuple_t)

        namespace rjmcmc {
    using boost::tuple;
    using boost::get;
    template<typename T> struct tuple_size : public  boost::tuples::length<T> {};
    template<unsigned int N, typename T> struct tuple_element : public  boost::tuples::element<N,T> {};
};

#endif // USE_VARIADIC_TEMPLATES

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
