#ifndef DELTA_ENERGY_END_TEST
#define DELTA_ENERGY_END_TEST

namespace simulated_annealing
{
    /**
 * \ingroup GroupEndTest
 */
    /**
 * This class is a model of the EndTest concept and stops the ... TODO
 */

    class delta_energy_end_test {
    public:
        delta_energy_end_test(unsigned int n) : m_i(0), m_n(n) {}
        template<typename Configuration, typename Sampler>
        inline bool operator()(const Configuration&, const Sampler& s, double) {
            m_i = (s.delta()==0)? (m_i+1) : 0;
            return m_i<m_n;
        }
        void stop () { m_n=0; }
    private:
        unsigned int m_i, m_n;
    };

};

#include <rjmcmc/tuple.hpp>

namespace simulated_annealing
{
    namespace internal
    {
        template<typename Configuration, typename Sampler>
        struct end_test_predicate
        {
            const Configuration& m_config;
            const Sampler& m_sample;
            double m_t;
            end_test_predicate(const Configuration& config, const Sampler& sample, double t)
                : m_config(config), m_sample(sample), m_t(t) {}
            template<typename T> bool operator()(const T& t) { return t(m_config,m_sample,m_t); }
            template<typename T> bool operator()(T* t) { (*t)(m_config,m_sample,m_t); }
        };
    }

    template<RJMCMC_TUPLE_TYPENAMES>
    class composite_end_test
    {
    public:
        typedef rjmcmc::tuple<RJMCMC_TUPLE_TYPES> EndTests;
        composite_visitor(RJMCMC_TUPLE_ARGS) : m_end_tests(RJMCMC_TUPLE_PARAMS) {}

        template<typename Configuration, typename Sampler>
        inline bool operator()(const Configuration&, const Sampler& s, double t)
        {
            internal::end_test_predicate<Configuration,Sampler> v(config,sample,t);
            rjmcmc::for_each(m_end_tests,v);
        }
    private:
        EndTests m_end_tests;
    };


};

#endif // DELTA_ENERGY_END_TEST
