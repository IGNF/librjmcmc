#ifndef KERNELS_HPP_
#define KERNELS_HPP_

#include "rjmcmc/random.hpp"
#include "rjmcmc/kernel/transform.hpp"
#include <boost/array.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/algorithm.hpp>

namespace rjmcmc {

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
                *it = (*bmax) - (*bmin);
            };
            t.second = rjmcmc::diagonal_affine_transform<size,double>(d,m);
        }
    };

    template< typename Searchspace >
    class generator {
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_real<> > die_type;
        mutable die_type m_die;
        Searchspace m_searchspace;

        typedef typename Searchspace::value_type value_type;
        typedef transform_map<value_type> transform_type;
        transform_type m_transform;

    public:
        generator(const Searchspace& searchspace) :
                m_die(rjmcmc::random(), boost::uniform_real<>(0,1)),
                m_searchspace(searchspace)
        {
            boost::fusion::for_each(m_transform,transform_init<Searchspace>(m_searchspace));
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

    template<typename Transform,  typename Input,  typename Output>
    class modifier {
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

/*        template<typename T0, typename T1>
        double operator()(const T0 &t0, T1 &t1) const {
            return 0;
        }
*/
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

        /*
    template< typename Searchspace >
    class modifier {
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_real<> > double_die_type;
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_smallint<> > int_die_type;
        mutable double_die_type m_dief;
        mutable int_die_type   m_die4;
        double m_p_translation;
        double m_dx, m_dy;
        Searchspace m_searchspace;
    public:
        modifier(const Searchspace& searchspace) :
                m_dief(rjmcmc::random(), boost::uniform_real<>(0,1)),
                m_die4(rjmcmc::random(), boost::uniform_smallint<>(0,3)),
                m_p_translation(0.5),
                m_dx(20), m_dy(20),
                m_searchspace(searchspace) {}

        typedef double result_type;

        template<typename T0, typename T1>
        double operator()(const T0 &t0, T1 &t1) const {
            return 0;
        }

#ifdef GEOMETRY_RECTANGLE_2_H
        template<typename K>
        result_type operator()(const geometry::Rectangle_2<K> &t, geometry::Rectangle_2<K> &res ) const
        {
            typename K::Point_2 c = t.center();
            typename K::Vector_2 n = t.normal();
            typename K::FT r = t.ratio();
            typename K::Vector_2 m(-r*n.y(),r*n.x());
            do {
                typename K::FT f = exp(0.5-m_dief());
                res = Rectangle_2(c+m*(1-f), n,f*r);
            } while (!m_searchspace(res));
            return 1.; // TODO
        }


        // translates edge(i) linearly from edge(i+2) with f : 0->degenerate, 1->identity...
        // assert(!is_degenerate());
        switch (i%4) {
        case  0: return Rectangle_2(c+(1-f)*m, n,f*r);
        case  1: return Rectangle_2(c-(1-f)*n, m,f/r);
        case  2: return Rectangle_2(c-(1-f)*m,-n,f*r);
        default: return Rectangle_2(c+(1-f)*n,-m,f/r);
        }
        template<typename K> result_type operator()(
        const geometry::Rectangle_2<K> &r,
        geometry::Rectangle_2<K> &res
        ) const
        {
                res = r.scaled_edge(m_die4(),exp(0.5-m_dief()));
            return 1.; // TODO
        }

  template<typename K> result_type operator()(
                                               const geometry::Rectangle_2<K> &r,
                                               std::pair<geometry::Rectangle_2<K>,geometry::Rectangle_2<K> > &res
                                             ) const
  {
    int i = m_die4();
    double f = m_dief();
    double g = m_dief()*(1-f);
    res.first  = r.scaled_edge(i  ,f);
    res.second = r.scaled_edge(i+2,g);
    return 1.; // TODO
  }

  template<typename K> result_type operator()(
                                               const std::pair<geometry::Rectangle_2<K>,geometry::Rectangle_2<K> > &r,
                                               geometry::Rectangle_2<K> &res
                                             ) const
  {
    res = r.first.rotate(m_die4()).merge(r.second.rotate(m_die4()));
    return 1.; // TODO
  }
#endif // GEOMETRY_RECTANGLE_2_H

#ifdef GEOMETRY_CIRCLE_2_H
  template<typename K> result_type operator()(
                                               const geometry::Circle_2<K> &c,
                                               geometry::Circle_2<K> &res
                                             ) const
  {
    typedef typename K::Vector_2 Vector_2;
    if(m_dief()<m_p_translation) {
      Vector_2 v(m_dx*(m_dief()-0.5),m_dy*(m_dief()-0.5));
      double d2 = v.squared_length();
      double d = sqrt(d2);
      if (m_die4()%2 && c.squared_radius()>d2) d = -d;
      res = geometry::Circle_2<K>(c.center()+v,geometry::radius(c)+d);
    } else {
      res = geometry::Circle_2<K>(c.center(),geometry::radius(c)*exp(0.5-m_dief()));
    }
    return 1.; // TODO
  }
#endif // GEOMETRY_CIRCLE_2_H

#if defined(GEOMETRY_CIRCLE_2_H) && defined(GEOMETRY_RECTANGLE_2_H)

  template<typename K> result_type operator()(
                                               const geometry::Rectangle_2<K> &r,
                                               geometry::Circle_2<K> &c
                                             ) const
  {
    double radius = sqrt(r.normal().squared_length());
    double ratio  = std::abs(r.ratio());
    if(ratio<1) radius *= ratio;
    c = geometry::Circle_2<K>(r.center(),radius);
    return 1.; // TODO
  }

  template<typename K> result_type operator()(
                                               const geometry::Circle_2<K> &c,
                                               geometry::Rectangle_2<K> &r
                                             ) const
  {
    typedef typename K::Vector_2 Vector_2;
    Vector_2 v((2*m_dief()-1),(2*m_dief()-1));
    v = v*(geometry::radius(c)/sqrt(v.squared_length()));
    r = geometry::Rectangle_2<K>(c.center(),v,1);
    return 1.; // TODO
  }

  template<typename K> result_type operator()(
                                               const geometry::Rectangle_2<K> &r,
                                               std::pair<geometry::Rectangle_2<K>,geometry::Circle_2<K> > &res
                                             ) const
  {
    typedef typename K::Vector_2 Vector_2;
                // todo: verifier que ca fait pas n'importe quoi...
    int i = m_die4();
    double f = m_dief();
    double g = m_dief();
    double h = m_dief();
    Vector_2 n = r.normal(i)*std::min(1.,std::abs(r.ratio()));
    if(i%2) n = n*(1./r.ratio());
    Vector_2 v(-n.x()+n.y(),-n.y()-n.x());
    res.first  = r.scaled_edge(i,f).scaled_edge(i+1,g);
    res.second = geometry::Circle_2<K>(r[i+3]+v*h,h*std::sqrt(n.squared_length()));
    return 1.; // TODO
  }
#endif // defined(GEOMETRY_CIRCLE_2_H) && defined(GEOMETRY_RECTANGLE_2_H)
*/
    };

} // namespace rjmcmc

#endif /*KERNELS_HPP_*/
