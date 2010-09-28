#ifndef RANDOM_HPP_
#define RANDOM_HPP_

#include <boost/random.hpp>

#ifndef _WINDOWS
#	include <sys/time.h>
#	include <unistd.h>
#endif

namespace rjmcmc {

    typedef boost::mt19937 generator;

    /**
 * @fn static RJMCMCRandom &GetRandom() Simple function which return an initialised random number generator
 * @todo Properly handle Windows platform
 */
    static generator &random()
    {
	static generator g;
#ifndef WIN32
	static bool uninitialised = true;
	if (uninitialised)
	{
            struct timeval tv;
            gettimeofday(&tv, 0);
            g.seed(tv.tv_sec + tv.tv_usec);
            uninitialised = false;
	}
#endif
	return g;
    }

}; // namespace rjmcmc

#endif /* RANDOM_HPP_ */
