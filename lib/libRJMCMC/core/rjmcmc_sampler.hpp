#ifndef __RJMCMC_SAMPLER_HPP__
#define __RJMCMC_SAMPLER_HPP__

#include "core/random.hpp"
#include <boost/tuple/tuple.hpp>
#include <boost/mpl/insert_range.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_params_with_a_default.hpp>

namespace rjmcmc {

namespace internal {

// apply a random function object within a tuple
// x should originaly be drawn uniformly in [0,1]
// t is the tuple of function objects
template<typename C, typename M>
inline unsigned int random_apply(double x, const boost::tuples::null_type&, C &c, M &m, double& R, unsigned int i=0) {
	R=0;
	return i;
}

template <class H, class T, typename C, typename M>
inline unsigned int random_apply(double x, boost::tuples::cons<H, T>& t, C &c, M &m, double& R, unsigned int i=0) {
	double p = t.get_head().probability();
	if(x<p) { R=t.get_head()(c,m); return i+t.get_head().kernel_id(); }
	return random_apply(x-p,t.get_tail(),c,m,R,i+H::size);
}

template<typename T> struct cons_list_kernel_traits {
	enum { size = 0 };
};

template <class H, class T> struct cons_list_kernel_traits < boost::tuples::cons<H, T> > {
	enum { size = H::size + cons_list_kernel_traits<T>::size };
};

}

template<typename T> struct kernel_traits {
	enum { size = internal::cons_list_kernel_traits<typename T::inherited>::size };
};


template<typename Variant> 
inline int uniform_random_type_init(Variant &v)
{
	return 1;
}

#include <boost/preprocessor/iteration/local.hpp>
template<BOOST_VARIANT_ENUM_PARAMS(typename T)> 
inline int uniform_random_type_init(boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> &v)
{
	typedef boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> T;
	typedef typename T::types Types;
	typedef boost::variate_generator<rjmcmc::generator&, boost::uniform_smallint<> > die_type;
	const int N = variant_size<T>::value;
	die_type vdie(random(), boost::uniform_smallint<>(0,N-1));
	switch(vdie())        { 
#define BOOST_PP_LOCAL_LIMITS (0,BOOST_VARIANT_LIMIT_TYPES-1)
#define BOOST_PP_LOCAL_MACRO(n) case n : v = typename boost::mpl::at_c<Types,n%N>::type(); return (n*(n+1)<N)?1:2;
#include BOOST_PP_LOCAL_ITERATE()
		default : return 1; // should not get here
	}
}


template <typename T,typename U>
struct std_pair { typedef std::pair<T,U> type; };

template <typename T>
struct variant_pairs { typedef boost::variant<T,std::pair<T,T> > type; };

template<typename S, typename I>
struct variant_pairs_detail {
	typedef typename boost::mpl::pop_front<I>::type tail;
	typedef typename boost::mpl::front<I>::type     head;
	typedef typename variant_pairs_detail<S,tail>::type T;	typedef typename boost::mpl::end<T>::type T_end;	typedef std_pair<head,boost::mpl::_1> make_std_pair;
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
		std::pair<Circle_2,Circle_2> > type;
*/
/*
	typedef typename boost::mpl::transform<types, pair_type<boost::mpl::_1> >::type pair_types;	typedef typename boost::mpl::end<types>::type types_end;
	typedef typename boost::mpl::insert_range<types,types_end,pair_types>::type variant_types;
	//typedef types variant_types;
	//typedef typename boost::make_variant_over<variant_types>::type type;
*/

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
	mutable boost::variate_generator<rjmcmc::generator&, boost::uniform_real<> > m_die;
	mutable unsigned int m_kernel_id;
public:
	enum { size = 2 };
	inline unsigned int kernel_id() const { return m_kernel_id; }
	binary_kernel(const Kernel0& k0, const Kernel1& k1, double p=1.) : 
		m_kernel0(k0), m_kernel1(k1), m_p_sum(p),
		m_die(random(), boost::uniform_real<>(0,1)) {
		m_p[0]=m_p[1]=p*0.5;
	}
	binary_kernel(const Kernel0& k0, const Kernel1& k1, double p0, double p1) :
		m_kernel0(k0), m_kernel1(k1), m_p_sum(p0+p1),
		m_die(random(), boost::uniform_real<>(0,1)) {
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
	double operator()(Configuration& c, Modification& modif) const
	{
		if(m_die()*m_p_sum<m_p[0]) {
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
	typedef boost::variate_generator<rjmcmc::generator&, boost::uniform_real<> > die_type;
	Generator m_generator;
	mutable die_type m_die;
public:
	uniform_birth_kernel(const Generator& generator) : 
		m_generator(generator),
		m_die(random(), boost::uniform_real<>(0,1)) {}

	template<typename Configuration, typename Modification>
	double operator()(Configuration& c, Modification& modif) const
	{
		typedef typename Configuration::value_type T;
		T res;
		uniform_random_type_init(res);
		double p = rjmcmc::apply_visitor(m_generator,res);
		modif.insert_birth(res);
		return p;
	}

	template<typename Configuration, typename Modification>
	double pdf(const Configuration& c, const Modification& modif) const
	{
		typedef typename Configuration::value_type T;
		if(modif.birth_size()!=0 || modif.death_size()!=1) return 0.;
		double p = rjmcmc::apply_visitor(m_generator.pdf(),c[*modif.death_begin()]);
		return p;
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
	double operator()(Configuration& c, Modification& modif) const
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

		int    num_births  = uniform_random_type_init(out);
		double green_ratio = rjmcmc::apply_visitor(m_modifier,in,out);
		rjmcmc::apply_visitor(modif.birth_inserter(),out);

		switch(num_births-num_deaths) {
		case -1 : return green_ratio * m_poisson/n;
		case  0 : return green_ratio;
		case  1 : 
		default : return green_ratio *(n+1)/m_poisson;
		}
	}
};

// variadic template support
#if 0

#define RJMCMC_SAMPLER_LIMIT_TYPES 10000000000 
template<typename K...>
class sampler
{
public:
	typedef std::tuple<K...> Kernels;
	sampler(const K&... k) : m_kernel(k...),
		m_die(random(), boost::uniform_real<>(0,1)) {}

#else

#define RJMCMC_SAMPLER_LIMIT_TYPES 10
#define RJMCMC_SAMPLER_ENUM_PARAMS(x) \
	BOOST_PP_ENUM_PARAMS(RJMCMC_SAMPLER_LIMIT_TYPES,x)
#define RJMCMC_SAMPLER_ENUM_BINARY_PARAMS(x,y) \
	BOOST_PP_ENUM_BINARY_PARAMS(RJMCMC_SAMPLER_LIMIT_TYPES,x,y)
#define RJMCMC_SAMPLER_ENUM_PARAMS_WITH_A_DEFAULT(x,y) \
	BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(RJMCMC_SAMPLER_LIMIT_TYPES,x,y)
#define RJMCMC_SAMPLER_ENUM(x) \
	BOOST_PP_ENUM(RJMCMC_SAMPLER_LIMIT_TYPES,x,nil)
#define RJMCMC_SAMPLER_ARG(z,n,_) const K##n& k##n = K##n()

template<RJMCMC_SAMPLER_ENUM_PARAMS_WITH_A_DEFAULT(typename K,boost::tuples::null_type)>
class sampler
{
public:
	typedef boost::tuple<RJMCMC_SAMPLER_ENUM_PARAMS(K)> Kernels;
	sampler(RJMCMC_SAMPLER_ENUM(RJMCMC_SAMPLER_ARG)) :
		m_kernel(RJMCMC_SAMPLER_ENUM_PARAMS(k)),
		m_die(random(), boost::uniform_real<>(0,1)) {}

#endif

// kernel accessors
	enum { size = boost::tuples::length<Kernels>::value };

	template<unsigned int N>
	inline const typename boost::tuples::element<N, Kernels>::type& kernel() const {
		return boost::tuples::get<N>(m_kernel);
	}
	template<unsigned int N>
	inline typename boost::tuples::element<N, Kernels>::type& kernel() {
		return boost::tuples::get<N>(m_kernel);
	}

// statistics accessors
	inline double delta() const { return m_delta; }
	inline double green_ratio() const { return m_green_ratio; }
	inline int kernel_id() const { return m_kernel_id; }
	inline bool accepted() const { return m_accepted; }

// main sampling function
	template<typename Configuration> void operator()(Configuration &c, double temp)
	{
		typename Configuration::modification modif;
		m_kernel_id   = internal::random_apply(m_die(),m_kernel,c,modif,m_green_ratio);
		if(m_green_ratio<=0) {
			m_delta   =0;
			m_accepted=false;
			return;
		}
		m_delta       = c.delta_energy(modif);
		m_green_ratio*= exp(-m_delta/temp);
		m_accepted    = ( m_die() < m_green_ratio );
		if (m_accepted) c.apply(modif);
	}

private:
	Kernels m_kernel;
	boost::variate_generator<rjmcmc::generator&, boost::uniform_real<> > m_die;
	int     m_kernel_id;
	double  m_delta;
	double  m_green_ratio;
	bool    m_accepted;
};

}; // namespace rjmcmc

#endif // __RJMCMC_SAMPLER_HPP__
