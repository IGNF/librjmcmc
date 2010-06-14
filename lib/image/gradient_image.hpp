#ifndef GRADIENT_IMAGE_HPP
#define GRADIENT_IMAGE_HPP

#include <boost/gil/image.hpp>
#include <boost/gil/extension/matis/float_images.hpp>
typedef boost::gil::dev2n32F_image_t gradient_image_t;
typedef boost::gil::dev2n32F_view_t  gradient_view_t;
typedef const boost::gil::dev2n32F_view_t  gradient_const_view_t;

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

template<typename View, typename IsoRectangle>
void gradient_image(gradient_image_t& img,  int& x0, int& y0,
	const View& view, IsoRectangle& bbox, double sigmaD=1, unsigned int step=0);

template<typename IsoRectangle>
void gradient_image(gradient_image_t& img,  int& x0, int& y0,
	const std::string &file, IsoRectangle& bbox, double sigmaD=1, unsigned int step=0);

#endif // GRADIENT_IMAGE_HPP
