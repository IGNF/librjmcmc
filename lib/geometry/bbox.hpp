#ifndef BOX_HPP_
#define BOX_HPP_
#include <boost/array.hpp>

fdqsfds
/**
 * @brief Bounding box class, templated on the dimension
 */
/*
template<typename F=float, unsigned int D=2> class bbox
{
public :
	typedef F                 float_type;
	typedef boost::array<F,D> point_type;
	typedef typename point_type::const_iterator const_iterator;
	enum { dimension = D };

	bbox(){}
	bbox(const point_type& pmin , const point_type& pmax)
	{
		m_volume = 1.;
		for(unsigned int i=0;i<D;++i)
		{
			m_min[i] = pmin[i];
			m_max[i] = pmax[i];
			m_volume *= pmax[i]-pmin[i];
		}
	}

	const point_type &min_point() const { return m_min; }
	const point_type &max_point() const { return m_max; }

	bool is_inside(const point_type& point) const
	{
		const_iterator imin = m_min.begin();
		const_iterator imax = m_max.begin();
		const_iterator p    = point.begin();
		const_iterator end  = point.end();
		for( ; p!=end ; ++imin,++imax,++p)
			if( *p < *imin || *imax < *p )
				return false;
		return true;
	}
	F volume() const { return m_volume; }

private:
	point_type m_min;
	point_type m_max;
	F          m_volume;
};


template <typename F, unsigned int D>
std::ostream& operator<<(std::ostream& o, bbox<F,D> b) {
	typedef typename bbox<F,D>::point_type point_type;
	typedef typename point_type::const_iterator const_iterator;
	const_iterator i = b.min_point().begin(), end = b.min_point().end();
	const_iterator j = b.max_point().begin();
	for(;i!=end;++i,++j) o<<"["<<*i<<","<<*j<<"]";
	return o;
}

typedef bbox<> bbox_2;
*/

#endif //#ifndef BOX_HPP_
