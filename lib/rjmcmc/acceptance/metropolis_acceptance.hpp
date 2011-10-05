#ifndef METROPOLIS_ACCEPTANCE_HPP
#define METROPOLIS_ACCEPTANCE_HPP

namespace rjmcmc
{
    /**
     * \ingroup GroupAcceptance
     *
     * This class is a model of the Acceptance concept and implements <i>Metropolis</i> acceptance rule. The new state is accepted with probability:
     * \f[P_{Metropolis}=\min\left(1,\exp\left(-\Delta E / T\right)\right)\f]
     */
    class metropolis_acceptance
    {
    public:
        inline double operator()(double delta, double temperature, double green_ratio) const
        {
            return green_ratio*exp(-delta/temperature);
        }
    };

} // namespace rjmcmc

#endif // METROPOLIS_ACCEPTANCE_HPP
