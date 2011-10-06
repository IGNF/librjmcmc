#ifndef __UNIFORM_DISTRIBUTION_HPP__
#define __UNIFORM_DISTRIBUTION_HPP__

#include <boost/random/uniform_smallint.hpp>
// boost::math::uniform is not used as it is linked to real values rather than discrete integral values

namespace rjmcmc {

    // 1_(n\in[a,b])/(b-a+1)
    class uniform_distribution {
    public:
        typedef double real_type;
        typedef int    int_type;
        typedef boost::uniform_smallint<int_type> rand_distribution_type;

        uniform_distribution(int_type a, int_type b)
            : m_rand_distribution(a,b), m_pdf(real_type(1)/(b-a+1)) {}

        // new/old
        real_type pdf_ratio(int_type n0, int_type n1) const
        {
            assert(pdf(n0)>0);
            return (m_rand_distribution.min() <= n1 && n1 <= m_rand_distribution.max() );
        }

        real_type pdf(int_type n) const
        {
            return m_pdf * (m_rand_distribution.min() <= n && n <= m_rand_distribution.max() );
        }

        template<typename Engine>
        inline int_type operator()(Engine& engine) const { return m_rand_distribution(engine); }

    private:
        mutable // because boost::poisson_distribution::operator()(Engine&) is non-const
        rand_distribution_type m_rand_distribution;
        real_type m_pdf;
    };

}; // namespace rjmcmc

#endif // __UNIFORM_DISTRIBUTION_HPP__
