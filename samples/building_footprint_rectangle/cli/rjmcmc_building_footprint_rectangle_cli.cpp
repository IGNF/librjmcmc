// parameters
#include "param/parameters.hpp"
typedef parameters< parameter > param;
#include "core/building_footprint_rectangle_parameters_inc.hpp"

// images
#include "image/image_types.hpp"

// optimization
#include "core/building_footprint_rectangle.hpp"
#include "simulated_annealing/simulated_annealing.hpp"

// visitors
#include "simulated_annealing/visitor/ostream_visitor.hpp"

// template instanciations
#include "image/oriented_inc.hpp"
#include "image/gradient_functor_inc.hpp"
#include "mpp/energy/image_gradient_unary_energy_inc.hpp"

int main(int argc , char** argv)
{
  param *p = param::instance();
  initialize_parameters(p);
  if (!p->parse(argc, argv)) return -1;
  
  simulated_annealing::ostream_visitor visitor;
  init_visitor(p,visitor);
  
  Iso_rectangle_2 bbox = get_bbox(p);
  std::string  dsm_file = p->get<boost::filesystem::path>("dsm" ).string();
  clip_bbox(bbox, dsm_file);
  
  gradient_functor gf(p->get<double>("sigmaD"));
  oriented_gradient_view grad_view(dsm_file, bbox, gf);
  
  configuration *conf;
  sampler       *samp;
  schedule      *sch;
  end_test      *end ;
  
  create_configuration(p,grad_view,conf);
//  estimate_initial_temperature(p,100,*conf);
  create_sampler      (p,samp);
  create_schedule     (p,sch);
  create_end_test     (p,end);
  
  simulated_annealing::optimize(*conf,*samp,*sch,*end,visitor);
  
  return 0;
}

