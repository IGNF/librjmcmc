#ifndef IMAGE_GRADIENT_UNARY_ENERGY_INC_HPP_
#define IMAGE_GRADIENT_UNARY_ENERGY_INC_HPP_

#include "energy/image_gradient_unary_energy.hpp"
#include "image/gradient_image.hpp"

template<typename View, typename IsoRectangle>
void image_gradient_unary_energy::gradient(const View& view, const IsoRectangle& bbox, double sigmaD, unsigned int step)
{
	m_image->load(view,bbox,sigmaD,step);
}

template<typename T>
double image_gradient_unary_energy::operator()(const T &t) const
{
        return m_defaultEnergy - m_ponderation_gradient*(*m_image)(t);
}

#endif
