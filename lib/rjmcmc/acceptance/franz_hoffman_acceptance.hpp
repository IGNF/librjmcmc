#ifndef FRANZ_HOFFMANN_ACCEPTANCE_HPP
#define FRANZ_HOFFMANN_ACCEPTANCE_HPP

namespace rjmcmc
{
    /**
     * \ingroup GroupAcceptance
     *
     * This class is a model of the Acceptance concept and implements <i>Franz and Hoffmann</i> acceptance rule, a modification of <i>Tsallis</i> rule:
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
    class franz_hoffmann_acceptance
    {
    private:
        double  m_q, m_inv_1_less_q, m_factor;

    public:
        franz_hoffmann_acceptance(double q)
            , m_q(q)
            , m_inv_1_less_q(1./(1.-q))
            , m_factor((1.-q)/(2.-q)) {}

        inline double operator()(double delta, double temperature, double green_ratio) const
        {
            double v = 1. - m_factor*delta/temperature;
            if(v<=0.) return 0.;
            return green_ratio*std::pow(v, m_inv_1_less_q);
        }
    };

} // namespace rjmcmc

#endif // FRANZ_HOFFMANN_ACCEPTANCE_HPP
