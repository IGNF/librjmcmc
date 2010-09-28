#ifndef __GEOMETRIC_SCHEDULE_HPP__
#define __GEOMETRIC_SCHEDULE_HPP__

namespace simulated_annealing {
    /**
 * \ingroup GroupSchedule
 */
    /**
 * This class is a model of the Schedule concept and implements a geometric schedule :
 * \f[T_i=T_0*\alpha^i\f]
 * This schedule does not guarantee that the simulated annealing will converge to the global optimum.
 * It is however the most commonly used schedule with a decrease coefficient close to 1 (e.g. 0.99999).
 */
    template<typename T>
    class geometric_schedule {
    public:
        typedef std::input_iterator_tag iterator_category;
        typedef T                       value_type;
        typedef std::ptrdiff_t          difference_type;
        typedef value_type*             pointer;
        typedef value_type&             reference;

        bool operator==(const geometric_schedule<T>& s) const { return (m_temp==s.m_temp) && (m_alpha==s.m_alpha); }
        bool operator!=(const geometric_schedule<T>& s) const { return !(*this==s); }

        /// @param temp Initial temperature
        /// @param alpha Geometric decrease coefficient
        geometric_schedule(value_type temp, value_type alpha) : m_temp(temp), m_alpha(alpha) {}

        inline value_type  operator*() const { return m_temp; }
        inline geometric_schedule<T>& operator++()    { m_temp *= m_alpha; return *this; }
        inline geometric_schedule<T>  operator++(int) { geometric_schedule<T> t(*this); m_temp *= m_alpha; return t; }

        inline value_type alpha() const { return m_alpha; }
        inline void alpha(value_type a) { m_alpha = a; }

    private:
        // Current temperature
        value_type m_temp;
        // Geometric decrease coefficient
        value_type m_alpha;
    };

}; // namespace simulated_annealing

#endif // __GEOMETRIC_SCHEDULE_HPP__
