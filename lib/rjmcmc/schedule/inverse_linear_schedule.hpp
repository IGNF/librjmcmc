#ifndef __INVERSE_LINEAR_SCHEDULE_HPP__
#define __INVERSE_LINEAR_SCHEDULE_HPP__

namespace simulated_annealing {
    /**
 * \ingroup GroupSchedule
 */
    /**
 * Inverse linear schedule class.
 * This class implements a inverse linear schedule class:
 * \f[\frac{1}{T_{i+1}}=\frac{1}{T_i}+\delta\f]
 */
    template<typename T>
    class inverse_linear_schedule {
        typedef inverse_linear_schedule<T> self;

    public:
        typedef std::input_iterator_tag iterator_category;
        typedef T              value_type;
        typedef std::ptrdiff_t difference_type;
        typedef T*             pointer;
        typedef T&             reference;

        bool operator==(const self& s) const { return (m_temp==s.m_temp) && (m_delta==s.m_delta); }
        bool operator!=(const self& s) const { return !(*this==s); }

        inverse_linear_schedule(value_type temp, value_type delta) : m_temp(temp), m_delta(delta) {}
        inline value_type  operator*() const { return m_temp; }
	inline inverse_linear_schedule  operator++()    { m_temp/=(1.+m_delta*m_temp); return *this; }
	inline inverse_linear_schedule  operator++(int) {
            inverse_linear_schedule t(*this);
            m_temp/=(1.+m_delta*m_temp);
            return t;
        }

    private:
        value_type m_temp;
        value_type m_delta;
    };

}; // namespace simulated_annealing

#endif // __INVERSE_LINEAR_SCHEDULE_HPP__
