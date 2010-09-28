#ifndef __STEP_SCHEDULE_HPP__
#define __STEP_SCHEDULE_HPP__

namespace simulated_annealing {

    //  step_temp(i) = template_temp( floor(i/n) )


    template<typename Schedule>
    class step_schedule {
        typedef step_schedule<Schedule> self;
    public:
        typedef std::input_iterator_tag iterator_category;
        typedef typename Schedule::value_type value_type;
        typedef std::ptrdiff_t difference_type;
        typedef T*             pointer;
        typedef T&             reference;

        bool operator==(const self& s) const { return (m_i==s.m_i) && (m_n==s.m_n) && (m_t==s.m_t) && (m_schedule==s.m_schedule); }
        bool operator!=(const self& s) const { return !(*this==s); }

	step_schedule(int n, const Schedule& schedule) 
            : m_schedule(schedule), m_t(*m_schedule), m_i(n), m_n(n) {}
        inline value_type  operator*() const { return m_t; }
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
        value_type m_t;
	int m_i, m_n;
    };

}; // namespace rjmcmc

#endif // __STEP_SCHEDULE_HPP__
