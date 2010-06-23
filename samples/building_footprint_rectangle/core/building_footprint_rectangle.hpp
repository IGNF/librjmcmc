#ifndef __BUILDING_FOOTPRINT_RECTANGLE_HPP__
#define __BUILDING_FOOTPRINT_RECTANGLE_HPP__

/************** application-specific types ****************/

#include "core/geometry.h"
typedef Rectangle_2 object;
//#include <boost/variant.hpp>
//typedef boost::variant<Rectangle_2> object;

#include "energy/box_is_valid.hpp"
typedef box_is_valid                         is_valid;

#include "energy/image_gradient_unary_energy.hpp"
#include "image/gradient_functor.hpp"
#include "image/oriented.hpp"
typedef oriented<gradient_image_t> oriented_gradient_view;
typedef image_gradient_unary_energy<oriented_gradient_view> unary_energy;

#include "energy/intersection_area_binary_energy.hpp"
typedef intersection_area_binary_energy      binary_energy;

#include "energy/kernels.hpp"
typedef generator<is_valid>          generator_kernel;
typedef modifier <is_valid>          modifier_kernel;

/************** rjmcmc library types ****************/

//#include "rjmcmc/logarithmic_schedule.hpp"
//#include "rjmcmc/step_schedule.hpp"
//typedef rjmcmc::step_schedule<rjmcmc::logarithmic_schedule>                  schedule;
#include "rjmcmc/schedule/geometric_schedule.hpp"
typedef rjmcmc::geometric_schedule                  schedule;
//#include "rjmcmc/schedule/inverse_linear_schedule.hpp"
//typedef rjmcmc::inverse_linear_schedule                  schedule;


#include "rjmcmc/end_test/delta_energy_end_test.hpp"
typedef delta_energy_end_test                           end_test;
//#include "rjmcmc/end_test/max_iteration_end_test.hpp"
//typedef max_iteration_end_test                           end_test;

//#include "rjmcmc/configuration/vector_configuration.hpp"
//typedef rjmcmc::vector_configuration<object, unary_energy, binary_energy> configuration;
#include "rjmcmc/configuration/graph_configuration.hpp"
typedef rjmcmc::graph_configuration<object, unary_energy, binary_energy> configuration;

//#include "rjmcmc/sampler/direct_poisson_sampler.hpp"
//typedef direct_poisson_sampler<generator_kernel> sampler;
//#include "rjmcmc/sampler/metropolis_sampler.hpp"
//#include "rjmcmc/sampler/dueck_scheuer_sampler.hpp"
#include "rjmcmc/sampler/franz_hoffmann_sampler.hpp"
typedef rjmcmc::uniform_birth_kernel<generator_kernel>          birth_kernel;
typedef rjmcmc::uniform_death_kernel                            death_kernel;
typedef rjmcmc::binary_kernel<birth_kernel,death_kernel>        birth_death_kernel;
typedef rjmcmc::modification_kernel<modifier_kernel>            modification_kernel;
//typedef rjmcmc::metropolis_sampler<birth_death_kernel,modification_kernel> sampler;
//typedef rjmcmc::dueck_scheuer_sampler<birth_death_kernel,modification_kernel> sampler;
typedef rjmcmc::franz_hoffmann_sampler<birth_death_kernel,modification_kernel> sampler;

/************** main ****************/

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

void create_sampler(const param *p, sampler *&s) {
  is_valid valid(
                  get_bbox(p),
                  p->get<double>("minsize"),
                  p->get<double>("maxratio")
                );
  
  generator_kernel birth(valid);
  modifier_kernel  modif(valid);
  
  birth_kernel        kbirth(birth);
  death_kernel        kdeath(      p->get<double>("poisson"));
  modification_kernel kmodif(modif,p->get<double>("poisson"));
  
  birth_death_kernel kbirthdeath(
                                  kbirth, kdeath,
                                  p->get<double>("pbirth"),
                                  p->get<double>("pdeath")
                                );
  
  //s = new sampler( kbirthdeath, kmodif );
  s = new sampler( p->get<double>("qtemp"), kbirthdeath, kmodif );
//	s = new sampler( p->get<double>("poisson"), birth );
}

void create_schedule(const param *p, schedule *&t)
{
	t = new schedule(
		p->get<double>("temp"),
		p->get<double>("deccoef")
	);
  //  t = new schedule( 1000, rjmcmc::logarithmic_schedule(p->get<double>("temp")) );
}

void create_end_test(const param *p, end_test *&e)
{
  e = new end_test(
                    p->get<int>("nbiter")
                  );
}

template<typename Visitor>
void init_visitor(const param *p, Visitor& v)
{
  v.init(
          p->get<int>("nbdump"),
          p->get<int>("nbsave")
        );
}

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
