#ifndef __LOGARITHMIC_SCHEDULE_HPP__
#define __LOGARITHMIC_SCHEDULE_HPP__

namespace simulated_annealing {

    // temp(i) = temp(0)/log2(2+i)

    template<typename T>
    class logarithmic_schedule {
        typedef logarithmic_schedule<T> self;

    public:
        typedef std::input_iterator_tag iterator_category;
        typedef T              value_type;
        typedef std::ptrdiff_t difference_type;
        typedef T*             pointer;
        typedef T&             reference;

        bool operator==(const self& s) const { return (m_temp==s.m_temp) && (m_n==s.m_n); }
        bool operator!=(const self& s) const { return !(*this==s); }

        logarithmic_schedule(value_type temp) : m_temp(temp), m_n(2) {}
        inline value_type  operator*() const { return m_temp/log2(m_n); }
	inline logarithmic_schedule  operator++()    { ++m_n; return *this; }
	inline logarithmic_schedule  operator++(int) { logarithmic_schedule t(*this); ++m_n; return t; }

    private:
        value_type m_temp;
        value_type m_n;
    };

}; // namespace simulated_annealing

#endif // __LOGARITHMIC_SCHEDULE_HPP__
