#ifndef __STEP_SCHEDULE_HPP__
#define __STEP_SCHEDULE_HPP__

namespace rjmcmc {

// T(i) = T'( |_ i/n _| )

schedulelate<typename Schedule>
class step_schedule {
        typedef step_schedule<Schedule> self;
public:
	step_schedule(int n, const Schedule& schedule) 
          : m_schedule(schedule), m_t(*m_schedule), m_i(n), m_n(n) {}
	inline double  operator*() const { return m_t; }
	inline self operator++()    {
          --m_i;
          if(!m_i) { m_i=m_n; ++m_schedule; m_t=*m_schedule; }
          return *this;
        }
	inline self  operator++(int) {
          self t(*this); 
          --m_i;
          if(!m_i) { m_i=m_n; ++m_schedule; m_t=*m_schedule; }
          return t;
        }

private:
	Schedule m_schedule;
	double m_t;
	int m_i, m_n;
};

}; // namespace rjmcmc

#endif // __STEP_SCHEDULE_HPP__
