#include "core/global_reconstruction_unary_energy.hpp"
#include "core/image.hpp"

////////////////////////////////////////////////////////////////////////////////

double global_reconstruction_unary_energy::operator()(const Circle_2 &c) const
{
	return m_defaultEnergy + m_image->error(c);
}

double global_reconstruction_unary_energy::operator()(const Rectangle_2 &n) const
{
	return m_defaultEnergy + m_image->error(n);
}


global_reconstruction_unary_energy::global_reconstruction_unary_energy(double default_energy, const std::string& file) :
	m_defaultEnergy(default_energy), 
	m_image(boost::shared_ptr<rjmcmc::image>(new rjmcmc::image))
{
	m_image->load(file);
}

