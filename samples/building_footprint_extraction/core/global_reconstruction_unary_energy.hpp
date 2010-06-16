#ifndef GLOBAL_RECONSTRUCTION_ENERGIES_HPP_
#define GLOBAL_RECONSTRUCTION_ENERGIES_HPP_

#include <boost/shared_ptr.hpp>

template<typename DSM, typename NDVI>
class global_reconstruction_unary_energy
{
public:
	typedef double result_type;

	template<typename T>
	result_type operator()(const T &t) const;

	global_reconstruction_unary_energy(
		const DSM& dsm, const NDVI& ndvi,
		double default_energy, double ponderation_dsm, double ndvi_ponderation);

private:
	DSM m_dsm;
	NDVI m_ndvi;
	double m_defaultEnergy;
	double m_ponderation_dsm;
	double m_ponderation_ndvi;
};

#endif /*GLOBAL_RECONSTRUCTION_ENERGIES_HPP_*/
