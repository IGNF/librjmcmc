#include "core/image_gradient_unary_energy.hpp"
#include "core/image.hpp"

double image_gradient_unary_energy::operator()(const Circle_2 &c) const
{
	return m_defaultEnergy - m_image->integrated_flux(c);
}

double image_gradient_unary_energy::operator()(const Rectangle_2 &n) const
{
	double res = m_defaultEnergy;
	for (unsigned int i = 0; i < 4; ++i)
	{
		res -= std::max(0.,m_image->integrated_flux(n.segment(i)));
	}
	return res;
}



image_gradient_unary_energy::image_gradient_unary_energy(double default_energy, const std::string& file, const bbox_2& bbox, double sigmaD, unsigned int step) :
	m_defaultEnergy(default_energy), 
	m_image(boost::shared_ptr<rjmcmc::image>(new rjmcmc::image))
{
	m_image->load(file,bbox,sigmaD,step);
}

image_gradient_unary_energy::image_gradient_unary_energy(double default_energy, const std::string& file, double sigmaD, unsigned int step) :
	m_defaultEnergy(default_energy), 
	m_image(boost::shared_ptr<rjmcmc::image>(new rjmcmc::image))
{
	m_image->load(file,sigmaD,step);
}

