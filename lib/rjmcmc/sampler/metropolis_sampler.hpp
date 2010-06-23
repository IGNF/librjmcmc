#ifndef METROPOLIS_SAMPLER_HPP
#define METROPOLIS_SAMPLER_HPP

#include "sampler_base.hpp"

namespace rjmcmc
{

template<typename CountSampler, RJMCMC_SAMPLER_TYPENAMES>
  class metropolis_sampler
  : public sampler_base<metropolis_sampler<CountSampler,RJMCMC_SAMPLER_TYPES>, CountSampler, RJMCMC_SAMPLER_TYPES>
{
private:
  typedef metropolis_sampler<CountSampler, RJMCMC_SAMPLER_TYPES> self;
  typedef sampler_base<self, CountSampler, RJMCMC_SAMPLER_TYPES> base;
  
public:
  metropolis_sampler(const CountSampler& cs, RJMCMC_SAMPLER_ARGS) : base(cs,RJMCMC_SAMPLER_PARAMS) {}
  
  inline double acceptance_probability() const {
    return base::m_green_ratio*exp(-base::m_delta/base::m_temperature);
  }
};

} // namespace rjmcmc

#endif // METROPOLIS_SAMPLER_HPP
