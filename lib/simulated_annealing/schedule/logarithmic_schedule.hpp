/***********************************************************************
This file is part of the librjmcmc project source files.

Copyright : Institut Geographique National (2008-2012)
Contributors : Mathieu Brédif, Olivier Tournaire, Didier Boldo
email : librjmcmc@ign.fr

This software is a generic C++ library for stochastic optimization.

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software. You can use,
modify and/or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty and the software's author, the holder of the
economic rights, and the successive licensors have only limited liability.

In this respect, the user's attention is drawn to the risks associated
with loading, using, modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean that it is complicated to manipulate, and that also
therefore means that it is reserved for developers and experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and, more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

***********************************************************************/

#ifndef LOGARITHMIC_SCHEDULE_HPP
#define LOGARITHMIC_SCHEDULE_HPP

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

#endif // LOGARITHMIC_SCHEDULE_HPP
