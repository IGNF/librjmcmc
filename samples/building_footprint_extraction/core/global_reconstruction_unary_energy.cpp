#include "core/global_reconstruction_unary_energy.hpp"
#include "image/image.hpp"
#include "image/gradient_image.hpp"

global_reconstruction_unary_energy::global_reconstruction_unary_energy(double default_energy, double ponderation_gradient, double ponderation_ndvi) :
	m_defaultEnergy(default_energy), 
	m_ponderation_gradient(ponderation_gradient),
	m_ponderation_ndvi(ponderation_ndvi), 
	m_gradient(boost::shared_ptr<rjmcmc::gradient_image>(new rjmcmc::gradient_image)),
	m_ndvi(boost::shared_ptr<rjmcmc::image>(new rjmcmc::image))
{
}

