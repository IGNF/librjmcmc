#ifndef RANDOM_HPP_
#define RANDOM_HPP_

#include <boost/random.hpp>

#ifndef _WINDOWS
#	include <sys/time.h>
#	include <unistd.h>
#endif

typedef boost::mt19937 RJMCMCRandom;

/**
 * @fn static RJMCMCRandom &GetRandom() Simple function which return an initialised random number generator
 * @todo Properly handle Windows platform
 */
static RJMCMCRandom &GetRandom()
{
	static RJMCMCRandom randomness;
#ifndef WIN32
	static bool is_already_initialised = true;
	if (is_already_initialised)
	{
		struct timeval tv;
		gettimeofday(&tv, 0);
		randomness.seed(tv.tv_sec + tv.tv_usec);
		is_already_initialised = false;
	}
#endif
	return randomness;
}

#endif /* RANDOM_HPP_ */
