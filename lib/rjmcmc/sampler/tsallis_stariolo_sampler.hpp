#ifndef TSALLIS_STARIOLO_SAMPLER_HPP
#define TSALLIS_STARIOLO_SAMPLER_HPP

#include "sampler_base.hpp"

namespace rjmcmc
{
    /**
     * \ingroup GroupSampler
     *
     * This class is a model of the Sampler concept and implements <i>Tsallis</i> acceptance rule. This is part of family of rules that occasionally accept large moves in energy, moves driven by a parameter \f$q\f$.
     * \f[
     * P_{Tsallis} = \left\{
     *                      \begin{array}{ll}
     *                          1 & \texttt{if } \Delta E\leq 0 \\
     *                          \left(1-\left(1-q\right)\frac{\Delta E}{T}\right)^{\frac{1}{1-q}} & \texttt{if } \Delta E> 0 \texttt{ and } \left(1-q\right)\frac{\Delta E}{T}\leq 1 \\
     *                          0 & \texttt{if } \Delta E> 0 \texttt{ and } \left(1-q\right)\frac{\Delta E}{T}>1 \\
     *                      \end{array}
     *               \right.
     * \f]
     * When \f$q\f$ tends to 1, <i>Tsallis</i> acceptance rule is equivalent to <i>Metropolis</i> acceptance rule. The final solution seems to be independant of \f$q\f$, but is reached quicker while decreasing \f$q\f$.
     */
    template<typename CountSampler, RJMCMC_TUPLE_TYPENAMES>
    class tsallis_tsariolo_sampler
        : public sampler_base<tsallis_tsariolo_sampler<CountSampler,RJMCMC_TUPLE_TYPES>, CountSampler, RJMCMC_TUPLE_TYPES>
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
