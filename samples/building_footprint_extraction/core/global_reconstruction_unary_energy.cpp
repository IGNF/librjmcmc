#include "core/global_reconstruction_unary_energy.hpp"
#include "image/image.hpp"
#include "image/gradient_image.hpp"

////////////////////////////////////////////////////////////////////////////////

double global_reconstruction_unary_energy::operator()(const Circle_2 &c) const
{
	return m_defaultEnergy - m_ponderation_gradient*m_gradient->integrated_flux(c) + m_ponderation_ndvi*m_ndvi->error(c);
}

double global_reconstruction_unary_energy::operator()(const Rectangle_2 &n) const
{
	double res = m_defaultEnergy;
	for (unsigned int i = 0; i < 4; ++i)
	{
		res -= m_ponderation_gradient*std::max(0.,m_gradient->integrated_flux(n.segment(i)));
	}
	return res + m_ponderation_ndvi*m_ndvi->error(n);
}

global_reconstruction_unary_energy::global_reconstruction_unary_energy(double default_energy, double ponderation_gradient, double ponderation_ndvi) :
	m_defaultEnergy(default_energy), 
	m_ponderation_gradient(ponderation_gradient),
	m_ponderation_ndvi(ponderation_ndvi), 
	m_gradient(boost::shared_ptr<rjmcmc::gradient_image>(new rjmcmc::gradient_image)),
	m_ndvi(boost::shared_ptr<rjmcmc::image>(new rjmcmc::image))
{
}

