#ifndef MAX_ITERATION_END_TEST
#define MAX_ITERATION_END_TEST

namespace simulated_annealing
{
    /**
     * \ingroup GroupEndTest
     *
     * This class is a model of the EndTest concept and stops
     * the simulated annealing process after `n` iterations.
     *
     */
    class max_iteration_end_test {
    public:
        max_iteration_end_test(int n) : m_iterations(n) {}
        template<typename Configuration, typename Sampler>
        inline bool operator()(const Configuration&, const Sampler&, double) {
            return ((--m_iterations)<=0);
        }
        void stop () { m_iterations=0; }
    private:
        int m_iterations;
    };

};

#endif
