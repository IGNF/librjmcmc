
#ifndef MPP_KERNEL_HPP_
#define MPP_KERNEL_HPP_

#include "rjmcmc/random.hpp"
#include "rjmcmc/kernel/binary_kernel.hpp"
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
            iterator coord_it  = coordinates_begin(t.rotate(die()));
          //   iterator coord_it  = coordinates_begin(t);
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
            typedef uniform_birth::result_type result_type;
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


    template <typename T, typename Transform>
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

    template <typename T, typename Transform>
            typename result_of_make_uniform_modification_kernel<T,Transform>::type
            make_uniform_modification_kernel(const Transform& t, double p)
    {
        typedef result_of_make_uniform_modification_kernel<T,Transform> res;
        typename res::view_type view;
        typename res::variate_type variate;
        return typename res::type(view,view,variate,variate,t, p*0.5, p*0.5);
    }

#if 0
    template<typename Modifier>
    class modification_kernel : public rjmcmc::unary_kernel
    {
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_smallint<> > die_type;
        Modifier m_modifier;
    public:
        modification_kernel(const Modifier& m, double p=1) : rjmcmc::unary_kernel(p), m_modifier(m) {}
        
        inline const char* name(unsigned int) const { return "modif"; } //m_modifier.name(); }
        
        template<typename Configuration, typename Modification>
        double operator()(double, Configuration& c, Modification& modif) const
        {
            modif.clear();
            if(c.empty()) return 1.;
            typedef typename Configuration::value_type T;
            typedef typename Configuration::iterator iterator;
            
            /*
            typedef typename variant_pairs<T>::type MT;
            MT out, in;
            
            int n = c.size();
            die_type ddie(rjmcmc::random(), boost::uniform_smallint<>(1,2));
            die_type cdie(rjmcmc::random(), boost::uniform_smallint<>(0,n-1));
            iterator it = c.begin();
            int i = cdie();
            std::advance(it, i);
            int num_deaths = std::min(n,ddie());
            if(num_deaths ==1) {
                modif.insert_death(it);
                in = c[it];
            } else {
                int i2;
                do { i2 = cdie(); } while(i==i2);
                iterator it2 = c.begin();
                std::advance(it2, cdie());
                modif.insert_death(it);
                modif.insert_death(it2);
                make_variant_pair<T> vmp;
                in = apply_visitor(vmp,c[it],c[it2]);
            }
            
            random_variant_init(out);
            */
            typedef typename Modifier::input_type  input_type;
            typedef typename Modifier::output_type output_type;
            
            int n = c.size();
            
            // TODO sample the input_type only and not all objects
            die_type cdie(rjmcmc::random(), boost::uniform_smallint<>(0,n-1));
            iterator it = c.begin();
            std::advance(it, cdie());
            const input_type *in = rjmcmc::variant_get<input_type>(&c[it]);
            if(!in) return 0;
            output_type out;
            
            double green_ratio = rjmcmc::apply_visitor(m_modifier,*in,out);
            modif.insert_death(it);
            modif.insert_birth(out);
            return green_ratio;
        }
    };
#endif
}; // namespace rjmcmc

#endif // MPP_KERNEL_HPP_
