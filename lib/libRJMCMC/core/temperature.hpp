#ifndef __RJMCMC_TEMPERATURE_HPP__
#define __RJMCMC_TEMPERATURE_HPP__

namespace rjmcmc {

class geometric_temperature {
public:
	geometric_temperature(double temp, double coefficient) : m_temp(temp), m_coefficient(coefficient) {}

	inline double  operator*() const { return m_temp; }
	inline double& operator*()       { return m_temp; }
	inline geometric_temperature  operator++()    { m_temp *= m_coefficient; return *this; }
	inline geometric_temperature  operator++(int) { geometric_temperature t(*this); m_temp *= m_coefficient; return t; }

	inline double coefficient() const { return m_coefficient; }
	inline void coefficient(double coef) { m_coefficient = coef; }

private:
	double m_temp;
	double m_coefficient;
};

}; // namespace rjmcmc

#endif // __RJMCMC_TEMPERATURE_HPP__
