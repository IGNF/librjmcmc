#ifndef DUECK_SCHEUER_ACCEPTANCE_HPP
#define DUECK_SCHEUER_ACCEPTANCE_HPP

namespace rjmcmc
{
    /**
     * \ingroup GroupAcceptance
     *
     * This class is a model of the Acceptance concept and implements the threshold acceptance rule (introduced by <i>Dueck</i> and <i>Scheuer</i>).
     * Compared to the <i>Metropolis</i> acceptance rule, it avoids the cost of an exponential:
     * \f[
     * P_{Dueck-Scheuer} = \left\{
     *                      \begin{array}{ll}
     *                          1 & \texttt{if } \Delta E\leq T \\
     *                          0 & \texttt{if } \Delta E> T \\
     *                      \end{array}
     *               \right.
     * \f]
     * It is of little use in the reversible jump context, as the log of the green_ratio must be computed
     */
    class dueck_scheuer_acceptance
    {
    public:
        inline double operator()(double delta, double temperature, double green_ratio) const
        {
            // return (metropolis_acceptance_probability()>=alpha) ? 1 : 0;
            // return (R.exp(-E/T)>=alpha) ? 1 : 0;
            // return (log(R)-E/T>=log(alpha)) ? 1 : 0;
            // return (E<=T(log(R)-log(alpha)) ? 1 : 0; with alpha = exp(-1)
            return (delta <= temperature*(log(green_ratio)+1)) ? 1. : 0.;
        }
    };

} // namespace rjmcmc

#endif // DUECK_SCHEUER_ACCEPTANCE_HPP
