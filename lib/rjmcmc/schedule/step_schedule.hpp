#ifndef __STEP_SCHEDULE_HPP__
#define __STEP_SCHEDULE_HPP__

namespace simulated_annealing {

    /**
 * \ingroup GroupSchedule
 */
    /**
 * This class is a model of the Schedule concept and wraps another schedule by slowing it down n-fold :
 * \f[T'_i=T_{\left\lfloor\frac{i}{n}\right\rfloor}\f]
 * This schedule is used to facilitate the convergence of the sampler at a fixed temperature
 * as the temperature decrease is broken down into plateaus of size n.
 */

    template<typename Schedule>
    class step_schedule {
    public:
        typedef std::input_iterator_tag            iterator_category;
        typedef typename Schedule::value_type      value_type;
        typedef typename Schedule::difference_type difference_type;
        typedef typename Schedule::pointer         pointer;
        typedef typename Schedule::reference       reference;

        bool operator==(const step_schedule<Schedule>& s) const { return (m_i==s.m_i) && (m_n==s.m_n) && (m_t==s.m_t) && (m_schedule==s.m_schedule); }
        bool operator!=(const step_schedule<Schedule>& s) const { return !(*this==s); }

        /// @param n Iteration count between wrapped schedule incrementations
        /// @param schedule Wrapped schedule
	step_schedule(int n, const Schedule& schedule) 
            : m_schedule(schedule), m_t(*m_schedule), m_i(n), m_n(n) {}
        inline value_type  operator*() const { return m_t; }
        inline step_schedule<Schedule>& operator++()    {
            --m_i;
            if(!m_i) { m_i=m_n; ++m_schedule; m_t=*m_schedule; }
            return *this;
        }
        inline step_schedule<Schedule>  operator++(int) {
            step_schedule<Schedule> t(*this);
            --m_i;
            if(!m_i) { m_i=m_n; ++m_schedule; m_t=*m_schedule; }
            return t;
        }

    private:
        // Wrapped schedule
	Schedule m_schedule;
        // Current temperature
        value_type m_t;
        // Iteration counts
	int m_i, m_n;
    };

}; // namespace rjmcmc

#endif // __STEP_SCHEDULE_HPP__
