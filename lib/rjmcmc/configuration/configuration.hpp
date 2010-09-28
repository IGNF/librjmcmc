#ifndef __RJMCMC_CONFIGURATION_HPP__
#define __RJMCMC_CONFIGURATION_HPP__

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
            inline void clear() { m_birth.clear(); m_death.clear(); }


            template<typename V> class inserter {
		mutable V& m_v;
            public:
		inserter(V& v) : m_v(v) {}
		typedef void result_type;
		template<typename T> void operator()(const T& t) const { m_v.push_back(value_type(t)); }
		template<typename T, typename U> void operator()(const std::pair<T,U>& t) const {
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

#endif // __RJMCMC_CONFIGURATION_HPP__
