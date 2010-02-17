#include "cli/console_parameter_traits.hpp"
#include "core/building_footprint_extraction_parameters_inc.hpp"
template class parameters<console_parameter_traits>;

#include "cli/image_types.hpp"
#include "core/image_gradient_unary_energy_inc.hpp"
template void image_gradient_unary_energy::gradient<rjmcmc::any_const_view_t>(const rjmcmc::any_const_view_t&, const Iso_Rectangle_2&, double, unsigned int);

#include "core/global_reconstruction_unary_energy_inc.hpp"
template void global_reconstruction_unary_energy::gradient<rjmcmc::any_const_view_t>(const rjmcmc::any_const_view_t&, const Iso_Rectangle_2&, double, unsigned int);

#include "core/gradient_image_inc.hpp"
template void rjmcmc::gradient_image::load<rjmcmc::any_const_view_t>(const rjmcmc::any_const_view_t&, const Iso_Rectangle_2&, double, unsigned int);

#include "core/image_inc.hpp"
template void rjmcmc::image::load<rjmcmc::any_const_view_t>(const rjmcmc::any_const_view_t&, const Iso_Rectangle_2&, unsigned int);

#include "core/building_footprint_extraction.hpp"
#include "rjmcmc/ostream_visitor.hpp"
#include <boost/gil/extension/io/tiff_dynamic_io.hpp>



int main(int argc , char** argv)
{
	param *p = param::Instance();
	if (!p->parse(argc, argv)) return -1;
	rjmcmc::ostream_visitor<sampler> visitor(std::cout);

	rjmcmc::any_image_t img;
        std::string dem(p->get<boost::filesystem::path>("dem").string());
	tiff_read_image(dem, img);

	building_footprint_extraction(visitor,const_view(img));
	return 0;
}

