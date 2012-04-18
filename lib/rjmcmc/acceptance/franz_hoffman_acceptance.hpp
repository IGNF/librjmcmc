/***********************************************************************
This file is part of the librjmcmc project source files.

Copyright : Institut Geographique National (2008-2012)
Contributors : Mathieu Brédif, Olivier Tournaire, Didier Boldo
email : librjmcmc@ign.fr

This software is a generic C++ library for stochastic optimization.

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software. You can use,
modify and/or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty and the software's author, the holder of the
economic rights, and the successive licensors have only limited liability.

In this respect, the user's attention is drawn to the risks associated
with loading, using, modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean that it is complicated to manipulate, and that also
therefore means that it is reserved for developers and experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and, more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

***********************************************************************/

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
