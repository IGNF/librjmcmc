#ifndef SIMULATED_ANNEALING_HPP
#define SIMULATED_ANNEALING_HPP
#include "boost/concept/assert.hpp"

namespace simulated_annealing
{

    //[simulated_annealing_signature
    template<
            class Configuration, class Sampler,
            class Schedule, class EndTest,
            class Visitor
            >
            void optimize(
                    Configuration& config, Sampler& sampler,
                    Schedule& schedule, EndTest& end_test,
                    Visitor& visitor )
            //]
    {

        //[simulated_annealing_concept_assertions
        BOOST_CONCEPT_ASSERT((boost::InputIterator<Schedule>));
        //]

        //[simulated_annealing_loop
        double t = *schedule;
        visitor.begin(config,sampler,t);
        for(; !end_test(config,sampler,t); t = *(++schedule))
        {
            sampler(config,t);
            visitor.visit(config,sampler,t);
        }
        visitor.end(config,sampler,t);
        //]
    }

}

#endif
