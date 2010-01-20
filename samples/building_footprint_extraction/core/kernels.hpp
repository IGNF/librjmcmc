#ifndef KERNELS_HPP_
#define KERNELS_HPP_

#include <boost/variant.hpp>
#include "geometry/geometry.h"
#include "core/random.hpp"
#include "core/bbox.hpp"

class uniform_birth_kernel {
	typedef boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > die_type;
	mutable die_type m_die;
public:
	uniform_birth_kernel() : m_die(GetRandom(), boost::uniform_real<>(0,1)) {}
	template<typename Configuration, typename Modification>
	double operator()(Configuration& c, Modification& modif) const
	{
		typedef typename Configuration::value_type T;
		const bbox_2& bbox = c.is_valid().bbox();
		float x0 = bbox.min_point()[0];
		float y0 = bbox.min_point()[1];
		float x1 = bbox.max_point()[0];
		float y1 = bbox.max_point()[1];
		do {
			modif.clear();
			Point_2 p(x0+(x1-x0)*m_die(), y0+(y1-y0)*m_die());
			Vector_2 v(0.5*(x1-x0)*m_die(), 0.5*(y1-y0)*m_die());
			if(m_die()<0.5) {
				modif.insert_birth(T(Rectangle_2(p, v, m_die())));
			} else {
				modif.insert_birth(T(Circle_2(p, m_die()*0.5*CGAL::min(x1-x0,y1-y0))));
			}
		} while (!c.is_valid(modif.birth()));
		return 1./bbox.volume();//(0.25*(x1-x0)*(x1-x0)*(y1-y0)*(y1-y0)); // TODO
	}
	template<typename Configuration, typename Modification>
	double pdf(const Configuration& c, const Modification& modif) const
	{
		if(modif.birth_size()!=0 || modif.death_size()!=1) return 0.;
		const bbox_2& bbox = c.is_valid().bbox();
		float x0 = bbox.min_point()[0];
		float y0 = bbox.min_point()[1];
		float x1 = bbox.max_point()[0];
		float y1 = bbox.max_point()[1];
		return 1./bbox.volume();//(0.25*(x1-x0)*(x1-x0)*(y1-y0)*(y1-y0)); // TODO
	}
};


template< typename T, typename IsValid >
class modifier {
	typedef boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > float_die_type;
	typedef boost::variate_generator<RJMCMCRandom&, boost::uniform_smallint<> > int_die_type;
	mutable float_die_type m_dief;
	mutable int_die_type   m_die4;
	mutable double m_green_ratio;
	double m_p_translation;
	double m_p_switch;
	double m_dx;
	double m_dy;
	IsValid m_is_valid;
public:
	typedef T result_type;
	modifier(const IsValid& is_valid) :
		m_dief(GetRandom(), boost::uniform_real<>(0,1)),
		m_die4(GetRandom(), boost::uniform_smallint<>(0,3)),
		m_p_translation(0.5), m_p_switch(0.5),
		m_dx(10), m_dy(10), m_is_valid(is_valid) {}

	inline double green_ratio() const { return m_green_ratio; }

	T operator()(const Rectangle_2 &r) const {
		if(m_dief()<m_p_switch) {
			double radius = sqrt(r.normal().squared_length());
			double ratio  = std::abs(r.ratio());
			if(ratio<1) radius *= ratio;
			Circle_2 c(r.center(),radius);
			if(m_is_valid(c)) {
				m_green_ratio = 1.; // TODO
				return T(c);
			}
		}
		Rectangle_2 res;
		if(m_dief()<m_p_translation) {
			do {
				res = r.scaled_edge(m_die4(),2*m_dief());
			} while (!m_is_valid(res));
		} else {
			do {
				Vector_2 v(m_dx*(2*m_dief()-1),m_dy*(2*m_dief()-1));
				res = r.rotation_scaled_corner(m_die4(), v);
			} while (!m_is_valid(res));
		}
		m_green_ratio = 1.;
		return T(res);
	}
	T operator()(const Circle_2 &c) const {
		if(m_dief()<m_p_switch) {
			Rectangle_2 r;
			do {
				Vector_2 v((2*m_dief()-1),(2*m_dief()-1));
				v = v*(c.radius()/sqrt(v.squared_length()));
				r = Rectangle_2(c.center(),v,1);
			} while (!m_is_valid(r));
			m_green_ratio = 1.; // TODO
			return T(r);
		}
		Circle_2 res;
		if(m_dief()<m_p_translation) {
			do {
				Vector_2 v(m_dx*(2*m_dief()-1),m_dy*(2*m_dief()-1));
				double dr = sqrt(v.squared_length());
				if (m_die4()%2 && c.radius()>dr) dr = -dr;
				res = Circle_2(c.center()+v,c.radius()+dr);
			} while (!m_is_valid(res));
		} else {
			do {
				res = Circle_2(c.center(),c.radius()*2*m_dief());
			} while (!m_is_valid(res));
		}
		m_green_ratio = 1.;
		return T(res);
	}
};

#endif /*KERNELS_HPP_*/
