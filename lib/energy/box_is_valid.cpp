#include "energy/box_is_valid.hpp"

#ifdef RECTANGLE_2_H
bool box_is_valid::operator()(const Rectangle_2 &n) const
{
	if (!m_box.has_on_bounded_side (n.center())) return false;
	if (n.squared_length(0) < m_squared_min_size || n.squared_length(1) < m_squared_min_size)
		return false;

	float ratio = std::abs(n.ratio());
	return (ratio <= m_max_ratio && 1 <= m_max_ratio*ratio );
}
#endif

#ifdef CIRCLE_2_H
bool box_is_valid::operator()(const Circle_2 &n) const
{
	if (!m_box.has_on_bounded_side (n.center())) return false;
	return 	n.squared_radius() >= m_squared_min_size;
}
#endif

