#ifndef BOX_IS_VALID_HPP_
#define BOX_IS_VALID_HPP_

class box_is_valid
{
public:
    typedef bool result_type;
#ifdef GEOMETRY_RECTANGLE_2_H
    template<typename K> result_type operator()(const geometry::Rectangle_2<K> &n) const
    {
	if (!m_box.has_on_bounded_side (n.center())) return false;
	if (n.squared_length(0) < m_squared_min_size || n.squared_length(1) < m_squared_min_size)
            return false;

	float ratio = std::abs(n.ratio());
	return (ratio <= m_max_ratio && 1 <= m_max_ratio*ratio );
    }
#endif
#ifdef GEOMETRY_CIRCLE_2_H
    template<typename K> result_type operator()(const geometry::Circle_2<K> &n) const
    {
	if (!m_box.has_on_bounded_side (n.center())) return false;
	return 	n.squared_radius() >= m_squared_min_size;
    }
#endif

    box_is_valid(const Iso_rectangle_2 &box, double min_size, double max_ratio)
        : m_box(box), m_squared_min_size(min_size*min_size), m_max_ratio(max_ratio) {}

    inline const Iso_rectangle_2& bbox() const { return m_box; }
    inline double squared_min_size() const { return m_squared_min_size; }
    inline double max_ratio() const { return m_max_ratio; }
private:
    Iso_rectangle_2 m_box;
    double m_squared_min_size;
    double m_max_ratio;
};

#endif /*BOX_IS_VALID_HPP_*/
