#include "core/building_footprint_extraction.hpp"
#include "core/rjmcmc_ostream_visitor.hpp"

int main(int argc , char** argv)
{
	if (!building_footprint_extraction_parameters::Instance()->parse_command_line(argc, argv))
		return -1;

	sampler_ostream_visitor<building_sampler> visitor(std::cout);
	rjmcmc_building_footprint_extraction(visitor);

	return 0;
}
