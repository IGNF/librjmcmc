#ifndef __RJMCMC_SAMPLER_HPP__
#define __RJMCMC_SAMPLER_HPP__

#include "core/random.hpp"



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


}

//////////////////


template<typename Kernel0, typename Kernel1> class binary_kernel {
	double m_p[2], m_p_sum;
	typedef boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > die_type;
	die_type m_die;
	Kernel0 m_kernel0;
	Kernel1 m_kernel1;
	unsigned int m_kernel_id;
public:
	enum { size = 2 };
	inline unsigned int kernel_id() const { return m_kernel_id; }
	binary_kernel(double p=1.) : m_p_sum(p),
		m_die(GetRandom(), boost::uniform_real<>(0,1)) {
		m_p[0]=m_p[1]=p*0.5;
	}
	binary_kernel(double p0, double p1) : m_p_sum(p0+p1),
		m_die(GetRandom(), boost::uniform_real<>(0,1)) {
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
	double operator()(Configuration& c, Modification& modif)
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

class uniform_birth_kernel {
	typedef boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > die_type;
	die_type m_die;
public:
	uniform_birth_kernel() : m_die(GetRandom(), boost::uniform_real<>(0,1)) {}
	template<typename Configuration, typename Modification>
	double operator()(Configuration& c, Modification& modif)
	{
		typedef typename Configuration::value_type T;
		const bbox_2& bbox = c.is_valid().bbox();
		float x0 = bbox.min_point()[0];
		float y0 = bbox.min_point()[1];
		float x1 = bbox.max_point()[0];
		float y1 = bbox.max_point()[1];
		do {
			typename T::Point_2 p(x0+(x1-x0)*m_die(), y0+(y1-y0)*m_die());
			typename T::Vector_2 v(0.5*(x1-x0)*m_die(), 0.5*(y1-y0)*m_die());
			typename T::FT r(m_die());
			modif.birth(T(p, v, r));
		} while (!c.is_valid(modif.birth()));
		modif.death(c.end());
		return 1./bbox.volume();//(0.25*(x1-x0)*(x1-x0)*(y1-y0)*(y1-y0));
	}
	template<typename Configuration, typename Modification>
	double pdf(const Configuration& c, const Modification& modif) const
	{
		if(modif.birth_size(c)!=0 || modif.death_size(c)!=1) return 0.;
		const bbox_2& bbox = c.is_valid().bbox();
		float x0 = bbox.min_point()[0];
		float y0 = bbox.min_point()[1];
		float x1 = bbox.max_point()[0];
		float y1 = bbox.max_point()[1];
		return 1./bbox.volume();//(0.25*(x1-x0)*(x1-x0)*(y1-y0)*(y1-y0));
	}
};

class uniform_death_kernel {
	typedef boost::variate_generator<RJMCMCRandom&, boost::uniform_smallint<> > die_type;
public:
	uniform_death_kernel() {}
	template<typename Configuration, typename Modification>
	double operator()(Configuration& c, Modification& modif)
	{
		typedef typename Configuration::value_type T;
		const bbox_2& bbox = c.is_valid().bbox();
		typename Configuration::iterator it = c.begin();
		if(!c.empty()) {
			die_type die(GetRandom(), boost::uniform_smallint<>(0,c.size()-1));
			std::advance(it, die());
		}
		modif.birth(T());
		modif.death(it);
		return 1./c.size();
	}
	template<typename Configuration, typename Modification>
	double pdf(const Configuration& c, const Modification& modif) const
	{
		if(modif.birth_size(c)!=1 || modif.death_size(c)!=0) return 0.;
		return 1./(c.size()+1);
	}
};

template<typename Kernel> class unary_kernel {
	double m_p;
	Kernel m_kernel;
public:
	enum { size = 1 };
	unary_kernel(double p=1.) : m_p(p) {}
	inline unsigned int kernel_id() const { return 0; }
	inline void probability(double p) { m_p = p; }
	inline double probability() const { return m_p; }
	inline void probability(unsigned int, double p) { m_p = p; }
	inline double probability(unsigned int) const { return m_p; }
	template<typename Configuration, typename Modification>
	double operator()(Configuration& c, Modification& modif)
	{
		return m_kernel(c,modif); //returns p(inverse(modif))/p(modif) //p270
	}
};

class rectangle_modification_kernel {
	typedef boost::variate_generator<RJMCMCRandom&, boost::uniform_smallint<> > int_die_type;
	typedef boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > float_die_type;
	mutable float_die_type m_dief;
	mutable int_die_type   m_die4;
	double m_p_edge_translation;
	double m_dx;
	double m_dy;
public:
	rectangle_modification_kernel() :
		m_dief(GetRandom(), boost::uniform_real<>(0,1)),
		m_die4(GetRandom(), boost::uniform_smallint<>(0,3)),
		m_p_edge_translation(0.5), m_dx(10), m_dy(10) {}

	template<typename Configuration, typename Modification>
	double operator()(Configuration& c, Modification& modif)
	{
		typedef typename Configuration::value_type T;
		if(c.empty()) {
			modif.death(c.end());
			modif.birth(T());
			return 0;
		}
		typename Configuration::iterator it = c.begin();
		int_die_type diec(GetRandom(), boost::uniform_smallint<>(0,c.size()-1));
		std::advance(it, diec());
		modif.death(it);
		T n = c[it];
		double d = m_dief();
		if(d<m_p_edge_translation) {
			do {
				modif.birth(n.scaled_edge(m_die4(),2*m_dief()));
			} while (!c.is_valid(modif.birth()));
		} else {
			do {
				typename T::Vector_2 v(m_dx*(2*m_dief()-1),m_dy*(2*m_dief()-1));
				modif.birth(n.rotation_scaled_corner(m_die4(), v));
			} while (!c.is_valid(modif.birth()));
		}
		return 1.; // neglecting border issues
	}
};



typedef binary_kernel<uniform_birth_kernel,uniform_death_kernel> uniform_birth_death_kernel;
typedef unary_kernel<rectangle_modification_kernel> modification_kernel;


class geometric_temperature {
public:
	geometric_temperature(double temp, double coefficient) : m_temp(temp), m_coefficient(coefficient) {}

	inline double  operator*() const { return m_temp; }
	inline double& operator*()       { return m_temp; }
	inline geometric_temperature  operator++()    { m_temp *= m_coefficient; return *this; }
	inline geometric_temperature  operator++(int) { geometric_temperature t(*this); m_temp *= m_coefficient; return t; }

	inline double coefficient() const { return m_coefficient; }
	inline void coefficient(double coef) { m_coefficient = coef; }

private:
	double m_temp;
	double m_coefficient;
};

// variadic template supports would hide the std::tuple implementation detail
#if 0
	#define IF_VARIADIC(x,y) x
#else
	#define IF_VARIADIC(x,y) y
#endif

template<typename IF_VARIADIC(K...,Kernels)>
class rjmcmc_sampler
{
	IF_VARIADIC(typedef std::tuple<K...> Kernels, )
public:
// constructor
	rjmcmc_sampler(const IF_VARIADIC(K&...,Kernels&) k) :
		m_kernel(IF_VARIADIC(k...,k)), m_die(GetRandom(), boost::uniform_real<>(0,1))
	{}

// kernel accessors
	enum { size = boost::tuples::length<Kernels>::value };

	template<unsigned int N>
	inline const typename boost::tuples::element<N, Kernels>::type& kernel() const {
		return m_kernel.get<N>();
	}
	template<unsigned int N>
	inline typename boost::tuples::element<N, Kernels>::type& kernel() {
		return m_kernel.get<N>();
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
		m_delta       = c.delta_energy(modif);
		m_green_ratio*= exp(-m_delta/temp);
		m_accepted    = ( m_die() < m_green_ratio );
		if (m_accepted) c.apply(modif);
	}

private:
	Kernels m_kernel;
	boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > m_die;
	int     m_kernel_id;
	double  m_delta;
	double  m_green_ratio;
	bool    m_accepted;
};

#endif // __RJMCMC_SAMPLER_HPP__
