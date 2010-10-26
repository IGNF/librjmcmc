//[ objects
#include "geometry/geometry.hpp"
#include "geometry/Circle_2.h"
#include "geometry/Circle_2_intersection.h"
typedef geometry::Simple_cartesian<double> K;
typedef geometry::Iso_rectangle_2_traits<K>::type Iso_rectangle_2;
typedef geometry::Circle_2<K> object;
//]

//[ objective_function
#include "mpp/energy/constant_unary_energy.hpp"
#include "mpp/energy/intersection_area_binary_energy.hpp"
#include "mpp/configuration/graph_configuration.hpp"
typedef constant_unary_energy           unary_energy;
typedef intersection_area_binary_energy binary_energy;
typedef marked_point_process::graph_configuration<object, unary_energy, binary_energy> configuration;
//]

//[ reference_process
#include "mpp/energy/box_is_valid.hpp"
#include "rjmcmc/kernel/kernels.hpp"
typedef rjmcmc::generator<box_is_valid> generator_kernel;
#include "mpp/density/poisson_density.hpp"
typedef marked_point_process::poisson_density                   density;
#include "mpp/direct_sampler.hpp"
typedef marked_point_process::direct_sampler<density,generator_kernel> d_sampler;
//]

//[ rjmcmc_sampler
#include "rjmcmc/sampler/metropolis_sampler.hpp"
typedef rjmcmc::uniform_birth_kernel<generator_kernel>          birth_kernel;
typedef rjmcmc::uniform_death_kernel                            death_kernel;
typedef rjmcmc::binary_kernel<birth_kernel,death_kernel>        birth_death_kernel;
typedef rjmcmc::metropolis_sampler<d_sampler,birth_death_kernel> sampler;
//]

//[ simulated_annealing
#include "simulated_annealing/schedule/geometric_schedule.hpp"
#include "simulated_annealing/end_test/max_iteration_end_test.hpp"
#include "simulated_annealing/simulated_annealing.hpp"
#include "simulated_annealing/visitor/ostream_visitor.hpp"
//]

//[ parse_parameters
int main(int argc , char** argv)
{
    int i=0;
    double energy   = (++i<argc) ? atof(argv[i]) : -1.;
    double surface  = (++i<argc) ? atof(argv[i]) : 10.;
    double minsize  = (++i<argc) ? atof(argv[i]) : 0.8;
    double maxratio = (++i<argc) ? atof(argv[i]) : 0.1;
    double poisson  = (++i<argc) ? atof(argv[i]) : 200.;
    double pbirth   = (++i<argc) ? atof(argv[i]) : 0.5;
    double pdeath   = (++i<argc) ? atof(argv[i]) : 0.5;
    int nbiter      = (++i<argc) ? atoi(argv[i]) : 15000000;
    double temp     = (++i<argc) ? atof(argv[i]) : 150.;
    double deccoef  = (++i<argc) ? atof(argv[i]) : 0.999999;
    int nbdump      = (++i<argc) ? atoi(argv[i]) : 10000;
    int nbsave      = (++i<argc) ? atoi(argv[i]) : 10000;
//]

//[ optimize
    unary_energy  e1(energy );
    binary_energy e2(surface);
    configuration c(e1,e2);

    Iso_rectangle_2 bbox(0.,0.,1.,1.);
    box_is_valid valid(bbox, minsize, maxratio );
    generator_kernel    birth(valid);
    density cs(poisson);
    d_sampler ds( cs, birth );

    birth_death_kernel kbirthdeath = rjmcmc::make_uniform_birth_death_kernel(birth, pbirth, pdeath);
    sampler samp( ds, kbirthdeath );

    simulated_annealing::geometric_schedule<double> sch(temp,deccoef);
    simulated_annealing::max_iteration_end_test     end(nbiter);
    simulated_annealing::ostream_visitor visitor;
    visitor.init(nbdump,nbsave);
    simulated_annealing::optimize(c,samp,sch,end,visitor);

    return 0;
}
//]
