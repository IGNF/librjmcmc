#include "../rjmcmc_building_footprint_extraction_thread.hpp"
#include "core/building_footprint_extraction_parameters.hpp"

int main(int argc , char** argv)
{
	if (!building_footprint_extraction_parameters::Instance()->parse_command_line(argc, argv))
		return -1;

	rjmcmc_building_footprint_extraction_thread thread;
	thread.Entry();

	return 0;
}
