#ifndef DUECK_SCHEUER_SAMPLER_HPP
#define DUECK_SCHEUER_SAMPLER_HPP

#include "sampler_base.hpp"

namespace rjmcmc
{

template<RJMCMC_SAMPLER_TYPENAMES>
class dueck_scheuer_sampler : public sampler_base<dueck_scheuer_sampler<RJMCMC_SAMPLER_TYPES>, RJMCMC_SAMPLER_TYPES>
{
private:
    typedef dueck_scheuer_sampler<RJMCMC_SAMPLER_TYPES> self;
    typedef sampler_base<self, RJMCMC_SAMPLER_TYPES> base;

public:
    dueck_scheuer_sampler(RJMCMC_SAMPLER_ARGS) : base(RJMCMC_SAMPLER_PARAMS) {}

    inline double acceptance_probability() const {
        return (base::m_delta <= base::m_temperature) ? /*1.**/base::m_green_ratio : 0.;
    }
};

} // namespace rjmcmc

#endif // DUECK_SCHEUER_SAMPLER_HPP
