#ifndef COMPOSITE_END_TEST
#define COMPOSITE_END_TEST

#include <rjmcmc/tuple.hpp>

namespace simulated_annealing
{
    namespace internal
    {
        template<typename Configuration, typename Sampler>
        struct end_test_or_predicate
        {
            const Configuration& m_config;
            const Sampler& m_sample;
            double m_t;
            bool m_value;
            bool value() const { return m_value; }
            end_test_predicate(const Configuration& config, const Sampler& sample, double t)
                : m_config(config), m_sample(sample), m_t(t), m_value(false) {}
            template<typename T> void operator()(const T& t) { m_value |= t(m_config,m_sample,m_t); }
            template<typename T> void operator()(T* t) { operator()(*t); }
        };
    }

    /**
     * \ingroup GroupEndTest
     *
     * This class is a model of the EndTest concept and stops
     * the simulated annealing process as soon as one of its encapsulated EndTest models stops.
     *
     */
    template<RJMCMC_TUPLE_TYPENAMES>
    class composite_end_test
    {
    public:
        typedef rjmcmc::tuple<RJMCMC_TUPLE_TYPES> EndTests;
        composite_visitor(RJMCMC_TUPLE_ARGS) : m_end_tests(RJMCMC_TUPLE_PARAMS) {}

        template<typename Configuration, typename Sampler>
        inline bool operator()(const Configuration&, const Sampler& s, double t)
        {
            internal::end_test_or_predicate<Configuration,Sampler> pred(config,sample,t);
            rjmcmc::for_each(m_end_tests,pred);
            return pred.value();
        }
    private:
        EndTests m_end_tests;
    };


};

#endif // COMPOSITE_END_TEST
