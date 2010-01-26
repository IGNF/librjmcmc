#ifndef GLOBAL_RECONSTRUCTION_ENERGIES_HPP_
#define GLOBAL_RECONSTRUCTION_ENERGIES_HPP_

#include "geometry/geometry.h"

////////////////////////////////////////////////////////////////////////////////

#include <boost/shared_ptr.hpp>
namespace rjmcmc { class image; };

class global_reconstruction_unary_energy
{
public:
	typedef double result_type;
	result_type operator()(const Rectangle_2 &n) const;
	result_type operator()(const Circle_2 &c) const;

	global_reconstruction_unary_energy(double default_energy, const std::string& file);

private:
	boost::shared_ptr<rjmcmc::image> m_image;
	double m_defaultEnergy;
};

#endif /*GLOBAL_RECONSTRUCTION_ENERGIES_HPP_*/
