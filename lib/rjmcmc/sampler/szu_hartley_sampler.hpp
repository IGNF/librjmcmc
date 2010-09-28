#ifndef SZU_HARTLEY_SAMPLER_HPP
#define SZU_HARTLEY_SAMPLER_HPP

#include "sampler_base.hpp"

namespace rjmcmc
{

    template<typename CountSampler, RJMCMC_TUPLE_TYPENAMES>
    class szu_hartley_sampler
        : public sampler_base<szu_hartley_sampler<CountSampler,RJMCMC_TUPLE_TYPES>, CountSampler, RJMCMC_TUPLE_TYPES>
    {
    private:
        typedef szu_hartley_sampler<CountSampler, RJMCMC_TUPLE_TYPES> self;
        typedef sampler_base<self, CountSampler, RJMCMC_TUPLE_TYPES> base;

    public:
        szu_hartley_sampler(const CountSampler& cs, RJMCMC_TUPLE_ARGS) : base(cs,RJMCMC_TUPLE_PARAMS) {}

        inline double acceptance_probability() const {
            return base::m_green_ratio/(1.+exp(base::m_delta/base::m_temperature));
        }
    };

} // namespace rjmcmc

#endif // SZU_HARTLEY_SAMPLER_HPP
