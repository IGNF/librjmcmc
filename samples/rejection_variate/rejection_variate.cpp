#include "rjmcmc/rjmcmc/random.hpp"
#include "rjmcmc/rjmcmc/kernel/variate.hpp"
#include "rjmcmc/rjmcmc/kernel/rejection_variate.hpp"


// simple predicate : is a 2D point within in the x+y<1 halfspace
struct Test
{
    template<typename InputIterator>
    bool operator()(InputIterator it) const
    {
        double x = *it++;
        double y = *it++;
        return (x+y)<1.;
    }
};

typedef rjmcmc::mt19937_generator Engine;  // source of randomness
typedef rjmcmc::variate<2>        Variate; // random 2D point variate in the unit square

template<typename Normalizer> void test(int iter, const Normalizer& normalizer)
{
    // rejection variate creation with the desired normalizer (=predicate success probability estimator)
    Variate var;
    Test test;
    rjmcmc::rejection_variate<Variate,Test,Normalizer> rej(var,test,normalizer);

    Engine& engine = rjmcmc::random();
    double val[2];
    // repeated sampling
    for(int i=0; i<iter; ++i)
    {
        double pdf = rej(engine,val);
        if(pdf==0) continue; // sampling failed
        if(pdf!=rej.pdf(val)) std::cerr << "pdf mismatch : " << pdf << "=" << rej.pdf(val) << std::endl;
        if(i % (iter/20) == 0)
            std::cout << pdf << ", ";
    }

    std::cout << "-> " << 1./rej.normalizer().inv_probability() << std::endl;

}

int main(int argc, char **argv)
{
    int iter = 10000;
    if(argc>1) iter = atoi(argv[1]);

    using namespace rjmcmc::rejection_variate_normalizer;
    test(iter,constant_normalizer(0.5));
    test(iter,monte_carlo());
    test(iter,decay_monte_carlo(0.999));
    test(iter,sliding_monte_carlo<1000>());

    return 0;
}
