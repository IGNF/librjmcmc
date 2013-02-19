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

#ifndef MPP_KERNEL_HPP
#define MPP_KERNEL_HPP

#include "rjmcmc/random.hpp"
#include "rjmcmc/kernel/kernel.hpp"
#include "rjmcmc/kernel/transform.hpp"
#include "geometry/coordinates/coordinates.hpp"
#include <boost/random/variate_generator.hpp>
#include <algorithm> // copy_n

namespace marked_point_process {
    

    // single object type configuration for now...
    template<typename T, unsigned int N=1>
    class uniform_view
    {
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_smallint<> > die_type;
    public:
        enum { dimension =  coordinates_iterator<T>::dimension };
        typedef typename coordinates_iterator<T>::type iterator;
        
        template<typename Configuration, typename Modification, typename OutputIterator>
        inline double operator()(Configuration& c, Modification& modif, OutputIterator out) const
        {
            unsigned int n = c.size();
            if(n<N) return 0.;
            unsigned int denom=1;
            int d[N];
            for(unsigned int i=0;i<N;++i)
            {
                typename Configuration::iterator it = c.begin();
                die_type die(rjmcmc::random(), boost::uniform_smallint<>(0,n-1-i));
                d[i]=die();
                for(unsigned int j=0;j<i;++j) if(d[j]<=d[i]) ++d[i];
                std::advance(it, d[i]);

                modif.insert_death(it);
                const T& t = c[it];
                iterator coord_it  = coordinates_begin(t.rotate(die())); //TODO!!!
                //iterator coord_it  = coordinates_begin(t);
                for(unsigned int j=0; j<dimension; ++j) *out++ = *coord_it++;
                denom *= n-i;
            }
            return 1./denom;
        }
        template<typename Configuration, typename Modification, typename InputIterator>
        inline double inverse_pdf(Configuration& c, Modification& modif, InputIterator it) const
        {
            unsigned int n = c.size()+modif.delta_size();
            unsigned int denom=1;
            object_from_coordinates<T> creator;
            for(unsigned int i=1;i<=N;++i)
            {
                modif.insert_birth(creator(it));
                it+=dimension;
                denom *= n+i;
            }
            return 1./denom;
        }
    };
    
    
    template<typename T, typename V = typename rjmcmc::variate<coordinates_iterator<T>::dimension> >
    class uniform_birth
    {
    public:
        enum { dimension = coordinates_iterator<T>::dimension };
        typedef typename coordinates_iterator<T>::type iterator;
        typedef T object_type;
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

        result_type operator()(T& t) const
        {
            double val0[dimension];
            double val1[dimension];
            double phi = m_variate(val0);
            m_transform.apply(val0,val1);
            object_from_coordinates<T> creator;
            t = creator(val1);
            return phi*m_transform.inverse_abs_jacobian(val1);
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
            double J10 = m_transform.inverse(val1,val0);
            return m_variate.pdf(val0)*J10;
        }
    private:
        variate_type m_variate;
        transform_type m_transform;
    };


    template <typename birth_type>
            struct result_of_make_uniform_birth_death_kernel
    {
        typedef typename birth_type::object_type object_type;
        typedef typename birth_type::variate_type variate_type;
        enum { N = coordinates_iterator<object_type>::dimension };
        typedef rjmcmc::kernel<
                rjmcmc::null_view, marked_point_process::uniform_view<object_type>,
                variate_type, rjmcmc::variate<0>,
                rjmcmc::diagonal_affine_transform<N,double>
                > type;
    };

    template <typename birth_type>
            typename result_of_make_uniform_birth_death_kernel<birth_type>::type
            make_uniform_birth_death_kernel(const birth_type& b, double pbirth, double pdeath)
    {
        typedef result_of_make_uniform_birth_death_kernel<birth_type> result_type;
        typedef typename result_type::object_type object_type;
        return typename result_type::type(
                rjmcmc::null_view(),   uniform_view<object_type>(),
                b.variate(),           rjmcmc::variate<0>(),
                b.transform(),
                pbirth,                pdeath);
    }

    template <typename Transform, typename T = typename Transform::value_type>
                                               struct result_of_make_uniform_modification_kernel
                                               {
        enum { N = Transform::dimension-coordinates_iterator<T>::dimension };
        typedef uniform_view<T>    view_type;
        typedef rjmcmc::variate<N> variate_type;
        typedef rjmcmc::kernel<view_type,view_type,variate_type,variate_type,Transform> type;
    };

    template <typename Transform>
            typename result_of_make_uniform_modification_kernel<Transform>::type
            make_uniform_modification_kernel(const Transform& t, double p)
    {
        typedef result_of_make_uniform_modification_kernel<Transform> res;
        typename res::view_type view;
        typename res::variate_type variate;
        return typename res::type(view,view,variate,variate,t, p*0.5, p*0.5);
    }

}; // namespace rjmcmc

#endif // MPP_KERNEL_HPP
