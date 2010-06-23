#ifndef __RJMCMC_SAMPLER_HPP__
#define __RJMCMC_SAMPLER_HPP__

#include "rjmcmc/variant.hpp"
#include "rjmcmc/random.hpp"
#include "rjmcmc/random_variant_init.hpp"
#include <boost/tuple/tuple.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/insert_range.hpp>
#include <boost/mpl/pop_front.hpp>

namespace rjmcmc {

    // apply a random function object within a tuple
    // x should originaly be drawn uniformly in [0,1]
    // t is the tuple of function objects

#if USE_VARIADIC_TEMPLATES

     template <typename T, typename C, typename M> struct random_modification {
         random_modification(double p) : x(p), R(0), s(0) {}
            template <int i> void apply(T& t, C &c, M &m)
            {
                double xt = x - std::get<i>(t).probability();
                if(xt<0) {
                    R=std::get<i>(t)(x,c,m);
                    s+=std::get<i>(t).kernel_id();
                } else {
                    x=xt;
                    typedef typename std::tuple_element<i,T>::type Ti;
                    s+=Ti::size;
                    apply<i+1>(t,c,m);
                }
            }

            inline unsigned int operator()(T& t, C &c, M &m) { return apply<0>(t,c,m); }
            double x;
            double R;
            unsigned int s;
        }

          template<typename T> struct kernel_traits {
              enum { size = 0 };
          };
          template <class H, class... T> struct kernel_traits < std::tuple<H, T...> > {
              enum { size = H::size + kernel_traits<std::tuple<T...> >::size };
          };


          template <typename T, typename C, typename M>
                  template <> void random_modification<T,C,M>::apply<std::tuple_size<T>::value>(T&, C &c, M &m)
          {
              R=0;
              return s;
          }

#else

    namespace internal {

template<typename C, typename M>
inline unsigned int random_apply(double x, const boost::tuples::null_type&, C &c, M &m, double& R, unsigned int i=0) {
	R=0;
	return i;
}

template <class H, class T, typename C, typename M>
inline unsigned int random_apply(double x, boost::tuples::cons<H, T>& t, C &c, M &m, double& R, unsigned int i=0) {
	double xt = x - t.get_head().probability();
	if(xt<0) { R=t.get_head()(x,c,m); return i+t.get_head().kernel_id(); }
	return random_apply(xt,t.get_tail(),c,m,R,i+H::size);
}

template<typename T> struct cons_list_kernel_traits {
	enum { size = 0 };
};

template <class H, class T> struct cons_list_kernel_traits < boost::tuples::cons<H, T> > {
	enum { size = H::size + cons_list_kernel_traits<T>::size };
};

} //  namespace internal

template<typename T> struct kernel_traits {
	enum { size = internal::cons_list_kernel_traits<typename T::inherited>::size };
};

#endif


template <typename T,typename U>
struct std_pair { typedef std::pair<T,U> type; };

template <typename T>
struct variant_pairs { typedef boost::variant<T,std::pair<T,T> > type; };

template<typename S, typename I>
struct variant_pairs_detail {
	typedef typename boost::mpl::pop_front<I>::type tail;
	typedef typename boost::mpl::front<I>::type     head;
	typedef typename variant_pairs_detail<S,tail>::type T;
	typedef typename boost::mpl::end<T>::type T_end;
	typedef std_pair<head,boost::mpl::_1> make_std_pair;
	typedef typename boost::mpl::transform<S,make_std_pair>::type pair_types;
	typedef typename boost::mpl::insert_range<T,T_end,pair_types>::type type;
};

template<typename S>
struct variant_pairs_detail<S,boost::mpl::l_end> {
	typedef S type;
};

template<BOOST_VARIANT_ENUM_PARAMS(typename T)>
struct variant_pairs<boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> > {
	typedef boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> T;
	typedef typename T::types types;

/* // boost::variant<Rectangle_2,Circle_2> should return :
	typedef boost::variant<Rectangle_2,Circle_2,
		std::pair<Rectangle_2,Rectangle_2>,
		std::pair<Circle_2,Rectangle_2>,
		std::pair<Circle_2,Circle_2>,
		std::pair<Rectangle_2,Circle_2> > type;
*/
	typedef typename variant_pairs_detail<types,types>::type pairs;
	typedef typename boost::make_variant_over<pairs>::type type;
};

template<typename V> 
struct make_variant_pair
{
	typedef typename variant_pairs<V>::type result_type;
	template<typename T,typename U>
	inline result_type operator()(const T& t, const U& u) const {
		return result_type(std::make_pair(t,u));
	}
};

//////////////////


template<typename Kernel0, typename Kernel1> class binary_kernel {
	double m_p[2], m_p_sum;
	Kernel0 m_kernel0;
	Kernel1 m_kernel1;
	mutable unsigned int m_kernel_id;
public:
	enum { size = 2 };
	inline unsigned int kernel_id() const { return m_kernel_id; }
	binary_kernel(const Kernel0& k0, const Kernel1& k1, double p=1.) : 
		m_kernel0(k0), m_kernel1(k1), m_p_sum(p) {
		m_p[0]=m_p[1]=p*0.5;
	}
	binary_kernel(const Kernel0& k0, const Kernel1& k1, double p0, double p1) :
		m_kernel0(k0), m_kernel1(k1), m_p_sum(p0+p1) {
		m_p[0]=p0; m_p[1]=p1;
	}
	inline void probability(double p_sum) { 
		double r = p_sum/m_p_sum;
		m_p[0] *= r; m_p[1] *= r;
		m_p_sum = p_sum;
	}
	inline double probability() const { return m_p_sum; }
	inline void probability(unsigned int i, double p) {
		m_p_sum += p-m_p[i];
		m_p[i] = p;
	}
	inline double probability(unsigned int i) const { return m_p[i]; }
	template<typename Configuration, typename Modification>
	double operator()(double p, Configuration& c, Modification& modif) const
	{
		if(p<m_p[0]) {
			m_kernel_id = 0;
			double x = m_p[0]*m_kernel0    (c,modif);
			double y = m_p[1]*m_kernel1.pdf(c,modif);
			return y/x;
		} else {
			m_kernel_id = 1;
			double x = m_p[1]*m_kernel1    (c,modif);
			double y = m_p[0]*m_kernel0.pdf(c,modif);
			return y/x;
		}
	}
};


class uniform_death_kernel {
	typedef boost::variate_generator<rjmcmc::generator&, boost::uniform_smallint<> > die_type;
	double m_poisson;
public:
	uniform_death_kernel(double poisson) : m_poisson(poisson) {}
	template<typename Configuration, typename Modification>
	double operator()(Configuration& c, Modification& modif) const
	{
		modif.clear();
		if(c.empty()) return 1.;
		typename Configuration::iterator it = c.begin();
		die_type die(random(), boost::uniform_smallint<>(0,c.size()-1));
		std::advance(it, die());
		modif.insert_death(it);
		return m_poisson/c.size();
	}
	template<typename Configuration, typename Modification>
	double pdf(const Configuration& c, const Modification& modif) const
	{
		if(modif.birth_size()!=1 || modif.death_size()!=0) return 0.;
		return m_poisson/(c.size()+1);
	}
};

template<typename Generator>
class uniform_birth_kernel {
	Generator m_generator;
public:
	uniform_birth_kernel(const Generator& generator) : 
		m_generator(generator) {}

	template<typename Configuration, typename Modification>
	double operator()(Configuration& c, Modification& modif) const
	{
		typedef typename Configuration::value_type T;
		T res;
		random_variant_init(res);
		double p = rjmcmc::apply_visitor(m_generator,res);
		modif.insert_birth(res);
		return p;
	}

	template<typename Configuration, typename Modification>
	double pdf(const Configuration& c, const Modification& modif) const
	{
		if(modif.birth_size()!=0 || modif.death_size()!=1) return 0.;
		return rjmcmc::apply_visitor(m_generator.pdf(),c[*modif.death_begin()]);
	}
};

class unary_kernel {
	double m_p;
public:
	enum { size = 1 };
	unary_kernel(double p=1.) : m_p(p) {}
	inline unsigned int kernel_id() const { return 0; }
	inline void probability(double p) { m_p = p; }
	inline double probability() const { return m_p; }
	inline void probability(unsigned int, double p) { m_p = p; }
	inline double probability(unsigned int) const { return m_p; }
};


template<typename Modifier>
class modification_kernel : public unary_kernel {
	typedef boost::variate_generator<rjmcmc::generator&, boost::uniform_smallint<> > die_type;
	double   m_poisson;
	Modifier m_modifier;
public:
	modification_kernel(const Modifier& m, double poisson, double p=1) : unary_kernel(p), m_poisson(poisson), m_modifier(m) {}

	template<typename Configuration, typename Modification>
	double operator()(double p, Configuration& c, Modification& modif) const
	{
		modif.clear();
		if(c.empty()) return 1.;
		typedef typename Configuration::value_type T;
		typedef typename Configuration::iterator iterator;
		typedef typename variant_pairs<T>::type MT;
		MT out, in;

		int n = c.size();
		die_type ddie(random(), boost::uniform_smallint<>(1,2));
		die_type cdie(random(), boost::uniform_smallint<>(0,n-1));
		iterator it = c.begin();
		std::advance(it, cdie());
		int num_deaths = std::min(n,ddie());
		if(num_deaths ==1) {
			modif.insert_death(it);
			in = c[it];
		} else {
			iterator it2;
			do {
				it2 = c.begin();
				std::advance(it2, cdie());
			} while (it==it2);
			modif.insert_death(it2);
			make_variant_pair<T> vmp;
			in = rjmcmc::apply_visitor(vmp,c[it],c[it2]);
		}

		random_variant_init(out);
		double green_ratio = rjmcmc::apply_visitor(m_modifier,in,out);
		rjmcmc::apply_visitor(modif.birth_inserter(),out);

		switch(modif.birth_size()-num_deaths) {
		case -1 : return green_ratio * m_poisson /n;
		case  0 : return green_ratio;
		case  1 : 
		default : return green_ratio *(n+1)/m_poisson;
		}
	}
};

#if USE_VARIADIC_TEMPLATES

// Derived: Curiously recurring template pattern
template<typename Derived, typename... K>
class sampler
{
public:
        typedef std::tuple<K...> Kernels;
        sampler(const K&... k) : m_kernel(k...),
		m_die(random(), boost::uniform_real<>(0,1)) {}

        enum { size = std::tuple_size<Kernels>::value };
#else

#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_params_with_a_default.hpp>
#ifndef RJMCMC_SAMPLER_LIMIT_TYPES
	#define RJMCMC_SAMPLER_LIMIT_TYPES 2
#endif // RJMCMC_SAMPLER_LIMIT_TYPES
#define RJMCMC_SAMPLER_ENUM_PARAMS(x) \
	BOOST_PP_ENUM_PARAMS(RJMCMC_SAMPLER_LIMIT_TYPES,x)
#define RJMCMC_SAMPLER_ENUM_BINARY_PARAMS(x,y) \
	BOOST_PP_ENUM_BINARY_PARAMS(RJMCMC_SAMPLER_LIMIT_TYPES,x,y)
#define RJMCMC_SAMPLER_ENUM_PARAMS_WITH_A_DEFAULT(x) \
	BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(RJMCMC_SAMPLER_LIMIT_TYPES,x,boost::tuples::null_type)
#define RJMCMC_SAMPLER_ENUM(x) \
	BOOST_PP_ENUM(RJMCMC_SAMPLER_LIMIT_TYPES,x,nil)
#define RJMCMC_SAMPLER_ARG(z,n,_) const K##n& k##n = K##n()

#define RJMCMC_SAMPLER_ARGS RJMCMC_SAMPLER_ENUM(RJMCMC_SAMPLER_ARG)
#define RJMCMC_SAMPLER_TYPENAMES RJMCMC_SAMPLER_ENUM_PARAMS_WITH_A_DEFAULT(typename K)
#define RJMCMC_SAMPLER_TYPES RJMCMC_SAMPLER_ENUM_PARAMS(K)
#define RJMCMC_SAMPLER_PARAMS RJMCMC_SAMPLER_ENUM_PARAMS(k)

// Derived: Curiously recurring template pattern
template<typename Derived, RJMCMC_SAMPLER_TYPENAMES >
class sampler_base
{
public:
       typedef boost::tuple<RJMCMC_SAMPLER_TYPES> Kernels;
        sampler_base(RJMCMC_SAMPLER_ARGS) :
                m_kernel(RJMCMC_SAMPLER_PARAMS),
		m_die(random(), boost::uniform_real<>(0,1)) {}

        enum { size = boost::tuples::length<Kernels>::value };
#endif
        typedef boost::variate_generator<rjmcmc::generator&, boost::uniform_real<> > die_t;

    // main sampling function
    template<typename Configuration>
    void operator()(Configuration &c, double temp)
    {
        typename Configuration::modification modif;
        m_temperature = temp;
        m_kernel_id   = internal::random_apply(m_die(),m_kernel,c,modif,m_green_ratio);
        if(m_green_ratio<=0) {
            m_delta   =0;
            m_accepted=false;
            return;
        }
        m_delta       = c.delta_energy(modif);
        m_acceptance  = static_cast<Derived*>(this)->acceptance_probability();
        m_accepted    = ( m_die() < m_acceptance );
        if (m_accepted) c.apply(modif);
    }

	template<unsigned int N>
	inline const typename boost::tuples::element<N, Kernels>::type& kernel() const {
		return boost::tuples::get<N>(m_kernel);
	}
	template<unsigned int N>
	inline typename boost::tuples::element<N, Kernels>::type& kernel() {
		return boost::tuples::get<N>(m_kernel);
	}

// statistics accessors
	inline double acceptance () const { return m_acceptance; }
	inline double temperature() const { return m_temperature; }
	inline double delta() const { return m_delta; }
	inline double green_ratio() const { return m_green_ratio; }
	inline int kernel_id() const { return m_kernel_id; }
	inline bool accepted() const { return m_accepted; }
        enum { kernel_size = kernel_traits<Kernels>::size };

protected:
	Kernels m_kernel;
	die_t   m_die;
	int     m_kernel_id;
	double m_acceptance;
	double  m_temperature;
	double  m_delta;
	double  m_green_ratio;
	bool    m_accepted;
};

}; // namespace rjmcmc

#endif // __RJMCMC_SAMPLER_HPP__
