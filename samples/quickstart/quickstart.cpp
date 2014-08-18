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

#include "rjmcmc/rjmcmc/random.hpp"

//[ objects
#include "rjmcmc/geometry/geometry.hpp"
#include "rjmcmc/geometry/Circle_2.hpp"
#include "rjmcmc/geometry/intersection/Circle_2_intersection.hpp"
#include "rjmcmc/geometry/coordinates/Circle_2_coordinates.hpp"
typedef geometry::Simple_cartesian<double> K;
typedef K::Point_2 Point_2;
typedef geometry::Circle_2<K> Circle_2;
typedef Circle_2 object;
//]

//[ objective_function
#include "rjmcmc/rjmcmc/energy/constant_energy.hpp"
#include "rjmcmc/rjmcmc/energy/energy_operators.hpp"
#include "rjmcmc/mpp/energy/intersection_area_binary_energy.hpp"
typedef constant_energy<>           unary_energy;
typedef intersection_area_binary_energy<> binary_energy;

//#include "rjmcmc/mpp/configuration/fusion_vector_configuration.hpp"
#include "rjmcmc/mpp/configuration/vector_configuration.hpp"
typedef marked_point_process::vector_configuration<object, unary_energy, multiplies_energy<constant_energy<>,binary_energy> > configuration;

//#include "rjmcmc/mpp/configuration/graph_configuration.hpp"
//typedef marked_point_process::graph_configuration<object, unary_energy, multiplies_energy<constant_energy<>,binary_energy> > configuration;

//]

//[ reference_process
#include "rjmcmc/rjmcmc/distribution/poisson_distribution.hpp"
typedef rjmcmc::poisson_distribution                   distribution;
#include "rjmcmc/mpp/kernel/uniform_birth.hpp"
typedef marked_point_process::uniform_birth<object> uniform_birth;
#include "rjmcmc/mpp/direct_sampler.hpp"
typedef marked_point_process::direct_sampler<distribution,uniform_birth> reference_process;
//]

//[ rjmcmc_sampler
#include "rjmcmc/rjmcmc/acceptance/metropolis_acceptance.hpp"
#include "rjmcmc/rjmcmc/sampler/sampler.hpp"
#include "rjmcmc/mpp/kernel/uniform_birth_death_kernel.hpp"
typedef rjmcmc::metropolis_acceptance                                                  acceptance;
typedef marked_point_process::uniform_birth_death_kernel<uniform_birth>::type  birth_death_kernel;
typedef rjmcmc::sampler<reference_process,acceptance,birth_death_kernel>                       sampler;
//]

//[ simulated_annealing
#include "rjmcmc/simulated_annealing/schedule/geometric_schedule.hpp"
#include "rjmcmc/simulated_annealing/end_test/max_iteration_end_test.hpp"
#include "rjmcmc/simulated_annealing/simulated_annealing.hpp"
#include "rjmcmc/simulated_annealing/visitor/ostream_visitor.hpp"
#include "rjmcmc/simulated_annealing/visitor/tex_visitor.hpp"
#include "rjmcmc/simulated_annealing/visitor/composite_visitor.hpp"
#ifdef USE_SHP
# include "rjmcmc/simulated_annealing/visitor/shp_visitor.hpp"
#endif
//]

//[ parse_parameters
int main(int argc , char** argv)
{
    int i=0;
    double energy   = (++i<argc) ? atof(argv[i]) : -1.;
    double surface  = (++i<argc) ? atof(argv[i]) : 10000.;
    double minradius= (++i<argc) ? atof(argv[i]) : 0.02;
    double maxradius= (++i<argc) ? atof(argv[i]) : 0.1;
    double poisson  = (++i<argc) ? atof(argv[i]) : 200.;
    double pbirth   = (++i<argc) ? atof(argv[i]) : 0.5;
    double pdeath   = (++i<argc) ? atof(argv[i]) : 0.5;
    int nbiter      = (++i<argc) ? atoi(argv[i]) : 10000001;
    double temp     = (++i<argc) ? atof(argv[i]) : 200.;
    double deccoef  = (++i<argc) ? atof(argv[i]) : 0.999999;
    int nbdump      = (++i<argc) ? atoi(argv[i]) : 1000000;
    int nbsave      = (++i<argc) ? atoi(argv[i]) : 1000000;
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
    simulated_annealing::ostream_visitor osvisitor;
    simulated_annealing::tex_visitor texvisitor("quickstart_");
    simulated_annealing::composite_visitor< simulated_annealing::ostream_visitor,simulated_annealing::tex_visitor> visitor(osvisitor,texvisitor);
#endif

    typedef rjmcmc::mt19937_generator Engine;
    Engine& e = rjmcmc::random();

    visitor.init(nbdump,nbsave);
    simulated_annealing::optimize(e,c,samp,sch,end,visitor);

    return 0;
}
//]
