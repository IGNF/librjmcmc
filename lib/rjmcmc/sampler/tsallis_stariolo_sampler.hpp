#ifndef TSALLIS_STARIOLO_SAMPLER_HPP
#define TSALLIS_STARIOLO_SAMPLER_HPP

#include "sampler_base.hpp"

namespace rjmcmc
{

template<typename CountSampler, RJMCMC_TUPLE_TYPENAMES>
class tsallis_tsariolo_sampler : public sampler_base<tsallis_tsariolo_sampler<CountSampler,RJMCMC_TUPLE_TYPES>, CountSampler, RJMCMC_TUPLE_TYPES>
{
private:
    typedef tsallis_tsariolo_sampler<CountSampler, RJMCMC_TUPLE_TYPES> self;
    typedef sampler_base<self, CountSampler, RJMCMC_TUPLE_TYPES> base;

    double  m_q, m_inv_1_less_q;

public:
    tsallis_tsariolo_sampler(double q, const CountSampler& cs, RJMCMC_TUPLE_ARGS)
        : base(cs,RJMCMC_TUPLE_PARAMS)
        , m_q(q)
        , m_inv_1_less_q(1./(1.-q)) {}

    inline double acceptance_probability() const
    {
        double v = 1. - (base::m_delta/(base::m_temperature*m_inv_1_less_q));
        if(v<=0.) return 0.;
        return base::m_green_ratio*std::pow(v, m_inv_1_less_q);
    }
};

} // namespace rjmcmc

#endif // TSALLIS_STARIOLO_SAMPLER_HPP
