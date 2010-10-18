#ifndef __LOGARITHMIC_SCHEDULE_HPP__
#define __LOGARITHMIC_SCHEDULE_HPP__

namespace simulated_annealing {
    /**
     * \ingroup GroupSchedule
     *
     * This class is a model of the Schedule concept and implements a logarithmic schedule:
     * \f[T_i=\frac{T_0}{\log_2(2+i)}\f]
     * This schedule guarantees that the simulated annealing will converge to the global optimum
     * if the initial temperature is set sufficiently high.
     * It is however usually too slow to converge in practice, leading to the usage of faster but suboptimal schedules.
     */
    template<typename T>
    class logarithmic_schedule {
    public:
        typedef std::input_iterator_tag iterator_category;
        typedef T                       value_type;
        typedef std::ptrdiff_t          difference_type;
        typedef value_type*             pointer;
        typedef value_type&             reference;

        bool operator==(const logarithmic_schedule<T>& s) const { return (m_temp==s.m_temp) && (m_n==s.m_n); }
        bool operator!=(const logarithmic_schedule<T>& s) const { return !(*this==s); }

        /// @param temp Initial temperature
        logarithmic_schedule(value_type temp) : m_temp(temp), m_n(2) {}
        inline value_type  operator*() const { return m_temp/log2(m_n); }
        inline logarithmic_schedule<T>& operator++()    { ++m_n; return *this; }
        inline logarithmic_schedule<T>  operator++(int) { logarithmic_schedule t(*this); ++m_n; return t; }

    private:
        // Initial temperature
        value_type m_temp;
        // Iteration count
        value_type m_n;
    };

}; // namespace simulated_annealing

#endif // __LOGARITHMIC_SCHEDULE_HPP__
