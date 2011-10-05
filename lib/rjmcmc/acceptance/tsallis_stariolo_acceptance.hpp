#ifndef TSALLIS_STARIOLO_ACCEPTANCE_HPP
#define TSALLIS_STARIOLO_ACCEPTANCE_HPP

namespace rjmcmc
{
    /**
     * \ingroup GroupAcceptance
     *
     * This class is a model of the Acceptance concept and implements <i>Tsallis</i> acceptance rule. This is part of family of rules that occasionally accept large moves in energy, moves driven by a parameter \f$q\f$.
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
    class tsallis_tsariolo_acceptance
    {
    private:
        double  m_q, m_inv_1_less_q;

    public:
        tsallis_tsariolo_acceptance(double q)
            , m_q(q)
            , m_inv_1_less_q(1./(1.-q)) {}

        inline double operator()(double delta, double temperature, double green_ratio) const
        {
            double v = 1. - (delta/(temperature*m_inv_1_less_q));
            if(v<=0.) return 0.;
            return green_ratio*std::pow(v, m_inv_1_less_q);
        }
    };

} // namespace rjmcmc

#endif // TSALLIS_STARIOLO_ACCEPTANCE_HPP
