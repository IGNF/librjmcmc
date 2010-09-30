#ifndef REJECTION_SAMPLER_HPP_
#define REJECTION_SAMPLER_HPP_

namespace rjmcmc
{

    template<typename Sampler, typename Pred>
    class rejection_sampler
    {
    public:
        rejection_sampler( const Sampler & sampler, Pred pred = Pred() ) :
                m_sampler(sampler), m_pred(pred)
        {}

        template<typename Configuration> void operator()(Configuration &c, double temperature=0)
        {
            // caveat: if m_sampler is (rj)mcmc, it may take a while to come back to valid configurations...
            // TODO: backup initial configuration ???
            do { m_sampler(c,temperature); } while (!m_pred(c));
        }

        inline const char * kernel_name(unsigned int i) const { return m_sampler.kernel_name(i); }
        inline unsigned int kernel_id() const { return m_sampler.kernel_id(); }
        inline bool accepted() const { return m_sampler.accepted(); }
        enum { kernel_size = Sampler::kernel_size };
    private:
        Sampler m_sampler;
        Pred    m_pred;
    };

    struct null_binary_energy_predicate
    {
        template<typename Configuration> bool operator()(const Configuration &c) const
        {
            return c.binary_energy()==0;
        }

    };

}

#endif // REJECTION_SAMPLER_HPP_
