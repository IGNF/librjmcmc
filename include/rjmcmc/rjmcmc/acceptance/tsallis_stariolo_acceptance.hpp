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
