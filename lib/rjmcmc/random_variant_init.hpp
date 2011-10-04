
#ifndef __RJMCMC_RANDOM_VARIANT_INIT_HPP__
#define __RJMCMC_RANDOM_VARIANT_INIT_HPP__

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

#endif // __RJMCMC_RANDOM_VARIANT_INIT_HPP__
