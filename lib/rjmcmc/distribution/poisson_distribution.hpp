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
