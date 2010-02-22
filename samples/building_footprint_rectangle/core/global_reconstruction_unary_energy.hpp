#ifndef GLOBAL_RECONSTRUCTION_ENERGIES_HPP_
#define GLOBAL_RECONSTRUCTION_ENERGIES_HPP_

#include <boost/shared_ptr.hpp>
#include "core/geometry.h"
namespace rjmcmc { class image; class gradient_image; };

class global_reconstruction_unary_energy
{
public:
	typedef double result_type;
	result_type operator()(const Rectangle_2 &n) const;
	result_type operator()(const Circle_2 &c) const;

	global_reconstruction_unary_energy(double default_energy,
                double gradient_ponderation);

	template<typename View>
        void gradient(const View& view, const Iso_rectangle_2& bbox, double sigmaD=1, unsigned int step=0);

        void gradient(const std::string& s, const Iso_rectangle_2& bbox, double sigmaD=1, unsigned int step=0);

private:
        boost::shared_ptr<rjmcmc::gradient_image> m_gradient;
	double m_defaultEnergy;
        double m_ponderation_gradient;
};

#endif /*GLOBAL_RECONSTRUCTION_ENERGIES_HPP_*/
