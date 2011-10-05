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

    namespace detail {

        template <unsigned int I, unsigned int N> struct random_apply_impl
        {
            template <typename T, typename F>
                    inline typename F::result_type operator()(unsigned int& i, double x, T& t, F &f) {
                double y = x - get<I>(t).probability();
                if(y>0) return random_apply_impl<I+1,N>()(i,y,t,f);
                i = I;
                return f(x,get<I>(t));
            }
        };

        template <unsigned int N> struct random_apply_impl<N,N>
        {
            template <typename T, typename F>
                    inline typename F::result_type operator()(unsigned int& i, double x, T& t, F &f) {
                i = N;
                return typename F::result_type();
            }
        };

        template <unsigned int I, unsigned int N> struct random_apply_normalisation
        {
            template <typename T>
                    inline double operator()(const T& t) {
                return get<I>(t).probability()+random_apply_normalisation<I+1,N>()(t);
            }
        };

        template <unsigned int N> struct random_apply_normalisation<N,N>
        {
            template <typename T>
                    inline double operator()(const T& t) {
                return 0;
            }
        };
    };

    template <typename T, typename F>
            inline typename F::result_type random_apply(unsigned int& i, double x, T& t, F &f) {
        double normalisation = detail::random_apply_normalisation<0,tuple_size<T>::value>()(t);
        return detail::random_apply_impl<0,tuple_size<T>::value>()(i,x*normalisation,t,f);
    }

}; // namespace rjmcmc

#endif // __RANDOM_APPLY_HPP__
