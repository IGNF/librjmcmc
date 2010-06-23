#ifndef __LOGARITHMIC_SCHEDULE_HPP__
#define __LOGARITHMIC_SCHEDULE_HPP__

namespace rjmcmc {

// T(i) = T(0)/log2(2+i)

class logarithmic_schedule {
public:
	logarithmic_schedule(double temp) : m_temp(temp), m_n(2) {}
	inline double  operator*() const { return m_temp/log2(m_n); }
	inline logarithmic_schedule  operator++()    { ++m_n; return *this; }
	inline logarithmic_schedule  operator++(int) { logarithmic_schedule t(*this); ++m_n; return t; }

private:
	double m_temp;
	double m_n;
};

}; // namespace rjmcmc

#endif // __LOGARITHMIC_SCHEDULE_HPP__
