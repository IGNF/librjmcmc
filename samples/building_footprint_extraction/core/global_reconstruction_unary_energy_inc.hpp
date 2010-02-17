#ifndef GLOBAL_RECONSTRUCTION_UNARY_ENERGY_INC_HPP_
#define GLOBAL_RECONSTRUCTION_UNARY_ENERGY_INC_HPP_

#include "core/global_reconstruction_unary_energy.hpp"
#include "core/gradient_image.hpp"
#include "core/image.hpp"

template<typename View>
void global_reconstruction_unary_energy::gradient(const View& view, const Iso_rectangle_2& bbox, double sigmaD, unsigned int step)
{
	m_gradient->load(view,bbox,sigmaD,step);
}


template<typename View>
void global_reconstruction_unary_energy::ndvi(const View& view, const Iso_rectangle_2& bbox, unsigned int step)
{
	m_ndvi->load(view,bbox,step);
}

void global_reconstruction_unary_energy::gradient(const std::string& s, const Iso_rectangle_2& bbox, double sigmaD, unsigned int step)
{
	m_gradient->load(s,bbox,sigmaD,step);
}

void global_reconstruction_unary_energy::ndvi(const std::string& s, const Iso_rectangle_2& bbox, unsigned int step)
{
	m_ndvi->load(s,bbox,step);
}

#endif
