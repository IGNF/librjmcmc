#ifndef COMPOSITE_VISITOR_HPP
#define COMPOSITE_VISITOR_HPP

#include "rjmcmc/tuple.hpp"

namespace rjmcmc {

    namespace internal {

        struct visitor_init
        {
            int m_dump, m_save;
            visitor_init(int dump, int save) : m_dump(dump), m_save(save) {}
            template<typename T> void operator()(const T& t) { t .init(m_dump,m_save); }
            template<typename T> void operator()(T* t) { t->init(m_dump,m_save); }
        };

        template<typename Configuration, typename Sampler>
        struct visitor_begin
        {
            const Configuration& m_config;
            const Sampler& m_sample;
            double m_t;
            visitor_begin(const Configuration& config, const Sampler& sample, double t) : m_config(config), m_sample(sample), m_t(t) {}
            template<typename T> void operator()(const T& t) { t .begin(m_config,m_sample,m_t); }
            template<typename T> void operator()(T* t) { t->begin(m_config,m_sample,m_t); }
        };
        template<typename Configuration, typename Sampler>
        struct visitor_end
        {
            const Configuration& m_config;
            const Sampler& m_sample;
            double m_t;
            visitor_end(const Configuration& config, const Sampler& sample, double t) : m_config(config), m_sample(sample), m_t(t) {}
            template<typename T> void operator()(const T& t) { t .end(m_config,m_sample,m_t); }
            template<typename T> void operator()(T* t) { t->end(m_config,m_sample,m_t); }
        };

        template<typename Configuration, typename Sampler>
        struct visitor_iterate
        {
            const Configuration& m_config;
            const Sampler& m_sample;
            double m_t;
            unsigned int m_i;
            bool m_bool;
            visitor_iterate(const Configuration& config, const Sampler& sample, double t, unsigned int i) : m_config(config), m_sample(sample), m_t(t), m_i(i), m_bool(true) {}
            template<typename T> void operator()(const T& t) { m_bool = m_bool && t .iterate(m_config,m_sample,m_t,m_i); }
            template<typename T> void operator()(T* t) { m_bool = m_bool && t->iterate(m_config,m_sample,m_t,m_i); }
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
        bool iterate(const Configuration& config, const Sampler& sample, double t, unsigned int i)
        {
            internal::visitor_iterate<Configuration,Sampler> v(config,sample,t,i);
            rjmcmc::for_each(m_visitors,v);
            return v.m_bool;
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

}

#endif // COMPOSITE_VISITOR_HPP
