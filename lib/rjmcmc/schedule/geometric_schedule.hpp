#ifndef __GEOMETRIC_SCHEDULE_HPP__
#define __GEOMETRIC_SCHEDULE_HPP__

namespace simulated_annealing {
    /**
 * \ingroup GroupSchedule
 */
    /**
 * Geometric schedule class.
 * This class implements a simple geometric schedule class:
 * \f[T_i=T_0*m_coefficient^i\f]
 */
    template<typename T>
    class geometric_schedule {
        typedef geometric_schedule<T> self;

    public:
        typedef std::input_iterator_tag iterator_category;
        typedef T              value_type;
        typedef std::ptrdiff_t difference_type;
        typedef T*             pointer;
        typedef T&             reference;

        bool operator==(const self& s) const { return (m_temp==s.m_temp) && (m_coefficient==s.m_coefficient); }
        bool operator!=(const self& s) const { return !(*this==s); }

        /// Constructor
        /// @param temp Initial temperature
        /// @param coefficient Geometric decrease coefficient
        geometric_schedule(value_type temp, value_type coefficient) : m_temp(temp), m_coefficient(coefficient) {}
        inline value_type  operator*() const { return m_temp; }
        inline self  operator++()    { m_temp *= m_coefficient; return *this; }
        inline self  operator++(int) { self t(*this); m_temp *= m_coefficient; return t; }

        inline value_type coefficient() const { return m_coefficient; }
        inline void coefficient(value_type coef) { m_coefficient = coef; }

    private:
        /// Current temperature
        value_type m_temp;
        /// Geometric decrease coefficient
        value_type m_coefficient;
    };

}; // namespace simulated_annealing

#endif // __GEOMETRIC_SCHEDULE_HPP__
