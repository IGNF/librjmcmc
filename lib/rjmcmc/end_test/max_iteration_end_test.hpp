#ifndef MAX_ITERATION_END_TEST
#define MAX_ITERATION_END_TEST

namespace simulated_annealing
{

    class max_iteration_end_test {
    public:
        max_iteration_end_test(int i) : m_iterations(i) {}
        template<typename Configuration, typename Sampler>
        inline bool operator()(const Configuration&, const Sampler&, double) {
            return ((--m_iterations)>0);
        }
        void stop () { m_iterations=0; }
    private:
        int m_iterations;
    };

};

#endif
