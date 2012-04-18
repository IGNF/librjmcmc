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

#include "rjmcmc/variant.hpp"

namespace marked_point_process {
    //////////////////////////////////////////////////////////

    struct trivial_accelerator {
	template<typename C, typename T> std::pair<typename C::iterator,typename C::iterator> operator()(const C &c, const T &t) const {
            return std::make_pair(c.begin(),c.end());
	}
    };

    //////////////////////////////////////////////////////////
    namespace internal {

        template<class Configuration>
        class modification
        {
            typedef typename Configuration::value_type	value_type;
            typedef typename Configuration::iterator	iterator;
            std::vector<value_type>	m_birth;
            std::vector<iterator>	m_death;
        public:
            typedef typename std::vector<value_type>::const_iterator	birth_const_iterator;
            typedef typename std::vector<iterator>::const_iterator	death_const_iterator;

            inline birth_const_iterator birth_begin() const { return m_birth.begin(); }
            inline birth_const_iterator birth_end  () const { return m_birth.end  (); }
            inline death_const_iterator death_begin() const { return m_death.begin(); }
            inline death_const_iterator death_end  () const { return m_death.end  (); }
            template<typename T>
            inline void insert_birth(const T& b) { m_birth.push_back(value_type(b)); }
            inline void insert_death(iterator d)  { m_death.push_back(d); }
            inline unsigned int birth_size() const { return m_birth.size(); }
            inline unsigned int death_size() const { return m_death.size(); }
            inline int delta_size() const { return (int) birth_size() - (int) death_size(); }
            inline void clear() { m_birth.clear(); m_death.clear(); }


            template<typename V> class inserter {
                V& m_v;
            public:
		inserter(V& v) : m_v(v) {}
		typedef void result_type;
                template<typename T> void operator()(const T& t) { m_v.push_back(value_type(t)); }
                template<typename T, typename U> void operator()(const std::pair<T,U>& t) {
                    m_v.push_back(value_type(t.first));
                    m_v.push_back(value_type(t.second));
		}
            };

            inline inserter<std::vector<value_type> > birth_inserter() {
		return inserter<std::vector<value_type> > (m_birth);
            }
            inline inserter<std::vector<iterator> > death_inserter() {
		return inserter<std::vector<iterator> >(m_death);
            }
        };

    }; // namespace internal

    // audit log
    template<typename Configuration>
    bool audit_configuration(const Configuration& c, std::ostream &out = std::cout, double epsilon = 1e-6)
    {
	bool res = true;
	double delta1 = std::abs(c.audit_unary_energy()-c.unary_energy());
	if (delta1 > epsilon)
	{
            out << "Ecart energie donnees : "<< delta1 << std::endl;
            res = false;
	}
	double delta2 = std::abs(c.audit_binary_energy()-c.binary_energy());
	if (delta2 > epsilon)
	{
            out << "Ecart energie a priori : " << delta2 << std::endl;
            res = false;
	}
	unsigned int nb_err = c.audit_structure();
	if (nb_err != 0)
	{
            out << "Nombre d'erreurs de structure du graph : " << nb_err << std::endl;
            res = false;
	}
	return res;
    }

}; // namespace marked_point_process

#endif // RJMCMC_CONFIGURATION_HPP
