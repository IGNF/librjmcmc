#ifndef KERNELS_HPP_
#define KERNELS_HPP_

#include <boost/variant.hpp>
#include "geometry/geometry.h"
#include "core/random.hpp"
#include "core/bbox.hpp"

template< typename T, typename IsValid >
class generator {
	typedef boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > die_type;
	mutable die_type m_die;
	IsValid m_is_valid;
public:
	generator(const IsValid& is_valid) :
		m_die(GetRandom(), boost::uniform_real<>(0,1)),
		m_is_valid(is_valid) {}

	typedef double result_type;

	result_type operator()(Rectangle_2 &r) const {
		const bbox_2& bbox = m_is_valid.bbox();
		float x0 = bbox.min_point()[0];
		float y0 = bbox.min_point()[1];
		float dx = bbox.max_point()[0]-x0;
		float dy = bbox.max_point()[1]-y0;
		do {
			Point_2 p(x0+dx*m_die(), y0+dy*m_die());
			Vector_2 v(0.5*dx*m_die(), 0.5*dy*m_die());
			r = Rectangle_2(p, v, m_die());
		} while (!m_is_valid(r));
		return 1./bbox.volume();
		//return 4./(dx*dx*dy*dy);
	}

	result_type operator()(Circle_2 &c) const {
		const bbox_2& bbox = m_is_valid.bbox();
		float x0 = bbox.min_point()[0];
		float y0 = bbox.min_point()[1];
		float dx = bbox.max_point()[0]-x0;
		float dy = bbox.max_point()[1]-y0;
		double radius = 0.5*CGAL::min(dx,dy);
		do {
			Point_2 p(x0+dx*m_die(), y0+dy*m_die());
			c = Circle_2(p, m_die()*radius);
		} while (!m_is_valid(c));
		return 1./bbox.volume();
		//return 1./(dx*dy*radius);
	}

	
	class pdf_visitor {
		IsValid m_is_valid;
	public:
		pdf_visitor(const IsValid& is_valid) : m_is_valid(is_valid) {}

		typedef double result_type;

		result_type operator()(const Rectangle_2 &r) const {
			const bbox_2& bbox = m_is_valid.bbox();
			return 1./bbox.volume();
			/*float x0 = bbox.min_point()[0];
			float y0 = bbox.min_point()[1];
			float dx = bbox.max_point()[0]-x0;
			float dy = bbox.max_point()[1]-y0;
			return 4./(dx*dx*dy*dy);*/
		}
	
		result_type operator()(const Circle_2 &c) const {
			const bbox_2& bbox = m_is_valid.bbox();
			return 1./bbox.volume();
			/*float x0 = bbox.min_point()[0];
			float y0 = bbox.min_point()[1];
			float dx = bbox.max_point()[0]-x0;
			float dy = bbox.max_point()[1]-y0;
			double radius = 0.5*CGAL::min(dx,dy);
			return 1./(dx*dy*radius);*/
		}
	};
	inline pdf_visitor pdf() const { return pdf_visitor(m_is_valid); }
};

template< typename T, typename IsValid >
class modifier {
	typedef boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > float_die_type;
	typedef boost::variate_generator<RJMCMCRandom&, boost::uniform_smallint<> > int_die_type;
	mutable float_die_type m_dief;
	mutable int_die_type   m_die4;
	double m_p_translation;
	double m_dx, m_dy;
	IsValid m_is_valid;
public:
	modifier(const IsValid& is_valid) :
		m_dief(GetRandom(), boost::uniform_real<>(0,1)),
		m_die4(GetRandom(), boost::uniform_smallint<>(0,3)),
		m_p_translation(0.5),
		m_dx(10), m_dy(10), m_is_valid(is_valid) {}

	typedef double result_type;

	double operator()(const Rectangle_2 &r, Rectangle_2 &res) const {
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
		return 1.;
	}

	double operator()(const Circle_2 &c, Circle_2 &res) const {
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
		return 1.;
	}

	double operator()(const Rectangle_2 &r, Circle_2 &c) const {
		double radius = sqrt(r.normal().squared_length());
		double ratio  = std::abs(r.ratio());
		if(ratio<1) radius *= ratio;
		c = Circle_2(r.center(),radius);
		if(m_is_valid(c)) {
			return 1.; // TODO
		} else {
			return 0.;
		}
	}

	double operator()(const Circle_2 &c, Rectangle_2 &r) const {
		do {
			Vector_2 v((2*m_dief()-1),(2*m_dief()-1));
			v = v*(c.radius()/sqrt(v.squared_length()));
			r = Rectangle_2(c.center(),v,1);
		} while (!m_is_valid(r));
		return 1.; // TODO
	}
};

#endif /*KERNELS_HPP_*/
