#ifndef TRANSFORM_KERNELS_HPP_
#define TRANSFORM_KERNELS_HPP_

#include "rjmcmc/random.hpp"
#include "rjmcmc/kernel/transform.hpp"
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/algorithm.hpp>

namespace rjmcmc {
    
    namespace detail {
        template<typename U> class transform_entry : public boost::fusion::pair<U,rjmcmc::diagonal_affine_transform<coordinates_iterator<U>::size,double> > {};

        template<typename U>
        class transform_map : public boost::fusion::map<transform_entry<U> > {};

        template<BOOST_VARIANT_ENUM_PARAMS(typename U)>
        class transform_map<boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)> >
            : public boost::fusion::result_of::as_map<
            typename boost::mpl::transform<typename boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)>::types, transform_entry<boost::mpl::_> >::type>::type
        {
        };

        template< typename Searchspace >
        struct transform_init {
            Searchspace m_searchspace;
            transform_init(const Searchspace& searchspace) :
                    m_searchspace(searchspace)
            {}

            template<typename T> void operator() (transform_entry<T>& t) const
            {
                typedef typename coordinates_iterator<T>::type iterator;
                iterator bmin  = coordinates_begin(m_searchspace.template min<T>());
                iterator bmax  = coordinates_begin(m_searchspace.template max<T>());
                iterator m = bmin;
                static const unsigned int size = coordinates_iterator<T>::size;
                double d[size];
                double *it = d, *end = it+size;
                for(;it!=end; ++it, ++bmin, ++bmax)
                {
                    *it = (*bmax) - (*bmin)+50;
                };
                t.second = rjmcmc::diagonal_affine_transform<size,double>(d,m);
            }
        };

    }

    template< typename Searchspace >
    class generator {
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_real<> > die_type;
        mutable die_type m_die;
        Searchspace m_searchspace;
        
        typedef typename Searchspace::value_type value_type;
        typedef detail::transform_map<value_type> transform_type;
        transform_type m_transform;
        
    public:
        generator(const Searchspace& searchspace) :
                m_die(rjmcmc::random(), boost::uniform_real<>(0,1)),
                m_searchspace(searchspace)
        {
            boost::fusion::for_each(m_transform,detail::transform_init<Searchspace>(m_searchspace));
        }
        
        typedef double result_type;
        
        template<typename T> result_type operator()(T &t) const
        {
            static const unsigned int size = coordinates_iterator<T>::size;
            double in [size];
            double out[size];
            object_from_coordinates<T> creator;
            do {
                for(double *it = in; it!=in+size; ++it) *it = m_die();
                boost::fusion::at_key<T>(m_transform).apply(in,out);
                t = creator(out);
            } while (!m_searchspace.inside(t)); // rejection sampling...
            return boost::fusion::at_key<T>(m_transform).abs_jacobian(out);
        }
        
        struct pdf_visitor {
            typedef generator::result_type result_type;
            const generator& m_generator;
            template<typename T> inline result_type operator()(const T &t) const { return m_generator.pdf(t); }
            pdf_visitor(const generator& g) : m_generator(g) {}
        };
        inline pdf_visitor pdf() const { return pdf_visitor(*this); }
        
        template<typename T>
        inline result_type pdf(const T &t) const
        {
            if(!m_searchspace.inside(t)) return 0;
            return boost::fusion::at_key<T>(m_transform).abs_jacobian(coordinates_begin(t));
        }
    };
    

    
} // namespace rjmcmc

#endif /*TRANSFORM_KERNELS_HPP_*/
