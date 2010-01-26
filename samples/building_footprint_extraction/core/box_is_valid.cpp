#include "core/box_is_valid.hpp"

bool box_is_valid::operator()(const Rectangle_2 &n) const
{
	for (unsigned int i = 0; i < 4; ++i)
	{
		bbox_2::point_type ptb;
		ptb[0] = n[i].x();
		ptb[1] = n[i].y();

		if (!m_box.is_inside(ptb)) return false;
	}
	if (n.squared_length(0) < m_squared_min_size || n.squared_length(1) < m_squared_min_size)
		return false;

	float ratio = std::abs(n.ratio());
	return (ratio <= m_max_ratio && 1 <= m_max_ratio*ratio );
}

bool box_is_valid::operator()(const Circle_2 &n) const
{
	bbox_2::point_type pmin = m_box.min_point();
	bbox_2::point_type pmax = m_box.max_point();
	return 	n.center().x()>=pmin[0]+n.radius() &&
		n.center().x()<=pmax[0]-n.radius() &&
		n.center().y()>=pmin[1]+n.radius() &&
		n.center().y()<=pmax[1]-n.radius() &&
		n.squared_radius() >= m_squared_min_size;
}

