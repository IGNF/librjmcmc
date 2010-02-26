#ifndef IMAGE_GRADIENT_UNARY_ENERGY_HPP_
#define IMAGE_GRADIENT_UNARY_ENERGY_HPP_

#include <boost/shared_ptr.hpp>

#include "core/geometry.h"
namespace rjmcmc { class gradient_image; };

class image_gradient_unary_energy
{
public:
	typedef double result_type;

	template<typename T>
	result_type operator()(const T &t) const;

	image_gradient_unary_energy(double default_energy, double ponderation_gradient);

	template<typename View, typename IsoRectangle>
	void gradient(const View& view, const IsoRectangle& bbox, double sigmaD=1, unsigned int step=0);

private:
	boost::shared_ptr<rjmcmc::gradient_image> m_image;
	double m_defaultEnergy, m_ponderation_gradient;
};


#endif /*IMAGE_GRADIENT_UNARY_ENERGY_HPP_*/
