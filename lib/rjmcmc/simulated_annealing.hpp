#ifndef SIMULATED_ANNEALING_HPP
#define SIMULATED_ANNEALING_HPP

template<
	typename Configuration, typename Sampler,
	typename Schedule, typename EndTest,
	typename Visitor
>
void simulated_annealing(
	Configuration& config, Sampler& sampler,
	Schedule& schedule, EndTest& end_test,
	Visitor& visitor )
{
	visitor.begin(config,sampler,*schedule);
	for (unsigned int i=1; end_test(config,sampler,*schedule,i); ++i, ++schedule)
	{
		sampler(config,*schedule);
                bool result = visitor.iterate(config,sampler,*schedule,i);
                if(!result) break;
	}
	visitor.end(config,sampler,*schedule);
}

#endif
