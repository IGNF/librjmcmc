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

#ifndef RJMCMC_TRANSFORMED_VARIATE_HPP
#define RJMCMC_TRANSFORMED_VARIATE_HPP

namespace rjmcmc {

    template<typename Transform, typename Variate = typename rjmcmc::variate<Transform::dimension> > // assert(Transform::dimension==Variate::dimension)
    class transformed_variate
    {
        Transform m_transform;
        Variate m_variate;
    public:
        typedef typename Variate::value_type value_type;
        enum { dimension = Variate::dimension };
        template<typename Engine, typename OutputIterator>
        inline double operator()(Engine& e, OutputIterator it) const {
            value_type val[dimension];
            double res = m_variate(e,val);
            double pdf = m_transform.template apply<0>(val,it);
            // this is a variable substitution (https://en.wikipedia.org/wiki/Probability_density_function#Multiple_variables)
            // q(it) = q(t(val)), t being the transform
            // q(it) = q(val) * |J_(t^-1)(t(val))| = q(val) / |J_t(val)|
            return res/pdf;
        }
        template<typename InputIterator>
        inline double pdf(InputIterator it) const {
            value_type val[dimension];
            double res = m_transform.template apply<1>(it,val);
            double pdf = m_variate.pdf(val);
            return res*pdf;
        }
        transformed_variate(const Transform& transform, const Variate& variate)
            : m_transform(transform), m_variate(variate) {}
    };

}; // namespace rjmcmc

#endif // RJMCMC_TRANSFORMED_VARIATE_HPP
