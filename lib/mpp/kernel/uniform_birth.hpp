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
    class object_birth
    {
    public:
        enum { dimension = coordinates_iterator<T>::dimension };
        typedef typename coordinates_iterator<T>::type iterator;
        typedef T value_type;
        typedef V variate_type;

        const variate_type& variate() const { return m_variate; }


        object_birth(const V& v) : m_variate(v) {}

        typedef double result_type;

        // used only be direct_sampler
        template<typename Engine>
        result_type operator()(Engine& e, T& t) const
        {
            double val[dimension];
            double phi = m_variate(e, val);
            object_from_coordinates<T> creator;
            t = creator(val);
            return phi;
        }

        struct pdf_visitor {
            typedef typename object_birth<T>::result_type result_type;
            const object_birth& m_object_birth;
            inline result_type operator()(const T &t) const { return m_object_birth.pdf(t); }
            pdf_visitor(const object_birth& g) : m_object_birth(g) {}
        };
        inline pdf_visitor pdf() const { return pdf_visitor(*this); }

        inline result_type pdf(const T &t) const
        {
            iterator val(coordinates_begin(t));
            return m_variate.pdf(val);
        }
    private:
        variate_type m_variate;
    };


}


#include "rjmcmc/kernel/transformed_variate.hpp"

namespace marked_point_process {
    template<typename T, typename V = typename rjmcmc::variate<coordinates_iterator<T>::dimension> >
    class uniform_birth : public object_birth<
            T,
            rjmcmc::transformed_variate<
                rjmcmc::diagonal_affine_transform<coordinates_iterator<T>::dimension,double>,
                V
            >
          >

    {
    public:
        enum { dimension = coordinates_iterator<T>::dimension };
        typedef typename coordinates_iterator<T>::type iterator;
        typedef T value_type;
        typedef double result_type;
        typedef typename rjmcmc::identity_transform<dimension,double> transform_type;
        typedef typename rjmcmc::diagonal_affine_transform<dimension,double> variate_transform_type;
        typedef typename rjmcmc::transformed_variate<variate_transform_type,V> variate_type;
        typedef object_birth<T,variate_type> base_type;
        
        transform_type transform() const { return transform_type(); }

    private:
        static variate_type get_variate(const T& a, const T& b, V v)
        {
            iterator ait  = coordinates_begin(a) , m = ait;
            iterator bit  = coordinates_begin(b);
            double d[dimension];
            double *it = d;
            for(unsigned int i=0;i<dimension; ++i) *it++ = (*bit++) - (*ait++);
            variate_transform_type trans(d,m);
            return variate_type(trans,v);
        }

    public:
        uniform_birth(const T& a, const T& b, V v = V()) : base_type(get_variate(a,b,v)) {}

    };




}; // namespace marked_point_process

#endif // MPP_UNIFORM_BIRTH_HPP
