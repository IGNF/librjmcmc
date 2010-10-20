#ifndef FRANZ_HOFFMANN_SAMPLER_HPP
#define FRANZ_HOFFMANN_SAMPLER_HPP

#include "sampler_base.hpp"

namespace rjmcmc
{
    /**
     * \ingroup GroupSampler
     *
     * This class is a model of the Sampler concept and implements <i>Franz and Hoffmann</i> acceptance rule, a modification of <i>Tsallis</i> rule:
     * \f[
     * P_{Franz_Hoffmann} = \left\{
     *                      \begin{array}{ll}
     *                          1 & \texttt{if } \Delta E\leq 0 \\
     *                          \left(1-\frac{1-q}{2-q}\frac{\Delta E}{T}\right)^{\frac{1}{1-q}} & \texttt{if } \Delta E> 0 \texttt{ and } \frac{1-q}{2-q}\frac{\Delta E}{T}\leq 1 \\
     *                          0 & \texttt{if } \Delta E> 0 \texttt{ and } \frac{1-q}{2-q}\frac{\Delta E}{T}>1 \\
     *                      \end{array}
     *               \right.
     * \f]
     *
     */
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
