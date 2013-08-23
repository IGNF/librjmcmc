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

#ifndef RJMCMC_CONFIGURATION_HPP
#define RJMCMC_CONFIGURATION_HPP

#include <vector>

namespace marked_point_process {
    //////////////////////////////////////////////////////////
    
    struct trivial_accelerator {
	template<typename C, typename T> std::pair<typename C::iterator,typename C::iterator> operator()(const C &c, const T &t) const {
            return std::make_pair(c.begin(),c.end());
	}
    };
    
    namespace internal {
        template<typename C> struct inserter {
            C& c_;
            inserter(C& c) : c_(c) {}
            template<typename T> void operator()(T& t) const { c_.insert(t); }
        };
        template<typename C> struct remover {
            C& c_;
            remover(C& c) : c_(c) {}
            template<typename T> void operator()(T& t) const { c_.remove(t); }
        };
        
        
        template<
                typename Configuration,
                typename Birth = std::vector<typename Configuration::value_type>,
                typename Death = std::vector<typename Configuration::const_iterator >
        >
        class modification
        {
        public:
            typedef	Birth birth_type;
            typedef Death death_type;
            const birth_type& birth() const { return m_birth; }
            const death_type& death() const { return m_death; }
            birth_type& birth() { return m_birth; }
            death_type& death() { return m_death; }

            // manipulators
            inline void apply(Configuration &c) const
            {
                std::for_each(m_death.begin(),m_death.end(),internal::remover <Configuration>(c));
                std::for_each(m_birth.begin(),m_birth.end(),internal::inserter<Configuration>(c));
            }

        private:
            birth_type	m_birth;
            death_type	m_death;
        };

    }; // namespace internal

}; // namespace marked_point_process

#endif // RJMCMC_CONFIGURATION_HPP
