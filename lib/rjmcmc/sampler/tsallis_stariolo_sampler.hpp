#ifndef TSALLIS_STARIOLO_SAMPLER_HPP
#define TSALLIS_STARIOLO_SAMPLER_HPP

#include "sampler.hpp"

namespace rjmcmc
{

template<RJMCMC_SAMPLER_TYPENAMES>
class tsallis_tsariolo_sampler : public sampler_base<tsallis_tsariolo_sampler<RJMCMC_SAMPLER_TYPES>, RJMCMC_SAMPLER_TYPES>
{
private:
    typedef tsallis_tsariolo_sampler<RJMCMC_SAMPLER_TYPES> self;
    typedef sampler_base<self, RJMCMC_SAMPLER_TYPES> base;

    double  m_q, m_one_less_q, m_factor;

public:
    tsallis_tsariolo_sampler(double q, RJMCMC_SAMPLER_ARGS) : base(RJMCMC_SAMPLER_PARAMS) , m_q(q), m_one_less_q(1.-q), m_factor((1.-q)/(2.-q)) {}

    inline double acceptance_probability() const
    {
        if(base::m_delta<=0.) return 1.;
        else
        {
            double v = m_factor*base::m_delta/base::m_temperature;
            if(v<=1.) return std::pow((1.-v), 1./m_one_less_q);
            else return 0.;
        }
    }
};

} // namespace rjmcmc

#endif // TSALLIS_STARIOLO_SAMPLER_HPP
