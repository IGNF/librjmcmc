#ifndef FRANZ_HOFFMANN_SAMPLER_HPP
#define FRANZ_HOFFMANN_SAMPLER_HPP

#include "sampler.hpp"

namespace rjmcmc
{

template<RJMCMC_SAMPLER_TYPENAMES>
class franz_hoffmann_sampler : public sampler_base<franz_hoffmann_sampler<RJMCMC_SAMPLER_TYPES>, RJMCMC_SAMPLER_TYPES>
{
private:
    typedef franz_hoffmann_sampler<RJMCMC_SAMPLER_TYPES> self;
    typedef sampler_base<self, RJMCMC_SAMPLER_TYPES> base;

    double  m_q, m_one_less_q;

public:
    franz_hoffmann_sampler(double q, RJMCMC_SAMPLER_ARGS) : base(RJMCMC_SAMPLER_PARAMS) , m_q(q), m_one_less_q(1.-q) {}

    inline double acceptance_probability() const
    {
        if(base::m_delta<=0.) return 1.;
        else
        {
            double v = m_one_less_q*base::m_delta/base::m_temperature;
            if(v<=1.) return std::pow((1.-v), 1./m_one_less_q);
            else return 0.;
        }
    }
};

} // namespace rjmcmc

#endif // FRANZ_HOFFMANN_SAMPLER_HPP
