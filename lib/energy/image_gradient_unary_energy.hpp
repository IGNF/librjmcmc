#ifndef IMAGE_GRADIENT_UNARY_ENERGY_HPP_
#define IMAGE_GRADIENT_UNARY_ENERGY_HPP_

#include <boost/shared_ptr.hpp>

#include "core/geometry.h"
namespace rjmcmc { class gradient_image; };

class image_gradient_unary_energy
{
public:
	typedef double result_type;
	result_type operator()(const Rectangle_2 &n) const;
	result_type operator()(const Circle_2    &c) const;

	image_gradient_unary_energy(double default_energy);

	template<typename View>
	void gradient(const View& view, const Iso_rectangle_2& bbox, double sigmaD=1, unsigned int step=0);

private:
	boost::shared_ptr<rjmcmc::gradient_image> m_image;
	double m_defaultEnergy;
};


#endif /*IMAGE_GRADIENT_UNARY_ENERGY_HPP_*/
