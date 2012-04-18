#ifndef GLOBAL_RECONSTRUCTION_UNARY_ENERGY_INC_HPP_
#define GLOBAL_RECONSTRUCTION_UNARY_ENERGY_INC_HPP_

#include "core/global_reconstruction_unary_energy.hpp"
#include "geometry/integrated_flux/all.hpp"
#include "geometry/image_error/all.hpp"

template<typename DSM, typename NDVI>
global_reconstruction_unary_energy<DSM,NDVI>::global_reconstruction_unary_energy(const DSM& dsm, const NDVI& ndvi, double default_energy, double ponderation_dsm, double ponderation_ndvi) :
	m_dsm(dsm),
	m_ndvi(ndvi),
	m_defaultEnergy(default_energy), 
	m_ponderation_dsm(ponderation_dsm),
	m_ponderation_ndvi(ponderation_ndvi)
{
}

template<typename DSM, typename NDVI>
template<typename T>
double global_reconstruction_unary_energy<DSM,NDVI>::operator()(const T &t) const
{
	return m_defaultEnergy
	- m_ponderation_dsm  * integrated_flux(m_dsm,t)
    + m_ponderation_ndvi * image_error(m_ndvi,t);
}

#endif
