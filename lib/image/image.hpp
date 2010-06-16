#ifndef RJMCMC_IMAGE_HPP_
#define RJMCMC_IMAGE_HPP_

#include <boost/gil/image.hpp>

template<typename View>
class oriented {
public:
	typedef View view_t;
	oriented(const view_t& view, int x0, int y0) : m_view(view), m_x0(x0), m_y0(y0) {}
	oriented() : m_view(), m_x0(0), m_y0(0) {}
	inline int x0() const { return m_x0; }
	inline int y0() const { return m_y0; }
	inline const view_t& view() const { return m_view; }
private:
	view_t m_view;
	int m_x0, m_y0;
};

template<typename IsoRectangle>
void clip_bbox(IsoRectangle& bbox, const std::string &file);

template<typename IsoRectangle, typename View>
void clip_bbox(IsoRectangle& bbox, const oriented<View>& v);

template<typename View, typename IsoRectangle>
void load_view(oriented<View>& out, const oriented<View>& in, const IsoRectangle& bbox);

template<typename View, typename Image, typename IsoRectangle>
void load_view(oriented<View>& out, Image& img, const std::string &file, const IsoRectangle& bbox);


struct static_cast_color_converter
{
	template<class Src, class Dst>
	void operator()(const Src &src, Dst &dst) const
	{
		typedef typename boost::gil::channel_type<Dst>::type type;
		boost::gil::at_c<0>(dst) = static_cast<type>(boost::gil::at_c<0>(src));
	}
};


#endif // RJMCMC_IMAGE_HPP_
