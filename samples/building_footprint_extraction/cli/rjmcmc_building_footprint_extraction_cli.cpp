#include "core/building_footprint_extraction.hpp"
#include "core/rjmcmc_ostream_visitor.hpp"

/*
void test() {
	Point_2 p(0,0);
	Vector_2 v(1,1);
	Rectangle_2 r(p,v,100);
	Circle_2 c(p,1);

	std::cout << intersection_area(r,c) << std::endl;
}
*/

int main(int argc , char** argv)
{
	if (!building_footprint_extraction_parameters::Instance()->parse_command_line(argc, argv))
		return -1;

	sampler_ostream_visitor<building_sampler> visitor(std::cout);
	rjmcmc_building_footprint_extraction(visitor);

	return 0;
}

