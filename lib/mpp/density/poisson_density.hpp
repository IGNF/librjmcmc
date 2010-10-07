#ifndef __POISSON_DENSITY_HPP__
#define __POISSON_DENSITY_HPP__

#include "rjmcmc/random.hpp"
#include <boost/random/poisson_distribution.hpp>

namespace marked_point_process {

    // exp(-poisson) * (poisson^n) / n!
    class poisson_density {
    public:
        typedef boost::variate_generator<rjmcmc::generator&, boost::poisson_distribution<> > density;
        poisson_density(double poisson)
            : m_poisson(poisson)
            , m_die(rjmcmc::random(), boost::poisson_distribution<>(poisson)) {}

        // old/new : (m_poisson^dn) * n! / (n+dn)!
        template<typename Configuration, typename Modification>
        double pdf_ratio(const Configuration &c, const Modification &m) const
        {
            unsigned int n = c.size();
            int dn = m.birth_size()-m.death_size();
            double res = 1.;
            for(   ;dn>0;--dn) res *= m_poisson/(n+dn);
            for(++n;dn<0;++dn) res *= (n+dn)/m_poisson;
            return res;
        }

        inline unsigned int operator()() const { return m_die(); }

    private:
        double m_poisson;
        mutable density m_die;
    };

}; // namespace marked_point_process

#endif // __POISSON_DENSITY_HPP__
