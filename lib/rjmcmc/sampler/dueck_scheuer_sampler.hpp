#ifndef DUECK_SCHEUER_SAMPLER_HPP
#define DUECK_SCHEUER_SAMPLER_HPP

#include "sampler_base.hpp"

namespace rjmcmc
{
    /**
     * \ingroup GroupSampler
     *
     * This class is a model of the Sampler concept and implements the threshold acceptance rule (introduced by <i>Dueck</i> and <i>Scheuer</i>). Compared to the <i>Metropolis</i> acceptance rule, it avoids the cost of an exponential:
     * \f[
     * P_{Dueck-Scheuer} = \left\{
     *                      \begin{array}{ll}
     *                          1 & \texttt{if } \Delta E\leq T \\
     *                          0 & \texttt{if } \Delta E> T \\
     *                      \end{array}
     *               \right.
     * \f]
     */
    template<typename CountSampler, RJMCMC_TUPLE_TYPENAMES>
    class dueck_scheuer_sampler
        : public sampler_base<dueck_scheuer_sampler<CountSampler, RJMCMC_TUPLE_TYPES>, CountSampler, RJMCMC_TUPLE_TYPES>
    {
    private:
        typedef dueck_scheuer_sampler<CountSampler, RJMCMC_TUPLE_TYPES> self;
        typedef sampler_base<self, CountSampler, RJMCMC_TUPLE_TYPES> base;

    public:
        dueck_scheuer_sampler(const CountSampler& cs, RJMCMC_TUPLE_ARGS) : base(cs,RJMCMC_TUPLE_PARAMS) {}

        inline double acceptance_probability() const {
            // return (metropolis_acceptance_probability()>=alpha) ? 1 : 0;
            // return (R.exp(-E/T)>=alpha) ? 1 : 0;
            // return (log(R)-E/T>=log(alpha)) ? 1 : 0;
            // return (E<=T(log(R)-log(alpha)) ? 1 : 0; with alpha = exp(-1)
            return (base::m_delta <= base::m_temperature*(log(base::m_green_ratio)+1)) ? 1. : 0.;
        }
    };

} // namespace rjmcmc

#endif // DUECK_SCHEUER_SAMPLER_HPP
