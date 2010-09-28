#ifndef IMAGE_GRADIENT_UNARY_ENERGY_INC_HPP_
#define IMAGE_GRADIENT_UNARY_ENERGY_INC_HPP_

#include "energy/image_gradient_unary_energy.hpp"
#include "geometry/integrated_flux.h"

template<typename DSM>
template<typename T>
double image_gradient_unary_energy<DSM>::operator()(const T &t) const
{
    return m_defaultEnergy
            - m_ponderation_dsm*integrated_flux(m_dsm,t);
}



template<typename DSM>
image_gradient_unary_energy<DSM>::image_gradient_unary_energy(
        const DSM& dsm,
        double default_energy, double ponderation_dsm) :
	m_defaultEnergy(default_energy), 
	m_ponderation_dsm(ponderation_dsm),
	m_dsm(dsm)
{
}

#endif
