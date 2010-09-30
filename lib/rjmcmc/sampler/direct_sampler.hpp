#ifndef DIRECT_SAMPLER_HPP_
#define DIRECT_SAMPLER_HPP_

#include "rjmcmc/random_variant_init.hpp"

namespace marked_point_process {

    template<typename CountSampler, typename ObjectSampler>
    class direct_sampler
    {
    public:
        direct_sampler( const CountSampler & count_sampler,
                        const ObjectSampler& object_sampler) :
        m_count_sampler(count_sampler), m_object_sampler(object_sampler)
        {}

        template<typename Configuration> void operator()(Configuration &c, double temperature=0) const
        {
            typedef typename Configuration::value_type T;
            T res;
            c.clear();
            int n = m_count_sampler();
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
            double ratio = m_count_sampler.pdf_ratio(c,m);
            for(typename Modification::birth_const_iterator b = m.birth_begin(); b!=m.birth_end(); ++b)
                ratio*=rjmcmc::apply_visitor(m_object_sampler.pdf(),*b);
            for(typename Modification::death_const_iterator d = m.death_begin(); d!=m.death_end(); ++d)
                ratio/=rjmcmc::apply_visitor(m_object_sampler.pdf(),c[*d]);
            return ratio;
        }

        inline const char * kernel_name(unsigned int i) const { return "direct"; }
        inline int kernel_id() const { return 0; }
        inline bool accepted() const { return true; }
        enum { kernel_size = 1 };
    private:
        CountSampler  m_count_sampler;
        ObjectSampler m_object_sampler;
    };
}

#endif // DIRECT_SAMPLER_HPP_
