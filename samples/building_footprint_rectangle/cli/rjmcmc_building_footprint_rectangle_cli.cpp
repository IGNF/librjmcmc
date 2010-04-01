#include "param/console_parameter_traits.hpp"
#include "core/building_footprint_rectangle_parameters_inc.hpp"

#include "visitor/ostream_visitor.hpp"

#include "core/building_footprint_rectangle.hpp"
#include "rjmcmc/simulated_annealing.hpp"

#include <boost/gil/extension/io/tiff_dynamic_io.hpp>
#include "image/image_types.hpp"

// template instanciations
#include "image/gradient_image_inc.hpp"
#include "energy/image_gradient_unary_energy_inc.hpp"

int main(int argc , char** argv)
{
	param *p = param::Instance();
        initialize_parameters(p);
	if (!p->parse(argc, argv)) return -1;

	rjmcmc::ostream_visitor<sampler> visitor(std::cout);
	init_visitor(p,visitor);

	rjmcmc::any_image_t img;
        std::string dem(p->get<boost::filesystem::path>("dem").string());
	tiff_read_image(dem, img);

	configuration *conf;
	sampler       *samp;
	temperature   *temp;
	end_test      *end ;

	create_configuration(p,const_view(img),conf);
	create_sampler      (p,samp);
	create_temperature  (p,temp);
	create_end_test     (p,end);
        
	simulated_annealing(*conf,*samp,*temp,*end,visitor);

	return 0;
}

