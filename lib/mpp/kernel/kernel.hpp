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
#include <boost/random/variate_generator.hpp>

namespace marked_point_process {
    
    // single object type configuration for now...
    template<typename T>
    class uniform_view
    {
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_smallint<> > die_type;
    public:
        enum { dimension =  coordinates_iterator<T>::dimension };
        typedef typename coordinates_iterator<T>::type iterator;
        
        template<typename Configuration, typename Modification, typename OutputIterator>
        inline double operator()(Configuration& c, Modification& modif, OutputIterator out) const
        {
            //unsigned int n = c.size<T>();
            unsigned int n = c.size();
            if(!n) return 0.;
            //typename Configuration::iterator<T> it = c.begin<T>();
            typename Configuration::iterator it = c.begin();
            die_type die(rjmcmc::random(), boost::uniform_smallint<>(0,n-1));
            std::advance(it, die());
            
            /*
            // noop
            const T *t = rjmcmc::variant_get<T>(&c[it]);
            if(!t) return 0;
            */
            
            modif.insert_death(it);
            const T& t = c[it];
//            iterator coord_it  = coordinates_begin(t.rotate(die()));
            iterator coord_it  = coordinates_begin(t);
            for(unsigned int i=0;i<dimension;++i) *out++ = *coord_it++;
            
            // return 1./c.size<T>();
            return 1./c.size();
        }
        template<typename Configuration, typename Modification, typename InputIterator>
        inline double inverse_pdf(Configuration& c, Modification& modif, InputIterator it) const
        {
            object_from_coordinates<T> creator;
            modif.insert_birth(creator(it));
            //            return 1./(c.size<T>()+modif.delta_size<T>());
            return 1./(c.size()+modif.delta_size());
        }
    };
    
    
    template<typename T>
    class uniform_birth
    {
    public:
        enum { dimension = coordinates_iterator<T>::dimension };
        typedef typename coordinates_iterator<T>::type iterator;
        typedef typename rjmcmc::variate<dimension> variate_type;
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
            int dummy_config, dummy_modif;
            double val0[dimension];
            double val1[dimension];
            double phi01 = m_variate(dummy_config,dummy_modif,val0);
            m_transform.apply(val0,val1);
            object_from_coordinates<T> creator;
            t = creator(val1);
            return  phi01*pdf(t);
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
            iterator it(coordinates_begin(t));
            double val[dimension];
            m_transform.inverse(it,val);
            return m_variate.pdf(val)*m_transform.abs_jacobian(val);
        }
    private:
        variate_type m_variate;
        transform_type m_transform;
    };


    template <typename T>
            struct result_of_make_uniform_birth_death_kernel
    {
        enum { N = coordinates_iterator<T>::dimension };
        typedef rjmcmc::kernel<
                rjmcmc::null_view, marked_point_process::uniform_view<T>,
                rjmcmc::variate<N>,rjmcmc::variate<0>,
                rjmcmc::diagonal_affine_transform<N,double>
                > type;
    };


    template <typename Transform, typename T = typename Transform::value_type>
            struct result_of_make_uniform_modification_kernel
    {
        enum { N = Transform::dimension-coordinates_iterator<T>::dimension };
        typedef uniform_view<T>    view_type;
        typedef rjmcmc::variate<N> variate_type;
        typedef rjmcmc::kernel<view_type,view_type,variate_type,variate_type,Transform> type;
    };

    template <typename T>
            typename result_of_make_uniform_birth_death_kernel<T>::type
            make_uniform_birth_death_kernel(const uniform_birth<T>& b, double pbirth, double pdeath)
    {
        return typename result_of_make_uniform_birth_death_kernel<T>::type(
                rjmcmc::null_view(),   uniform_view<T>(),
                b.variate(),           rjmcmc::variate<0>(),
                b.transform(),
                pbirth,                pdeath);
    }

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
