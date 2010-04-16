#ifndef IMAGE_GRADIENT_FACADE_ELEMENTS_UNARY_ENERGY_INC_HPP_
#define IMAGE_GRADIENT_FACADE_ELEMENTS_UNARY_ENERGY_INC_HPP_

#include "energy/image_gradient_unary_energy_facade_elements.hpp"
#include "image/gradient_image.hpp"

template<typename View, typename IsoRectangle>
void image_gradient_unary_energy_facade_elements::gradient(const View& view, const IsoRectangle& bbox, double sigmaD, unsigned int step)
{
        m_image->load(view,bbox,sigmaD,step);
}

template<typename T>
double image_gradient_unary_energy_facade_elements::operator()(const T &t) const
{
        return m_defaultEnergy - m_ponderation_gradient*(*m_image)(t);
}

#endif
