#ifndef __RANDOM_APPLY_HPP__
#define __RANDOM_APPLY_HPP__

#include "rjmcmc/tuple.hpp"

namespace rjmcmc {

/*
 random_apply(x,t,f) applies
- the function object f
- to a random element of the tuple t (according to their probability())
- x should be drawn uniformly in [0,1]
*/

namespace internal {

template <int I, int N> struct random_apply_impl
{
  template <typename T, typename F>
    inline typename F::result_type operator()(double x, T& t, F &f) {
      double xt = x - get<I>(t).probability();
      if(xt<0) return f(get<I>(t));
      return random_apply_impl<I+1,N>()(xt,t,f);
    }
};

template <int N> struct random_apply_impl<N,N>
{
  template <typename T, typename F>
    inline typename F::result_type operator()(double x, T& t, F &f) {
      return typename F::result_type();
    }
};

};

template <typename T, typename F>
  inline typename F::result_type random_apply(double x, T& t, F &f) {
    return internal::random_apply_impl<0,tuple_size<T>::value>()(x,t,f);
  }

}; // namespace rjmcmc

#endif // __RANDOM_APPLY_HPP__
