#ifndef __LOGARITHMIC_TEMPERATURE_HPP__
#define __LOGARITHMIC_TEMPERATURE_HPP__

namespace rjmcmc {

class logarithmic_temperature {
public:
	logarithmic_temperature(double temp) : m_temp(temp), m_n(2) {}
	inline double  operator*() const { return m_temp/log2(m_n); }
	inline logarithmic_temperature  operator++()    { ++m_n; return *this; }
	inline logarithmic_temperature  operator++(int) { logarithmic_temperature t(*this); ++m_n; return t; }

private:
	double m_temp;
	double m_n;
};

}; // namespace rjmcmc

#endif // __LOGARITHMIC_TEMPERATURE_HPP__
