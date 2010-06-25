#ifndef DUECK_SCHEUER_SAMPLER_HPP
#define DUECK_SCHEUER_SAMPLER_HPP

#include "sampler_base.hpp"

namespace rjmcmc
{

template<typename CountSampler, RJMCMC_TUPLE_TYPENAMES>
  class dueck_scheuer_sampler : public sampler_base<dueck_scheuer_sampler<CountSampler, RJMCMC_TUPLE_TYPES>, CountSampler, RJMCMC_TUPLE_TYPES>
{
private:
  typedef dueck_scheuer_sampler<CountSampler, RJMCMC_TUPLE_TYPES> self;
  typedef sampler_base<self, CountSampler, RJMCMC_TUPLE_TYPES> base;
  
public:
  dueck_scheuer_sampler(const CountSampler& cs, RJMCMC_TUPLE_ARGS) : base(cs,RJMCMC_TUPLE_PARAMS) {}
  
  inline double acceptance_probability() const {
    // return (metropolis_acceptance_probability()>=exp(-1)) ? 1 : 0;
    // return (R.exp(-E/T)>=exp(-1)) ? 1 : 0;
    // return (T.log(R)-E>=-T) ? 1 : 0;
    return (base::m_delta <= base::m_temperature*(1+log(base::m_green_ratio))) ? 1. : 0.;
  }
};

} // namespace rjmcmc

#endif // DUECK_SCHEUER_SAMPLER_HPP
