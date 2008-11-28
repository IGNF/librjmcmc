
class RJMCMC_BuildingsDetectionThread
{
public :
	void *Entry();
};

#define USE_IHM 0
#include "RJMCMC_BuildingsDetectionThread.hpp"

int main (int argc, char **argv)
{
	RJMCMC_BuildingsDetectionThread rien;
	BuildingsDetectorParametersSingleton::Instance()->ParseCmdLine(argc, argv);
	rien.Entry();
	return 0;
}
