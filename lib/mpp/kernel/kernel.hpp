
#ifndef MPP_KERNEL_HPP_
#define MPP_KERNEL_HPP_

#include "rjmcmc/kernel/unary_kernel.hpp"
#include "rjmcmc/kernel/binary_kernel.hpp"
#include "rjmcmc/variant_pair.hpp"
#include "rjmcmc/random_variant_init.hpp"

namespace mpp {

    class uniform_death_kernel
    {
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_smallint<> > die_type;
    public:
        inline const char* name() const { return "death"; }
        template<typename Configuration, typename Modification>
        double operator()(Configuration& c, Modification& modif) const
        {
            modif.clear();
            if(c.empty()) return 0.;
            typename Configuration::iterator it = c.begin();
            die_type die(rjmcmc::random(), boost::uniform_smallint<>(0,c.size()-1));
            std::advance(it, die());
            modif.insert_death(it);
            return 1.;
        }
        template<typename Configuration, typename Modification>
        double reverse_pdf(const Configuration& c, const Modification& modif) const
        {
            return (modif.birth_size()==1 && modif.death_size()==0)
                    ? 1. : (0.);
        }
    };

    template<typename Generator>
    class uniform_birth_kernel {
        Generator m_generator;
    public:
        inline const char* name() const { return "birth"; }
        uniform_birth_kernel(const Generator& generator) :
                m_generator(generator) {}

        template<typename Configuration, typename Modification>
        double operator()(Configuration& c, Modification& modif) const
        {
            typedef typename Configuration::value_type T;
            T res;
            rjmcmc::random_variant_init(res);
            double p = rjmcmc::apply_visitor(m_generator,res);
            modif.insert_birth(res);
            return p;
        }

        template<typename Configuration, typename Modification>
        double reverse_pdf(const Configuration& c, const Modification& modif) const
        {
            return (modif.death_size()==1 && modif.birth_size()==0)
                    ? rjmcmc::apply_visitor(m_generator.pdf(),c[*modif.death_begin()])
                        : (0.);
        }
    };

    // convenience make functions

    template<typename Generator>
    uniform_birth_kernel<Generator> make_uniform_birth_kernel(const Generator& g)
    {
        return uniform_birth_kernel<Generator>(g);
    }

    template<typename Kernel0, typename Kernel1>
    rjmcmc::binary_kernel<Kernel0,Kernel1> make_binary_kernel(const Kernel0& k0, const Kernel1& k1, double q0=0.5, double q1=0.5)
    {
        return rjmcmc::binary_kernel<Kernel0,Kernel1>(k0,k1,q0,q1);
    }

    template<typename Generator>
    rjmcmc::binary_kernel<uniform_birth_kernel<Generator>,uniform_death_kernel>
            make_uniform_birth_death_kernel(const Generator& g, double q0=0.5, double q1=0.5)
    {
        return make_binary_kernel(make_uniform_birth_kernel(g),uniform_death_kernel(),q0,q1);
    }

    template<typename Modifier>
    class modification_kernel : public rjmcmc::unary_kernel
    {
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_smallint<> > die_type;
        Modifier m_modifier;
    public:
        modification_kernel(const Modifier& m, double p=1) : rjmcmc::unary_kernel(p), m_modifier(m) {}

        inline const char* name(unsigned int) const { return "modif"; } //m_modifier.name(); }

        template<typename Configuration, typename Modification>
        double operator()(double, Configuration& c, Modification& modif) const
        {
            modif.clear();
            if(c.empty()) return 1.;
            typedef typename Configuration::value_type T;
            typedef typename Configuration::iterator iterator;

            /*
            typedef typename variant_pairs<T>::type MT;
            MT out, in;

            int n = c.size();
            die_type ddie(rjmcmc::random(), boost::uniform_smallint<>(1,2));
            die_type cdie(rjmcmc::random(), boost::uniform_smallint<>(0,n-1));
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
                modif.insert_death(it);
                modif.insert_death(it2);
                make_variant_pair<T> vmp;
                in = apply_visitor(vmp,c[it],c[it2]);
            }

            random_variant_init(out);
            */
            typedef typename Modifier::input_type  input_type;
            typedef typename Modifier::output_type output_type;

            int n = c.size();
            
             // TODO sample the input_type only and not all objects
            die_type cdie(rjmcmc::random(), boost::uniform_smallint<>(0,n-1));
            iterator it = c.begin();
            std::advance(it, cdie());
            const input_type *in = rjmcmc::variant_get<input_type>(&c[it]);
            if(!in) return 0;
            output_type out;

            double green_ratio = rjmcmc::apply_visitor(m_modifier,*in,out);
            modif.insert_death(it);
            modif.insert_birth(out);
            return green_ratio;
        }
    };

    template<typename Modifier>
    modification_kernel<Modifier> make_modification_kernel(const Modifier& m, double p=1)
    {
        return modification_kernel<Modifier>(m,p);
    }
}; // namespace rjmcmc

template<typename Modifier>
mpp::modification_kernel<Modifier> operator*(double p, const Modifier& m)
{
    return mpp::modification_kernel<Modifier>(m,p);
}

#endif // MPP_KERNEL_HPP_
