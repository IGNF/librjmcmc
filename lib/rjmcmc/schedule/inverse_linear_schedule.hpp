#ifndef __INVERSE_LINEAR_SCHEDULE_HPP__
#define __INVERSE_LINEAR_SCHEDULE_HPP__

namespace rjmcmc {

// 1/T(i+1) = 1/T(i) + delta

class inverse_linear_schedule {
public:
	inverse_linear_schedule(double temp, double delta) : m_temp(temp), m_delta(delta) {}
	inline double  operator*() const { return m_temp; }
	inline inverse_linear_schedule  operator++()    { m_temp/=(1.+m_delta*m_temp); return *this; }
	inline inverse_linear_schedule  operator++(int) {
          inverse_linear_schedule t(*this);
          m_temp/=(1.+m_delta*m_temp);
          return t;
        }

private:
	double m_temp;
	double m_delta;
};

}; // namespace rjmcmc

#endif // __INVERSE_LINEAR_SCHEDULE_HPP__
