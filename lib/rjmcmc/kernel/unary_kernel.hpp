#ifndef UNARY_KERNEL_HPP_
#define UNARY_KERNEL_HPP_

#include "rjmcmc/variant_pair.hpp"
#include "rjmcmc/random_variant_init.hpp"

namespace rjmcmc {

    class unary_kernel
    {
        double m_p;
    public:
        enum { size = 1 };
        unary_kernel(double p=1.) : m_p(p) {}
        inline unsigned int kernel_id() const { return 0; }
        inline void probability(double p) { m_p = p; }
        inline double probability() const { return m_p; }
    };


    template<typename Modifier>
    class modification_kernel : public unary_kernel
    {
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_smallint<> > die_type;
        Modifier m_modifier;
    public:
        modification_kernel(const Modifier& m, double p=1) : unary_kernel(p), m_modifier(m) {}

        inline const char* name(unsigned int) const { return "modif"; } //m_modifier.name(); }

        template<typename Configuration, typename Modification>
        double operator()(double, Configuration& c, Modification& modif) const
        {
            modif.clear();
            if(c.empty()) return 1.;
            typedef typename Configuration::value_type T;
            typedef typename Configuration::iterator iterator;
            typedef typename variant_pairs<T>::type MT;
            MT out, in;

            int n = c.size();
            die_type ddie(random(), boost::uniform_smallint<>(1,2));
            die_type cdie(random(), boost::uniform_smallint<>(0,n-1));
            iterator it = c.begin();
            int i = cdie();
            std::advance(it, i);
            int num_deaths = std::min(n,ddie());
            if(num_deaths ==1) {
                modif.insert_death(it);
                in = c[it];
            } else {
                int i2;
                do { i2 = cdie(); } while(i==i2);
                iterator it2 = c.begin();
                std::advance(it2, cdie());
                modif.insert_death(it2);
                make_variant_pair<T> vmp;
                in = apply_visitor(vmp,c[it],c[it2]);
            }

            random_variant_init(out);
            double green_ratio = apply_visitor(m_modifier,in,out);
            apply_visitor(modif.birth_inserter(),out);
            return green_ratio;
        }
    };

}; // namespace rjmcmc

#endif // UNARY_KERNEL_HPP_
