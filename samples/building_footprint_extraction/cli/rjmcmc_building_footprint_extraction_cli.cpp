// parameters
#include "param/parameters.hpp"
typedef parameters< parameter > param;
#include "core/building_footprint_extraction_parameters_inc.hpp"

// images
#include "image/image_types.hpp"

// optimization
#include "core/building_footprint_extraction.hpp"
#include "rjmcmc/simulated_annealing.hpp"

// visitors
#include "visitor/ostream_visitor.hpp"

// template instanciations
#include "image/gradient_image_inc.hpp"
#include "image/image_inc.hpp"
#include "core/global_reconstruction_unary_energy_inc.hpp"

int main(int argc , char** argv)
{
    param *p = param::instance();
    initialize_parameters(p);
	if (!p->parse(argc, argv)) return -1;

	rjmcmc::ostream_visitor visitor;
	init_visitor(p,visitor);

	Iso_rectangle_2 bbox = get_bbox(p);
	int x0, y0;

	gradient_image_t grad;
	gradient_image(grad,x0,y0,p->get<boost::filesystem::path>("dsm").string(),bbox);
	oriented_dsm_view dsm_view(view(grad),x0,y0);

	ndvi_image_t ndvi;
	ndvi_image(ndvi,x0,y0,p->get<boost::filesystem::path>("ndvi").string(),bbox);
	oriented_ndvi_view ndvi_view(view(ndvi),x0,y0);

	configuration *conf;
	sampler       *samp;
	temperature   *temp;
	end_test      *end ;

	create_configuration(p,dsm_view,ndvi_view,conf);
	create_sampler      (p,samp);
	create_temperature  (p,temp);
	create_end_test     (p,end);
        
	simulated_annealing(*conf,*samp,*temp,*end,visitor);

	return 0;
}
