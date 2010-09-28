#ifndef METROPOLIS_SAMPLER_HPP
#define METROPOLIS_SAMPLER_HPP

#include "sampler_base.hpp"

namespace rjmcmc
{

    template<typename CountSampler, RJMCMC_TUPLE_TYPENAMES>
    class metropolis_sampler
        : public sampler_base<metropolis_sampler<CountSampler,RJMCMC_TUPLE_TYPES>, CountSampler, RJMCMC_TUPLE_TYPES>
    {
    private:
        typedef metropolis_sampler<CountSampler, RJMCMC_TUPLE_TYPES> self;
        typedef sampler_base<self, CountSampler, RJMCMC_TUPLE_TYPES> base;

    public:
        metropolis_sampler(const CountSampler& cs, RJMCMC_TUPLE_ARGS) : base(cs,RJMCMC_TUPLE_PARAMS) {}

        inline double acceptance_probability() const {
            return base::m_green_ratio*exp(-base::m_delta/base::m_temperature);
        }
    };

} // namespace rjmcmc

#endif // METROPOLIS_SAMPLER_HPP
