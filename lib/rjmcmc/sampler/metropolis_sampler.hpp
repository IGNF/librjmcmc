#ifndef METROPOLIS_SAMPLER_HPP
#define METROPOLIS_SAMPLER_HPP

#include "sampler_base.hpp"

namespace rjmcmc
{
    /**
     * \ingroup GroupSampler
     *
     * This class is a model of the Sampler concept and implements <i>Metropolis</i> acceptance rule. The new state is accepted with probability:
     * \f[P_{Metropolis}=\min\left(1,\exp\left(-\Delta E / T\right)\right)\f]
     */
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
