#ifndef GRADIENT_IMAGE_HPP
#define GRADIENT_IMAGE_HPP

#include <boost/gil/image.hpp>
#include <boost/gil/extension/matis/float_images.hpp>

namespace rjmcmc {
class gradient_image
{
public:
        typedef boost::gil::dev2n32F_image_t image_t;
	typedef boost::gil::dev2n32F_pixel_t pixel_t;

	typedef double result_type;
        template<typename T> double operator()(const T&t) const {
                return /*inverted_*/integrated_flux(boost::gil::const_view(m_gradients),x0,y0,t);
        }

	template<typename IsoRectangle>
	void load(const std::string &file, const IsoRectangle& bbox, double sigmaD=1, unsigned int step=0);

	template<typename View, typename IsoRectangle>
	void load(const View& view, const IsoRectangle& bbox, double sigmaD=1, unsigned int step=0);

private:
	image_t m_gradients;
	int x0, y0;
};
};

#endif // GRADIENT_IMAGE_HPP
