#ifndef DELTA_ENERGY_END_TEST
#define DELTA_ENERGY_END_TEST

class delta_energy_end_test {
  public:
    delta_energy_end_test(unsigned int n) : m_i(0), m_n(n) {}
    template<typename Configuration, typename Sampler>
    inline bool operator()(Configuration&, Sampler& s, double, unsigned int) {
      m_i = (s.delta()==0)? (m_i+1) : 0;
      return m_i>=m_n;
    }
    void stop () { m_n=0; }
  private:
    unsigned int m_i, m_n;
};

#endif // DELTA_ENERGY_END_TEST
