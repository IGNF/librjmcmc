#ifndef SALAMON_INITIAL_SCHEDULE_HPP
#define SALAMON_INITIAL_SCHEDULE_HPP

template<typename DirectSampler, typename Configuration>
double salamon_initial_schedule(const DirectSampler& sampler, Configuration& c, unsigned int iterations)
{
  double e1 = 0;
  double e2 = 0;
  double inv = 1./iterations;
  for(unsigned int i=0; i<iterations; ++i) {
    sampler(c);
    double e = c.energy();
    double inv_e = inv*e;
    e1 += inv_e;
    e2 += inv_e*e;
  }
  double std_dev = sqrt(e2-e1*e1);
  return 2*std_dev;
}

#endif // SALAMON_INITIAL_SCHEDULE_HPP
