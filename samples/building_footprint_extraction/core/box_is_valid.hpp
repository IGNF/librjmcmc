#ifndef BOX_IS_VALID_HPP_
#define BOX_IS_VALID_HPP_

#include "geometry/geometry.h"
#include "geometry/bbox.hpp"

//////////////////////////////////////////////////////////

class box_is_valid
{
public:
	typedef bool result_type;
	result_type operator()(const Rectangle_2 &n) const;
	result_type operator()(const Circle_2 &n) const;

	box_is_valid(const bbox_2 &box, double min_size, double max_ratio)
	 : m_box(box), m_squared_min_size(min_size*min_size), m_max_ratio(max_ratio) {}

	inline const bbox_2& bbox() const { return m_box; }
private:
	bbox_2 m_box;
	double m_squared_min_size;
	double m_max_ratio;
};

#endif /*BOX_IS_VALID_HPP_*/
