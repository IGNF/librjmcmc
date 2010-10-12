#ifndef __BUILDING_FOOTPRINT_RECTANGLE_HPP__
#define __BUILDING_FOOTPRINT_RECTANGLE_HPP__

/************** application-specific types ****************/

//[building_footprint_rectangle_definition_geometry
#include "core/geometry.h"
typedef Rectangle_2 object;
//]
//#include <boost/variant.hpp>
//typedef boost::variant<Rectangle_2> object;

//[building_footprint_rectangle_definition_energies
#include "mpp/energy/image_gradient_unary_energy.hpp"
#include "image/gradient_functor.hpp"
#include "image/oriented.hpp"
typedef oriented<gradient_image_t>                          oriented_gradient_view;
typedef image_gradient_unary_energy<oriented_gradient_view> unary_energy;

#include "mpp/energy/intersection_area_binary_energy.hpp"
typedef intersection_area_binary_energy                     binary_energy;

#include "mpp/energy/box_is_valid.hpp"
typedef box_is_valid is_valid;
//]

//[building_footprint_rectangle_definition_kernels
#include "rjmcmc/kernel/kernels.hpp"
typedef generator<is_valid> generator_kernel;
typedef modifier <is_valid> modifier_kernel;
//]

/************** rjmcmc library types ****************/

//[building_footprint_rectangle_definition_simulated_annealing
/*< Choice of the schedule (/include/ and /typedef/) >*/
#include "simulated_annealing/schedule/geometric_schedule.hpp"
typedef simulated_annealing::geometric_schedule<double> schedule;
/*< Choice of the end_test (/include/ and /typedef/) >*/
#include "simulated_annealing/end_test/max_iteration_end_test.hpp"
typedef simulated_annealing::max_iteration_end_test     end_test;
//]

//#include "rjmcmc/step_schedule.hpp"
//typedef simulated_annealing::step_schedule<rjmcmc::logarithmic_schedule<double> > schedule;

//#include "simulated_annealing/schedule/inverse_linear_schedule.hpp"
//typedef simulated_annealing::inverse_linear_schedule<double>                 schedule;

//#include "simulated_annealing/end_test/delta_energy_end_test.hpp"
//typedef simulated_annealing::delta_energy_end_test                           end_test;

//#include "mpp/configuration/vector_configuration.hpp"
//typedef marked_point_process::vector_configuration<object, unary_energy, binary_energy> configuration;
//[building_footprint_rectangle_definition_configuration
#include "mpp/configuration/graph_configuration.hpp"
typedef marked_point_process::graph_configuration<object, unary_energy, binary_energy> configuration;
//]

//[building_footprint_rectangle_definition_density
#include "mpp/density/poisson_density.hpp"
typedef marked_point_process::poisson_density                           density;
//]
//#include "mpp/density/uniform_density.hpp"
//typedef marked_point_process::uniform_density                           density;

//[building_footprint_rectangle_definition_sampler
#include "rjmcmc/sampler/sampler_base.hpp"
typedef rjmcmc::uniform_birth_kernel<generator_kernel>          birth_kernel;
typedef rjmcmc::uniform_death_kernel                            death_kernel;
typedef rjmcmc::binary_kernel<birth_kernel,death_kernel>        birth_death_kernel;
typedef rjmcmc::modification_kernel<modifier_kernel>            modification_kernel;
#include "mpp/direct_sampler.hpp"
typedef marked_point_process::direct_sampler<density,generator_kernel> d_sampler;
#include "rjmcmc/sampler/metropolis_sampler.hpp"
typedef rjmcmc::metropolis_sampler<d_sampler,birth_death_kernel,modification_kernel> sampler;
//]
//#include "rjmcmc/sampler/dueck_scheuer_sampler.hpp"
//typedef rjmcmc::dueck_scheuer_sampler<density,birth_death_kernel,modification_kernel> sampler;

//#include "rjmcmc/sampler/franz_hoffmann_sampler.hpp"
//typedef rjmcmc::franz_hoffmann_sampler<density,birth_death_kernel,modification_kernel> sampler;

//#include "rjmcmc/sampler/rejection_sampler.hpp"
//typedef rejection_sampler<d_sampler,null_binary_energy_predicate> sampler;

/************** main ****************/

//[building_footprint_rectangle_bbox_accessors
Iso_rectangle_2 get_bbox(const param *p) {
  int x0 = p->get<int>("xmin");
  int x1 = p->get<int>("xmax");
  int y0 = p->get<int>("ymin");
  int y1 = p->get<int>("ymax");
  if(x0>x1) std::swap(x0,x1);
  if(y0>y1) std::swap(y0,y1);
  return Iso_rectangle_2(x0,y0,x1,y1);
}

void set_bbox(param *p, const Iso_rectangle_2& r) {
  p->set("xmin",(int) r.min().x());
  p->set("ymin",(int) r.min().y());
  p->set("xmax",(int) r.max().x());
  p->set("ymax",(int) r.max().y());
}
//]

//[building_footprint_rectangle_create_configuration
void create_configuration(const param *p, const oriented_gradient_view& grad, configuration *&c) {
	// energies
  unary_energy e1( grad,
                   p->get<double>("energy"),
                   p->get<double>("ponderation_grad")
                 );
  
  binary_energy e2(
                    p->get<double>("surface")
                  );
  
	// empty initial configuration
  c = new configuration(e1,e2);
}
//]

//[building_footprint_rectangle_create_sampler
void create_sampler(const param *p, sampler *&s) {
  is_valid valid(
                  get_bbox(p),
                  p->get<double>("minsize"),
                  p->get<double>("maxratio")
                );
  generator_kernel    birth(valid);
  density cs(p->get<double>("poisson"));
  
  birth_death_kernel kbirthdeath = rjmcmc::make_uniform_birth_death_kernel(
		birth,
        p->get<double>("pbirth"),
        p->get<double>("pdeath")
	);

  modifier_kernel     modif(valid);
  modification_kernel kmodif(modif);
  
  d_sampler ds( cs, birth );
  s = new sampler( ds, kbirthdeath, kmodif );
  //s = new sampler( p->get<double>("qtemp"), cs, kbirthdeath, kmodif );
  //s = new sampler( ds );
}
//]

//[building_footprint_rectangle_create_schedule
void create_schedule(const param *p, schedule *&t)
{
	t = new schedule(
		p->get<double>("temp"),
		p->get<double>("deccoef")
	);
  //  t = new schedule( 1000, rjmcmc::logarithmic_schedule(p->get<double>("temp")) );
}
//]

//[building_footprint_rectangle_create_end_test
void create_end_test(const param *p, end_test *&e)
{
  e = new end_test(
                    p->get<int>("nbiter")
                  );
}
//]

//[building_footprint_rectangle_init_visitor
template<typename Visitor>
void init_visitor(const param *p, Visitor& v)
{
  v.init(
          p->get<int>("nbdump"),
          p->get<int>("nbsave")
        );
}
//]

/*
// Salamon
#include "rjmcmc/sampler/direct_poisson_sampler.hpp"
double estimate_initial_temperature(param *p, unsigned int n, configuration& c)
{
  is_valid valid(
                  get_bbox(p),
                  p->get<double>("minsize"),
                  p->get<double>("maxratio")
                );
  generator_kernel generator(valid);
  direct_poisson_sampler<generator_kernel> dps(p->get<double>("poisson"),generator);
  double e1 = 0;
  double e2 = 0;
  double invn = 1./n;
  for(unsigned int i=0; i<n; ++i) {
    dps(c);
    double e = c.energy();
    double invne = invn*e;
    e1 += invne;
    e2 += invne*e;
  }
  double std_dev = sqrt(e2-e1*e1);
  p->set("temp",2*std_dev);
}
*/

#endif // __BUILDING_FOOTPRINT_RECTANGLE_HPP__
