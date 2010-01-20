#ifndef __Circle_2_H__
#define __Circle_2_H__

CGAL_BEGIN_NAMESPACE

template<class R_> class Circle_2
{
public:
	typedef R_ R;
	typedef typename R_::RT RT;
	typedef typename R_::FT FT;
	typedef typename R_::Point_2 Point_2;
	typedef typename R_::Vector_2 Vector_2;
	typedef typename R_::Line_2 Line_2;
	typedef Circle_2 Self;

	Circle_2()
	{}
	Circle_2(const Point_2 &center, double radius) : m_center(center), m_radius(radius), m_squared_radius(radius*radius)
	{}

	inline Point_2 center() const
	{	return m_center;}
	inline bool is_degenerate() const
	{	return m_radius==0;}
	inline FT radius() const
	{	return m_radius;}
	inline FT squared_radius() const
	{	return m_squared_radius;}

	inline FT area() const
	{	return M_PI*squared_radius();}
	inline FT perimeter() const
	{	return 2*M_PI*radius();}

private :
	Point_2 m_center;
	FT m_radius, m_squared_radius;
};


template < class R >
std::ostream &
operator<<(std::ostream &os, const Circle_2<R> &c)
{
	switch(os.iword(IO::mode))
	{
		case IO::ASCII :
		return os << c.center() << ' ' << c.radius();
		case IO::BINARY :
		return os << c.center() << c.radius();
		default:
		return os << "Circle_2(" << c.center() << ", " << c.radius() << ")";
	}
}

template < class R >
std::istream &
operator>>(std::istream &is, Circle_2<R> &b)
{
	typename R::Point_2 p;
	typename R::FT r;

	is >> p >> r;

	if (is)
	b = Circle_2<R>(p, r);
	return is;
}

CGAL_END_NAMESPACE

#endif //#ifndef __Circle_2_H__
