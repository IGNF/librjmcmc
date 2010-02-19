#ifndef KERNELS_HPP_
#define KERNELS_HPP_

#include "rjmcmc/random.hpp"
#include "geometry/geometry.h"

template< typename IsValid >
class generator {
	typedef boost::variate_generator<rjmcmc::generator&, boost::uniform_real<> > die_type;
	mutable die_type m_die;
	IsValid m_is_valid;
public:
	generator(const IsValid& is_valid) :
		m_die(rjmcmc::random(), boost::uniform_real<>(0,1)),
		m_is_valid(is_valid) {}

	typedef double result_type;

	result_type operator()(Rectangle_2 &r) const {
		const Iso_rectangle_2& bbox = m_is_valid.bbox();
		float x0 = bbox.min().x();
		float y0 = bbox.min().y();
		float dx = bbox.max().x()-x0;
		float dy = bbox.max().y()-y0;
		do {
			Point_2 p(x0+dx*m_die(), y0+dy*m_die());
			Vector_2 v(0.5*dx*m_die(), 0.5*dy*m_die());
			r = Rectangle_2(p, v, m_die());
		} while (!m_is_valid(r));
		return 1.;
	}

	result_type operator()(Circle_2 &c) const {
		const Iso_rectangle_2& bbox = m_is_valid.bbox();
		float x0 = bbox.min().x();
		float y0 = bbox.min().y();
		float dx = bbox.max().x()-x0;
		float dy = bbox.max().y()-y0;
		double radius = 0.5*CGAL::min(dx,dy);
		do {
			Point_2 p(x0+dx*m_die(), y0+dy*m_die());
			c = Circle_2(p, m_die()*radius);
		} while (!m_is_valid(c));
		return 1.;
	}

	
	struct pdf_visitor {
		typedef double result_type;
		result_type operator()(const Rectangle_2 &r) const { return 1.; }
		result_type operator()(const Circle_2 &c) const { return 1.; }
	};
	inline pdf_visitor pdf() const { return pdf_visitor(); }
};

template< typename IsValid >
class modifier {
	typedef boost::variate_generator<rjmcmc::generator&, boost::uniform_real<> > float_die_type;
	typedef boost::variate_generator<rjmcmc::generator&, boost::uniform_smallint<> > int_die_type;
	mutable float_die_type m_dief;
	mutable int_die_type   m_die4;
	double m_p_translation;
	double m_dx, m_dy;
	IsValid m_is_valid;
public:
	modifier(const IsValid& is_valid) :
		m_dief(rjmcmc::random(), boost::uniform_real<>(0,1)),
		m_die4(rjmcmc::random(), boost::uniform_smallint<>(0,3)),
		m_p_translation(0.5),
		m_dx(20), m_dy(20), m_is_valid(is_valid) {}

	typedef double result_type;

	template<typename T0, typename T1>
	double operator()(const T0 &t0, T1 &t1) const {
		return 0;
	}

	double operator()(const Rectangle_2 &r, Rectangle_2 &res) const {
		if(m_dief()<m_p_translation) {
			res = r.scaled_edge(m_die4(),exp(0.5-m_dief()));
		} else {
			Vector_2 v(m_dx*(m_dief()-0.5),m_dy*(m_dief()-0.5));
			res = r.rotation_scaled_corner(m_die4(), v);
		}
		if(!m_is_valid(res)) return 0.;
		return 1.; // TODO
	}

	double operator()(const Circle_2 &c, Circle_2 &res) const {
		if(m_dief()<m_p_translation) {
			Vector_2 v(m_dx*(m_dief()-0.5),m_dy*(m_dief()-0.5));
			double d2 = v.squared_length();
			double d = sqrt(d2);
			if (m_die4()%2 && c.squared_radius()>d2) d = -d;
			res = Circle_2(c.center()+v,CGAL::radius(c)+d);
		} else {
			res = Circle_2(c.center(),radius(c)*exp(0.5-m_dief()));
		}
		if(!m_is_valid(res)) return 0.;
		return 1.; // TODO
	}

	double operator()(const Rectangle_2 &r, Circle_2 &c) const {
		double radius = sqrt(r.normal().squared_length());
		double ratio  = std::abs(r.ratio());
		if(ratio<1) radius *= ratio;
		c = Circle_2(r.center(),radius);
		if(!m_is_valid(c)) return 0.;
		return 1.; // TODO
	}

	double operator()(const Circle_2 &c, Rectangle_2 &r) const {
		Vector_2 v((2*m_dief()-1),(2*m_dief()-1));
		v = v*(CGAL::radius(c)/sqrt(v.squared_length()));
		r = Rectangle_2(c.center(),v,1);
		if (!m_is_valid(r)) return 0;
		return 1.; // TODO
	}

	double operator()(const Rectangle_2 &r, std::pair<Rectangle_2,Rectangle_2> &res) const {
		int i = m_die4();
		float f = m_dief();
		float g = m_dief()*(1-f);
		res.first  = r.scaled_edge(i  ,f);
		res.second = r.scaled_edge(i+2,g);
		if (!(m_is_valid(res.first) && m_is_valid(res.second) )) return 0;
		return 1.; // TODO
	}
	double operator()(const std::pair<Rectangle_2,Rectangle_2> &r, Rectangle_2 &res) const {
		res = r.first.rotate(m_die4()).merge(r.second.rotate(m_die4()));
		if (!(m_is_valid(res))) return 0;
		return 1.; // TODO
	}

	double operator()(const Rectangle_2 &r, std::pair<Rectangle_2,Circle_2> &res) const {
		// todo: verifier que ca fait pas n'importe quoi...
		int i = m_die4();
		float f = m_dief();
		float g = m_dief();
		float h = m_dief();
		Vector_2 n = r.normal(i)*std::min(1.,std::abs(r.ratio()));
		if(i%2) n = n*(1./r.ratio());
		Vector_2 v(-n.x()+n.y(),-n.y()-n.x());
		res.first  = r.scaled_edge(i,f).scaled_edge(i+1,g);
		res.second = Circle_2(r[i+3]+v*h,h*std::sqrt(n.squared_length()));
		if (!(m_is_valid(res.first) && m_is_valid(res.second) )) return 0;
		return 1.; // TODO
	}

};

#endif /*KERNELS_HPP_*/
