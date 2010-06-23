#ifndef __GEOMETRIC_SCHEDULE_HPP__
#define __GEOMETRIC_SCHEDULE_HPP__

namespace rjmcmc {

// T(i) = T(0) * (coef ^ i)

class geometric_schedule {
public:
	geometric_schedule(double temp, double coefficient) : m_temp(temp), m_coefficient(coefficient) {}
	inline double  operator*() const { return m_temp; }
	inline geometric_schedule  operator++()    { m_temp *= m_coefficient; return *this; }
	inline geometric_schedule  operator++(int) { geometric_schedule t(*this); m_temp *= m_coefficient; return t; }

	inline double coefficient() const { return m_coefficient; }
	inline void coefficient(double coef) { m_coefficient = coef; }

private:
	double m_temp;
	double m_coefficient;
};

}; // namespace rjmcmc

#endif // __GEOMETRIC_SCHEDULE_HPP__
