#ifndef __RJMCMC_TUPLE_HPP__
#define __RJMCMC_TUPLE_HPP__

#if USE_VARIADIC_TEMPLATES

#include <tuple> 

#define RJMCMC_TUPLE_ARGS      (const RJMCMC_TUPLE_T&... rjmcmc_tuple_t)
#define RJMCMC_TUPLE_TYPENAMES (typename... RJMCMC_TUPLE_T)
#define RJMCMC_TUPLE_TYPES     (RJMCMC_TUPLE_T...)
#define RJMCMC_TUPLE_PARAMS    (rjmcmc_tuple_t...)

namespace rjmcmc {
using std::tuple;
using std::get;
using std::tuple_size;
using std::tuple_element;
};

#else // USE_VARIADIC_TEMPLATES

#include <boost/tuple/tuple.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_params_with_a_default.hpp>

#ifndef RJMCMC_TUPLE_LIMIT_TYPES
#  define RJMCMC_TUPLE_LIMIT_TYPES 2
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

#endif // __RJMCMC_TUPLE_HPP__
