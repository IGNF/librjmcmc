#ifndef SIMULATED_ANNEALING_HPP
#define SIMULATED_ANNEALING_HPP
#include "boost/concept/assert.hpp"

namespace simulated_annealing
{

    template<
            class Configuration, class Sampler,
            class Schedule, class EndTest,
            class Visitor
            >
            void optimize(
                    Configuration& config, Sampler& sampler,
                    Schedule& schedule, EndTest& end_test,
                    Visitor& visitor )
    {
        BOOST_CONCEPT_ASSERT((boost::InputIterator<Schedule>));

	visitor.begin(config,sampler,*schedule);
	for (unsigned int i=1; end_test(config,sampler,*schedule,i); ++i, ++schedule)
	{
            sampler(config,*schedule);
            bool result = visitor.iterate(config,sampler,*schedule,i);
            if(!result) break;
	}
	visitor.end(config,sampler,*schedule);
    }

}

#endif
