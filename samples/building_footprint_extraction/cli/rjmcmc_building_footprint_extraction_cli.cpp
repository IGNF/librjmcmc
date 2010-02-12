#include "core/building_footprint_extraction_parameters_inc.hpp"
template class parameters<>;
typedef parameters<> param;

#include "core/building_footprint_extraction.hpp"
#include "rjmcmc/ostream_visitor.hpp"

int main(int argc , char** argv)
{
	if (!param::Instance()->parse(argc, argv)) return -1;
	rjmcmc::ostream_visitor<sampler> visitor(std::cout);
	rjmcmc_building_footprint_extraction(visitor);
	return 0;
}

