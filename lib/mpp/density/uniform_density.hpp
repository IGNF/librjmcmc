#ifndef __UNIFORM_DENSITY_HPP__
#define __UNIFORM_DENSITY_HPP__

#include "rjmcmc/random.hpp"
#include <boost/random/uniform_smallint.hpp>

namespace marked_point_process {

    // 1_(n\in[a,b])/(b-a+1)
    class uniform_density {
    public:
        typedef boost::variate_generator<rjmcmc::generator&, boost::uniform_smallint<> > density;
        uniform_density(unsigned int a, unsigned int b)
            : m_min(a), m_max(b)
            , m_die(rjmcmc::random(), boost::uniform_smallint<>(a,b)) {}

        uniform_density(unsigned int b)
            : m_min(0), m_max(b)
            , m_die(rjmcmc::random(), boost::uniform_smallint<>(0,b)) {}

        template<typename Configuration, typename Modification>
        double pdf_ratio(const Configuration &c, const Modification &m) const
        {
            unsigned int n0 = c.size();
            unsigned int n1 = (n0 + m.birth_size())-m.death_size();
            return (n0<m_min || n1<m_min || n0>m_max || n1>m_max)? 0. : 1.;
        }

        inline unsigned int operator()() const { return m_die(); }

    private:
        unsigned int m_min, m_max;
        mutable density m_die;
    };

}; // namespace marked_point_process

#endif // __UNIFORM_DENSITY_HPP__
