#include "param/console_parameter_traits.hpp"
#include "core/building_footprint_rectangle_parameters_inc.hpp"

#include "energy/image_gradient_unary_energy_inc.hpp"

#include "image/image_types.hpp"
#include "image/gradient_image_inc.hpp"
#include "image/image_inc.hpp"

#include "core/building_footprint_rectangle.hpp"
#include "visitor/ostream_visitor.hpp"
#include <boost/gil/extension/io/tiff_dynamic_io.hpp>



int main(int argc , char** argv)
{
	param *p = param::Instance();
        initialize_parameters(p);
	if (!p->parse(argc, argv)) return -1;
	rjmcmc::ostream_visitor<sampler> visitor(std::cout);

	rjmcmc::any_image_t img;
        std::string dem(p->get<boost::filesystem::path>("dem").string());
	tiff_read_image(dem, img);

        building_footprint_rectangle(visitor,const_view(img));
	return 0;
}

