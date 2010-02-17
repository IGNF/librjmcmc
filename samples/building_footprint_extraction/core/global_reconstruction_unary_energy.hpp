#ifndef GLOBAL_RECONSTRUCTION_ENERGIES_HPP_
#define GLOBAL_RECONSTRUCTION_ENERGIES_HPP_

#include "geometry/geometry.h"
#include <boost/shared_ptr.hpp>
namespace rjmcmc { class image; class gradient_image; };

class global_reconstruction_unary_energy
{
public:
	typedef double result_type;
	result_type operator()(const Rectangle_2 &n) const;
	result_type operator()(const Circle_2 &c) const;

	global_reconstruction_unary_energy(double default_energy,
		double gradient_ponderation, double ndvi_ponderation);

	template<typename View>
	void gradient(const View& view, const Iso_Rectangle_2& bbox, double sigmaD=1, unsigned int step=0);

	template<typename View>
	void ndvi(const View& view, const Iso_Rectangle_2& bbox, unsigned int step=0);

	void gradient(const std::string& s, const Iso_Rectangle_2& bbox, double sigmaD=1, unsigned int step=0);

	void ndvi(const std::string& s, const Iso_Rectangle_2& bbox, unsigned int step=0);

private:
	boost::shared_ptr<rjmcmc::gradient_image> m_gradient;
	boost::shared_ptr<rjmcmc::image> m_ndvi;
	double m_defaultEnergy;
	double m_ponderation_gradient;
	double m_ponderation_ndvi;
};

#endif /*GLOBAL_RECONSTRUCTION_ENERGIES_HPP_*/
