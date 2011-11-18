#ifndef IMAGE_GRADIENT_UNARY_ENERGY_INC_HPP_
#define IMAGE_GRADIENT_UNARY_ENERGY_INC_HPP_

#include "mpp/energy/image_gradient_unary_energy.hpp"
#include "geometry/integrated_flux.h"

template<typename Image, typename Value>
template<typename T>
typename image_gradient_unary_energy<Image,Value>::result_type
image_gradient_unary_energy<Image,Value>::operator()(const T &t) const
{
    return integrated_flux(m_image,t);
}

template<typename Image, typename Value>
image_gradient_unary_energy<Image,Value>::image_gradient_unary_energy(const Image& image)
    : m_image(image) {}

#endif
