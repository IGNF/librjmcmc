#ifndef IMAGE_GRADIENT_UNARY_ENERGY_INC_HPP_
#define IMAGE_GRADIENT_UNARY_ENERGY_INC_HPP_

#include "core/image_gradient_unary_energy.hpp"
#include "core/gradient_image.hpp"

template<typename View>
void image_gradient_unary_energy::gradient(const View& view, const Iso_Rectangle_2& bbox, double sigmaD, unsigned int step)
{
	m_image->load(view,bbox,sigmaD,step);
}

#endif
