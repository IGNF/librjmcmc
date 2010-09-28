#ifndef FRANZ_HOFFMANN_SAMPLER_HPP
#define FRANZ_HOFFMANN_SAMPLER_HPP

#include "sampler_base.hpp"

namespace rjmcmc
{

    template<typename CountSampler, RJMCMC_TUPLE_TYPENAMES>
    class franz_hoffmann_sampler : public sampler_base<franz_hoffmann_sampler<CountSampler,RJMCMC_TUPLE_TYPES>, CountSampler, RJMCMC_TUPLE_TYPES>
    {
    private:
        typedef franz_hoffmann_sampler<CountSampler, RJMCMC_TUPLE_TYPES> self;
        typedef sampler_base<self, CountSampler, RJMCMC_TUPLE_TYPES> base;

        double  m_q, m_inv_1_less_q, m_factor;

    public:
        franz_hoffmann_sampler(double q, const CountSampler& cs, RJMCMC_TUPLE_ARGS)
            : base(cs, RJMCMC_TUPLE_PARAMS)
            , m_q(q)
            , m_inv_1_less_q(1./(1.-q))
            , m_factor((1.-q)/(2.-q)) {}

        inline double acceptance_probability() const
        {
            double v = 1. - m_factor*base::m_delta/base::m_temperature;
            if(v<=0.) return 0.;
            return base::m_green_ratio*std::pow(v, m_inv_1_less_q);
        }
    };

} // namespace rjmcmc

#endif // FRANZ_HOFFMANN_SAMPLER_HPP
