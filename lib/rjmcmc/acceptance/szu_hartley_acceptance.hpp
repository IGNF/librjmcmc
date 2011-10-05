#ifndef SZU_HARTLEY_ACCEPTANCE_HPP
#define SZU_HARTLEY_ACCEPTANCE_HPP

namespace rjmcmc
{
    /**
     * \ingroup GroupAcceptance
     *
     * This class is a model of the Acceptance concept and implements <i>Szu and Hartley</i> fast annealing.
     * This is the first variant proposed variant of the classical Metropolis acceptance rule.
     * \f[P_{SH}=\frac{1}{1+\exp\left(\Delta E / T\right)}\f]
     * This probability is always less than the Metropolis one, the difference becoming negligible for large \f$\Delta E / T\f$.
     */
    class szu_hartley_acceptance
    {

    public:
        inline double operator()(double delta, double temperature, double green_ratio) const
        {
            return green_ratio/(1.+exp(delta/temperature));
        }
    };

} // namespace rjmcmc

#endif // SZU_HARTLEY_ACCEPTANCE_HPP
