#ifndef RANDOM_HPP_
#define RANDOM_HPP_

#include <boost/random.hpp>

#ifndef WIN32
	#include <sys/time.h>
	#include <unistd.h>
#endif

//typedef boost::lagged_fibonacci607 RJMCMCRandom;
typedef boost::mt19937 RJMCMCRandom;

static RJMCMCRandom &GetRandom()
{
	static RJMCMCRandom randomness;
#ifndef WIN32
	static bool initialyze = true;
	if (initialyze)
	{
		struct timeval tv;
		gettimeofday(&tv,0);
		randomness.seed( tv.tv_sec + tv.tv_usec );
		initialyze = false;
	}
#endif
	return randomness;
}

#endif /* RANDOM_HPP_ */
