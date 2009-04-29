#ifndef RANDOM_HPP_
#define RANDOM_HPP_

#include <ctime>

#include <boost/random.hpp>

typedef boost::mt19937 RJMCMCRandom;

static RJMCMCRandom &GetRandom()
{
	static RJMCMCRandom randomness;
	static bool initialyze = true;
	if (initialyze)
	{
		boost::uint32_t it = std::time(00);
		std::cout << "Random seed : " << it <<std::endl;
		randomness.seed(it);
		initialyze = false;
	}
	return randomness;
}

#endif /* RANDOM_HPP_ */
