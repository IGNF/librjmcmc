#ifndef SIMULATED_ANNEALING_HPP
#define SIMULATED_ANNEALING_HPP

template<
	typename Configuration, typename Sampler,
	typename Temperature, typename EndTest,
	typename Visitor
>
void simulated_annealing(
	Configuration& config, Sampler& sampler,
	Temperature& temp, EndTest& end_test,
	Visitor& visitor )
{
	visitor.begin(config,sampler,*temp);
	for (unsigned int i=1; end_test(config,sampler,*temp,i); ++i, ++temp)
	{
		sampler(config,*temp);
		if(!visitor.iterate(config,sampler,*temp,i)) break;
	}
	visitor.end(config,sampler,*temp);
}

#endif
