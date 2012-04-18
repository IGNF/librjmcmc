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

#ifndef STEP_SCHEDULE_HPP
#define STEP_SCHEDULE_HPP

namespace simulated_annealing {
    /**
     * \ingroup GroupSchedule
     *
     * This class is a model of the Schedule concept and wraps another schedule by slowing it down n-fold:
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

#endif // STEP_SCHEDULE_HPP
