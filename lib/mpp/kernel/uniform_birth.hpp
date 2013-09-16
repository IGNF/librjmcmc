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
encouraged to load and test thesoftware's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and, more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

***********************************************************************/

#ifndef MPP_UNIFORM_BIRTH_HPP
#define MPP_UNIFORM_BIRTH_HPP

#include "rjmcmc/kernel/transform.hpp"
#include "geometry/coordinates/coordinates.hpp"

#include "rjmcmc/kernel/variate.hpp"

namespace marked_point_process {
    

    template<typename T, typename V = typename rjmcmc::variate<coordinates_iterator<T>::dimension> >
    class uniform_birth
    {
    public:
        enum { dimension = coordinates_iterator<T>::dimension };
        typedef typename coordinates_iterator<T>::type iterator;
        typedef T value_type;
        typedef V variate_type;
        typedef typename rjmcmc::diagonal_affine_transform<dimension,double> transform_type;

        const variate_type& variate() const { return m_variate; }
        const transform_type& transform() const { return m_transform; }


        uniform_birth(const T& a, const T& b)
        {
            iterator ait  = coordinates_begin(a) , m = ait;
            iterator bit  = coordinates_begin(b);
            double d[dimension];
            double *it = d;
            for(unsigned int i=0;i<dimension; ++i) *it++ = (*bit++) - (*ait++);
            m_transform = rjmcmc::diagonal_affine_transform<dimension,double>(d,m);
        }
        typedef double result_type;

        // used only be direct_sampler
        template<typename Engine>
        result_type operator()(Engine& e, T& t) const
        {
            double val0[dimension];
            double val1[dimension];
            double phi = m_variate(e, val0);
            m_transform.template apply<0>(val0,val1);
            object_from_coordinates<T> creator;
            t = creator(val1);
            return phi*m_transform.template abs_jacobian<1>(val1);
        }

        struct pdf_visitor {
            typedef typename uniform_birth<T>::result_type result_type;
            const uniform_birth& m_uniform_birth;
            inline result_type operator()(const T &t) const { return m_uniform_birth.pdf(t); }
            pdf_visitor(const uniform_birth& g) : m_uniform_birth(g) {}
        };
        inline pdf_visitor pdf() const { return pdf_visitor(*this); }

        inline result_type pdf(const T &t) const
        {
            iterator val1(coordinates_begin(t));
            double val0[dimension];
            double J10 = m_transform.template apply<1>(val1,val0);
            return m_variate.pdf(val0)*J10;
        }
    private:
        variate_type m_variate;
        transform_type m_transform;
    };

}; // namespace marked_point_process

#endif // MPP_UNIFORM_BIRTH_HPP
