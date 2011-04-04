#ifndef DELTA_ENERGY_END_TEST
#define DELTA_ENERGY_END_TEST

namespace simulated_annealing
{
    /**
     * \ingroup GroupEndTest
     *
     * This class is a model of the EndTest concept and stops
     * the simulated annealing process if the energy of the configuration
     * has not changed during the last `n` iterations.
     *
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

#endif // DELTA_ENERGY_END_TEST
