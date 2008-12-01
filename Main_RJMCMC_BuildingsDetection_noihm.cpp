#include <iostream>
#include <string>

class RJMCMC_BuildingsDetectionThread
{
public :
	void *Entry();
};

#define USE_IHM 0
#include "RJMCMC_BuildingsDetectionThread.hpp"


int main (int argc, char **argv)
{
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, BuildingsDetectorParametersSingleton::Instance()->GetAsProgramOption()), vm);
	if (vm.count("config"))
	{
		std::ifstream config_file(vm["config"].as<std::string>().c_str());
		po::store(po::parse_config_file(config_file, BuildingsDetectorParametersSingleton::Instance()->GetAsProgramOption()), vm);
	}
	po::notify(vm);
	
	if (vm.count("help")) 
	{
		std::cout << BuildingsDetectorParametersSingleton::Instance()->GetAsProgramOption() << "\n";
		return 1;
	}


	
	RJMCMC_BuildingsDetectionThread rien;
	rien.Entry();
	return 0;
}
