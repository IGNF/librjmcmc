#ifndef KERNELS_HPP_
#define KERNELS_HPP_

#include "rjmcmc/random.hpp"
#include "rjmcmc/kernel/transform.hpp"
#include <boost/array.hpp>
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

    template<
            typename Transform,
            typename Input  = typename Transform::input_type,
            typename Output = typename Transform::input_type>
        class transform_modifier
                              {
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_real<> > die_type;
        mutable die_type m_die;
        Transform   m_transform;

                              public:
        modifier(const Transform& transform) :
                m_die(rjmcmc::random(), boost::uniform_real<>(0,1)),
                m_transform(transform) {}

        typedef double result_type;
        typedef Input  input_type;
        typedef Output output_type;
        typedef typename coordinates_iterator<input_type>::type iterator;

        double operator()(const input_type &input, output_type &output) const
        {
            static const unsigned int size = Transform::size;
            static const unsigned int in_size  = coordinates_iterator< input_type>::size;
            static const unsigned int out_size = coordinates_iterator<output_type>::size;
            double in [Transform::size];
            double out[Transform::size];
            iterator input_it  = coordinates_begin(input);
            for(double *it = in; it!=in+in_size; ++it, ++input_it) *it = *input_it;
            for(double *it = in+in_size; it!=in+size; ++it) *it = m_die();
            m_transform.apply(in,out);
            object_from_coordinates<output_type> creator;
            output = creator(out);
            return m_transform.abs_jacobian(in); // *probability(out+out_size,out+size)/probability(in+in_size,in+size);
            // |dTmn(theta_m,u_mn)/d(theta_m,u_mn)| * phi(v_mn) / phi(u_mn)
        }
    };

} // namespace rjmcmc

#endif /*KERNELS_HPP_*/
