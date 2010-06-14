#ifndef IMAGE_GRADIENT_UNARY_ENERGY_HPP_
#define IMAGE_GRADIENT_UNARY_ENERGY_HPP_

template<typename DSM>
class image_gradient_unary_energy
{
public:
	typedef double result_type;

	template<typename T>
	result_type operator()(const T &t) const;

	image_gradient_unary_energy(const DSM& dsm,
		double default_energy, double ponderation_dsm);

private:
	DSM m_dsm;
	double m_defaultEnergy, m_ponderation_dsm;
};


#endif /*IMAGE_GRADIENT_UNARY_ENERGY_HPP_*/
