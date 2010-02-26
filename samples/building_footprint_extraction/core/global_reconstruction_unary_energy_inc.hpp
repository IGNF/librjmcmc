#ifndef GLOBAL_RECONSTRUCTION_UNARY_ENERGY_INC_HPP_
#define GLOBAL_RECONSTRUCTION_UNARY_ENERGY_INC_HPP_

#include "core/global_reconstruction_unary_energy.hpp"
#include "image/gradient_image.hpp"
#include "image/image.hpp"

template<typename View, typename IsoRectangle>
void global_reconstruction_unary_energy::gradient(const View& view, const IsoRectangle& bbox, double sigmaD, unsigned int step)
{
	m_gradient->load(view,bbox,sigmaD,step);
}


template<typename View, typename IsoRectangle>
void global_reconstruction_unary_energy::ndvi(const View& view, const IsoRectangle& bbox, unsigned int step)
{
	m_ndvi->load(view,bbox,step);
}

template<typename T>
double global_reconstruction_unary_energy::operator()(const T &t) const
{
	return m_defaultEnergy
	- m_ponderation_gradient*(*m_gradient)(t)
	+ m_ponderation_ndvi*m_ndvi->error(t);
}

#endif
