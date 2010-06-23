#ifndef __STEP_TEMPERATURE_HPP__
#define __STEP_TEMPERATURE_HPP__

namespace rjmcmc {

template<typename Temperature>
class step_temperature {
        typedef step_temperature<Temperature> self;
public:
	step_temperature(int n, const Temperature& temp) : m_temp(temp), m_t(*m_temp), m_i(n), m_n(n) {}
	inline double  operator*() const { return m_t; }
	inline self operator++()    {
          --m_i;
          if(!m_i) { m_i=m_n; ++m_temp; m_t=*m_temp; }
          return *this;
        }
	inline self  operator++(int) {
          self t(*this); 
          --m_i;
          if(!m_i) { m_i=m_n; ++m_temp; m_t=*m_temp; }
          return t;
        }

private:
	Temperature m_temp;
	double m_t;
	int m_i, m_n;
};

}; // namespace rjmcmc

#endif // __STEP_TEMPERATURE_HPP__
