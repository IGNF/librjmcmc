#include "energy/image_gradient_unary_energy_facade_elements.hpp"
#include "image/gradient_image.hpp"

image_gradient_unary_energy_facade_elements::image_gradient_unary_energy_facade_elements(double default_energy, double ponderation_gradient) :
        m_defaultEnergy(default_energy),
        m_ponderation_gradient(ponderation_gradient),
        m_image(boost::shared_ptr<rjmcmc::gradient_image>(new rjmcmc::gradient_image))
{
}
