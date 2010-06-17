// parameters
#include "param/parameters.hpp"
typedef parameters< parameter > param;
#include "core/building_footprint_rectangle_parameters_inc.hpp"

// images
#include "image/image_types.hpp"

// optimization
#include "core/building_footprint_rectangle.hpp"
#include "rjmcmc/simulated_annealing.hpp"

// visitors
#include "visitor/ostream_visitor.hpp"

// template instanciations
#include "image/gradient_image_inc.hpp"
#include "energy/image_gradient_unary_energy_inc.hpp"


int main(int argc , char** argv)
{
  param *p = param::instance();
  initialize_parameters(p);
  if (!p->parse(argc, argv)) return -1;
  
  rjmcmc::ostream_visitor visitor;
  init_visitor(p,visitor);
  
  Iso_rectangle_2 bbox = get_bbox(p);
  std::string  dsm_file = p->get<boost::filesystem::path>("dsm" ).string();
  clip_bbox(bbox,dsm_file );
  
  oriented_gradient_view grad_view;
  gradient_image_t       grad_image;
  gradient_view(grad_view, grad_image, dsm_file, bbox, p->get<double>("sigmaD"));
  
  configuration *conf;
  sampler       *samp;
  temperature   *temp;
  end_test      *end ;
  
  create_configuration(p,grad_view,conf);
  create_sampler      (p,samp);
  create_temperature  (p,temp);
  create_end_test     (p,end);
  
  simulated_annealing(*conf,*samp,*temp,*end,visitor);
  
  return 0;
}

