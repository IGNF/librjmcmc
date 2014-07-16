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

#ifndef COMPOSITE_VISITOR_HPP
#define COMPOSITE_VISITOR_HPP

#include "rjmcmc/tuple.hpp"

namespace simulated_annealing {

    namespace internal {

        struct visitor_init
        {
            int m_dump, m_save;
            visitor_init(int dump, int save) : m_dump(dump), m_save(save) {}
            template<typename T> inline void operator()(T& t) { t .init(m_dump,m_save); }
            template<typename T> inline void operator()(T* t) { t->init(m_dump,m_save); }
        };

        template<typename Configuration, typename Sampler>
        struct visitor_begin
        {
            const Configuration& m_config;
            const Sampler& m_sample;
            double m_t;
            visitor_begin(const Configuration& config, const Sampler& sample, double t) : m_config(config), m_sample(sample), m_t(t) {}
            template<typename T> inline void operator()(T& t) { t .begin(m_config,m_sample,m_t); }
            template<typename T> inline void operator()(T* t) { t->begin(m_config,m_sample,m_t); }
        };
        template<typename Configuration, typename Sampler>
        struct visitor_visit
        {
            const Configuration& m_config;
            const Sampler& m_sample;
            double m_t;
            visitor_visit(const Configuration& config, const Sampler& sample, double t) : m_config(config), m_sample(sample), m_t(t) {}
            template<typename T> inline void operator()(T& t) { t .visit(m_config,m_sample,m_t); }
            template<typename T> inline void operator()(T* t) { t->visit(m_config,m_sample,m_t); }
        };
        template<typename Configuration, typename Sampler>
        struct visitor_end
        {
            const Configuration& m_config;
            const Sampler& m_sample;
            double m_t;
            visitor_end(const Configuration& config, const Sampler& sample, double t) : m_config(config), m_sample(sample), m_t(t) {}
            template<typename T> inline void operator()(T& t) { t .end(m_config,m_sample,m_t); }
            template<typename T> inline void operator()(T* t) { t->end(m_config,m_sample,m_t); }
        };

    } // namespace internal

    template<RJMCMC_TUPLE_TYPENAMES>
    class composite_visitor
    {
    public:
        typedef rjmcmc::tuple<RJMCMC_TUPLE_TYPES> Visitors;
        composite_visitor(RJMCMC_TUPLE_ARGS) : m_visitors(RJMCMC_TUPLE_PARAMS) {}

        void init(int dump, int save)
        {
            internal::visitor_init v(dump,save);
            rjmcmc::for_each(m_visitors,v);
        }

        template<typename Configuration, typename Sampler>
        void begin(const Configuration& config, const Sampler& sample, double t)
        {
            internal::visitor_begin<Configuration,Sampler> v(config,sample,t);
            rjmcmc::for_each(m_visitors,v);
        }

        template<typename Configuration, typename Sampler>
        void visit(const Configuration& config, const Sampler& sample, double t)
        {
            internal::visitor_visit<Configuration,Sampler> v(config,sample,t);
            rjmcmc::for_each(m_visitors,v);
        }

        template<typename Configuration, typename Sampler>
        void end(const Configuration& config, const Sampler& sample, double t)
        {
            internal::visitor_end<Configuration,Sampler> v(config,sample,t);
            rjmcmc::for_each(m_visitors,v);
        }
    private:
        Visitors m_visitors;
    };

/*
    template<RJMCMC_TUPLE_TYPENAMES>
    composite_visitor<RJMCMC_TUPLE_TYPES> make_visitor(RJMCMC_TUPLE_ARGS)
    {
        return composite_visitor(RJMCMC_TUPLE_PARAMS);
    }
*/
} // namespace simulated_annealing

#endif // COMPOSITE_VISITOR_HPP
