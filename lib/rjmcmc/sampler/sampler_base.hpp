#ifndef __RJMCMC_SAMPLER_BASE_HPP__
#define __RJMCMC_SAMPLER_BASE_HPP__

#include "rjmcmc/tuple.hpp"
#include "rjmcmc/random.hpp"
#include "rjmcmc/random_apply.hpp"
#include "rjmcmc/kernel/kernel_traits.hpp"
#include "rjmcmc/kernel/unary_kernel.hpp"
#include "rjmcmc/kernel/binary_kernel.hpp"

namespace rjmcmc {

// Derived: Curiously recurring template pattern
template<typename Derived, typename CountSampler, RJMCMC_TUPLE_TYPENAMES >
  class sampler_base
{
public:
  sampler_base(const CountSampler& cs, RJMCMC_TUPLE_ARGS) :
    m_kernel(RJMCMC_TUPLE_PARAMS),
    m_die(random(), boost::uniform_real<>(0,1)),
    m_count_sampler(cs)
  {}
  
  typedef tuple<RJMCMC_TUPLE_TYPES> Kernels;
  typedef boost::variate_generator<rjmcmc::generator&, boost::uniform_real<> > die_t;
  
  enum { size = tuple_size<Kernels>::value };
  enum { kernel_size = kernel_traits<Kernels>::size };
  
  template<typename Configuration, typename Modification>
    struct kernel_functor
  {
    Configuration& m_c;
    Modification& m_m;
    typedef double result_type;
    kernel_functor(Configuration &c, Modification &m) : m_c(c), m_m(m) {}
    template<typename T> inline result_type operator()(const T& t) { return t(m_c,m_m); }
  };
  
    // main sampling function
  template<typename Configuration>
    void operator()(Configuration &c, double temp)
  {
    typedef typename Configuration::modification Modification;
    Modification modif;
    m_temperature = temp;
    kernel_functor<Configuration,Modification> kf(c,modif);
    m_green_ratio  = random_apply(m_die(),m_kernel,kf);
    m_green_ratio *= m_count_sampler.pdf_ratio(c.size(), modif.birth_size()-modif.death_size());
    if(m_green_ratio<=0) {
      m_delta   =0;
      m_accepted=false;
      return;
    }
    m_delta       = c.delta_energy(modif);
    m_acceptance  = static_cast<Derived*>(this)->acceptance_probability();
    m_accepted    = ( m_die() < m_acceptance );
    if (m_accepted) c.apply(modif);
  }
  
// statistics accessors
  inline double acceptance () const { return m_acceptance; }
  inline double temperature() const { return m_temperature; }
  inline double delta() const { return m_delta; }
  inline double green_ratio() const { return m_green_ratio; }
  inline int kernel_id() const { return m_kernel_id; }
  inline bool accepted() const { return m_accepted; }
  
protected:
  double  m_temperature;
  double  m_delta;
  double  m_green_ratio;
  
private:
  double  m_acceptance;
  bool    m_accepted;
  int     m_kernel_id;
  Kernels m_kernel;
  die_t   m_die;
  CountSampler m_count_sampler;
};

}; // namespace rjmcmc

#endif // __RJMCMC_SAMPLER_BASE_HPP__
