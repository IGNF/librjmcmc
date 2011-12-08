//[ objects
#include "geometry/geometry.hpp"
#include "geometry/Circle_2.h"
#include "geometry/Circle_2_intersection.h"
#include "geometry/Circle_2_coordinates.hpp"
typedef geometry::Simple_cartesian<double> K;
typedef K::Point_2 Point_2;
typedef geometry::Circle_2<K> Circle_2;
typedef Circle_2 object;
//]

//[ objective_function
#include "rjmcmc/energy/constant_energy.hpp"
#include "rjmcmc/energy/energy_operators.hpp"
#include "mpp/energy/intersection_area_binary_energy.hpp"
#include "mpp/configuration/graph_configuration.hpp"
typedef constant_energy<>           unary_energy;
typedef intersection_area_binary_energy<> binary_energy;
typedef marked_point_process::graph_configuration<object, unary_energy, multiplies_energy<constant_energy<>,binary_energy> > configuration;
//]

//[ reference_process
#include "rjmcmc/distribution/poisson_distribution.hpp"
typedef rjmcmc::poisson_distribution                   distribution;
#include "mpp/kernel/kernel.hpp"
typedef marked_point_process::uniform_birth<object> uniform_birth;
#include "mpp/direct_sampler.hpp"
typedef marked_point_process::direct_sampler<distribution,uniform_birth> reference_process;
//]

//[ rjmcmc_sampler
#include "rjmcmc/acceptance/metropolis_acceptance.hpp"
#include "rjmcmc/sampler/sampler.hpp"
typedef rjmcmc::metropolis_acceptance                                                  acceptance;
typedef marked_point_process::result_of_make_uniform_birth_death_kernel<object>::type  birth_death_kernel;
typedef rjmcmc::sampler<reference_process,acceptance,birth_death_kernel>                       sampler;
//]

//[ simulated_annealing
#include "simulated_annealing/schedule/geometric_schedule.hpp"
#include "simulated_annealing/end_test/max_iteration_end_test.hpp"
#include "simulated_annealing/simulated_annealing.hpp"
#include "simulated_annealing/visitor/ostream_visitor.hpp"
#ifdef USE_SHP
# include "simulated_annealing/visitor/composite_visitor.hpp"
# include "simulated_annealing/visitor/shp/shp_visitor.hpp"
#endif
//]

//[ parse_parameters
int main(int argc , char** argv)
{
    int i=0;
    double energy   = (++i<argc) ? atof(argv[i]) : -1.;
    double surface  = (++i<argc) ? atof(argv[i]) : 10.;
    double minradius= (++i<argc) ? atof(argv[i]) : 0.1;
    double maxradius= (++i<argc) ? atof(argv[i]) : 1;
    double poisson  = (++i<argc) ? atof(argv[i]) : 200.;
    double pbirth   = (++i<argc) ? atof(argv[i]) : 0.5;
    double pdeath   = (++i<argc) ? atof(argv[i]) : 0.5;
    int nbiter      = (++i<argc) ? atoi(argv[i]) : 15000000;
    double temp     = (++i<argc) ? atof(argv[i]) : 150.;
    double deccoef  = (++i<argc) ? atof(argv[i]) : 0.999999;
    int nbdump      = (++i<argc) ? atoi(argv[i]) : 10000;
    int nbsave      = (++i<argc) ? atoi(argv[i]) : 10000;
    //]

    //[optimize
    // Reference process
    distribution dpoisson(poisson);
    uniform_birth birth( Circle_2(Point_2(0,0),minradius), Circle_2(Point_2(1,1),maxradius) );
    reference_process reference_pdf( dpoisson, birth );

    // Initial empty configuration linked with the minimized energy
    unary_energy  e1(energy );
    multiplies_energy<constant_energy<>,binary_energy> e2(surface,binary_energy());
    configuration c(e1,e2);

    // Optimization
    sampler samp( reference_pdf, acceptance(),
                  marked_point_process::make_uniform_birth_death_kernel(birth, pbirth, pdeath)
                  );

    simulated_annealing::geometric_schedule<double> sch(temp,deccoef);
    simulated_annealing::max_iteration_end_test     end(nbiter);

#ifdef USE_SHP
    simulated_annealing::ostream_visitor osvisitor;
    simulated_annealing::shp::shp_visitor shpvisitor("quickstart_");
    simulated_annealing::composite_visitor< simulated_annealing::ostream_visitor,simulated_annealing::shp::shp_visitor> visitor(osvisitor,shpvisitor);
#else
    simulated_annealing::ostream_visitor visitor;
#endif

    visitor.init(nbdump,nbsave);
    simulated_annealing::optimize(c,samp,sch,end,visitor);

    return 0;
}
//]
