#ifndef DIRECT_SAMPLER_HPP_
#define DIRECT_SAMPLER_HPP_

#include "rjmcmc/random_variant_init.hpp"

namespace marked_point_process {

    template<typename Density, typename ObjectSampler>
    class direct_sampler
    {
    public:
        direct_sampler( const Density & density,
                        const ObjectSampler& object_sampler) :
        m_density(density), m_object_sampler(object_sampler)
        {}

        template<typename Configuration> void operator()(Configuration &c, double temperature=0) const
        {
            typedef typename Configuration::value_type T;
            T res;
            c.clear();
            int n = m_density();
            for(int i=0; i<n; ++i) {
                rjmcmc::random_variant_init(res);
                rjmcmc::apply_visitor(m_object_sampler,res);
                c.insert(res);
            }
        }

        // new/old
        template<typename Configuration, typename Modification>
        double pdf_ratio(const Configuration &c, const Modification &m) const
        {
            double ratio = m_density.pdf_ratio(c.size(),c.size()+m.delta_size());
            for(typename Modification::birth_const_iterator b = m.birth_begin(); b!=m.birth_end(); ++b)
                ratio*=rjmcmc::apply_visitor(m_object_sampler.pdf(),*b);
            for(typename Modification::death_const_iterator d = m.death_begin(); d!=m.death_end(); ++d)
                ratio/=rjmcmc::apply_visitor(m_object_sampler.pdf(),c[*d]);
            return ratio;
        }

        template<typename Configuration>
        double pdf(const Configuration &c) const
        {
            double res = m_density.pdf(c);
            for(typename Configuration::const_iterator it = c.begin(); it!=c.end(); ++it)
                res*=rjmcmc::apply_visitor(m_object_sampler.pdf(),c[it]);
            return res;
        }

        inline const char * kernel_name(unsigned int i) const { return "direct"; }
        inline int kernel_id() const { return 0; }
        inline bool accepted() const { return true; }
        enum { kernel_size = 1 };
    private:
        Density  m_density;
        ObjectSampler m_object_sampler;
    };
}

#endif // DIRECT_SAMPLER_HPP_
