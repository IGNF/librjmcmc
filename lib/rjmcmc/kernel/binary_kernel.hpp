#ifndef BINARY_KERNEL_HPP_
#define BINARY_KERNEL_HPP_

#include "rjmcmc/random_variant_init.hpp"

namespace rjmcmc {

    template<typename Kernel0, typename Kernel1> class binary_kernel
    {
        double m_p, m_q[2];
        Kernel0 m_kernel0;
        Kernel1 m_kernel1;
        mutable unsigned int m_kernel_id;
    public:
        enum { size = 2 };
        inline unsigned int kernel_id() const { return m_kernel_id; }
        inline const char* name(unsigned int i) const { return (i)?m_kernel1.name():m_kernel0.name(); }

        binary_kernel(const Kernel0& k0, const Kernel1& k1, double q0=0.5, double q1=0.5) :
                m_kernel0(k0), m_kernel1(k1), m_p(q0+q1)
        {
            m_q[0]=q0;
            m_q[1]=q1;
        }

        inline void probability(double p) {
            if(m_p==0) { m_q[0]=0.5*p; }
            else { m_q[0]*=p/m_p; }
            m_p=p;
            m_q[1]=m_p-m_q[0];
        }
        inline double probability() const { return m_p; }

        template<typename Configuration, typename Modification>
        double operator()(double p, Configuration& c, Modification& modif) const
        {
            if(p<m_q[0]) {
                m_kernel_id = 0;
                double x = m_q[0]*m_kernel0            (c,modif); // pdf of the sampled modification
                double y = m_q[1]*m_kernel1.reverse_pdf(c,modif); // pdf of the reverse modification
                //std::cout << "y1="<<y << "\t";
                //std::cout << "x0="<<x << "\t";
                //std::cout << "y1/x0="<<y/x << "\t";
                return y/x;
            } else {
                m_kernel_id = 1;
                double x = m_q[1]*m_kernel1            (c,modif); // pdf of the sampled modification
                double y = m_q[0]*m_kernel0.reverse_pdf(c,modif); // pdf of the reverse modification
                //std::cout << "y0="<<y << "\t";
                //std::cout << "x1="<<x << "\t";
                //std::cout << "y0/x1="<<y/x << "\t";
                return y/x;
            }
        }
    };


    class uniform_death_kernel
    {
        typedef boost::variate_generator<rjmcmc::generator&, boost::uniform_smallint<> > die_type;
    public:
        inline const char* name() const { return "death"; }
        template<typename Configuration, typename Modification>
        double operator()(Configuration& c, Modification& modif) const
        {
            modif.clear();
            if(c.empty()) return 0.;
            typename Configuration::iterator it = c.begin();
            die_type die(random(), boost::uniform_smallint<>(0,c.size()-1));
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
            random_variant_init(res);
            double p = apply_visitor(m_generator,res);
            modif.insert_birth(res);
            return p;
        }

        template<typename Configuration, typename Modification>
        double reverse_pdf(const Configuration& c, const Modification& modif) const
        {
            return (modif.death_size()==1 && modif.birth_size()==0)
                    ? apply_visitor(m_generator.pdf(),c[*modif.death_begin()])
                        : (0.);
        }
    };

    /*
    template<typename Transform>
    class transform_kernel : public unary_kernel
    {
        typedef boost::variate_generator<rjmcmc::generator&, boost::uniform_real<> > die_t;
        double m_p, m_q, m_q_over_1_q, m_1_q_over_q;
        Transform m_trans;
        mutable unsigned int m_kernel_id;
        mutable die_t m_die;

    public:
        enum { size = 2 };
        inline unsigned int kernel_id() const { return m_kernel_id; }
        inline const char* name(unsigned int i) const { return "transform_kernel"; }

        transform_kernel(const Transform& t, double p=1., double q=0.5) :
                m_trans(t), m_p(p), m_q(q)
        {
            // assert(q>0 && q<1);
            m_q_over_1_q = q/(1-q);
            m_1_q_over_q = (1-q)/q;
        }

        inline void probability(double p) { m_p = p; }
        inline double probability() const { return m_p; }

        template<typename Configuration, typename Modification>
        double operator()(double p, Configuration& c, Modification& modif) const
        {
            typedef typename Transform::FT FT;
            enum { N =Transform::size };
            enum { N0=Transform::argument_size };
            enum { N1=Transform::result_size   };
            FT in [N];
            FT out[N];
            if(p<m_q*m_p) {
                m_kernel_id = 0;
                for(unsigned int i=N0; i<N; ++i) in[i]=m_die();
                typename Transform::argument_type in_objects;
                get_objects(c,in_objects);
                get_parameters(in_objects,in);
                double res = m_trans.apply(in,out);
                typename Transform::result_type out_objects;
                set_parameters(out,out_objects);
                return m_1_q_over_q*res;
            } else {
                m_kernel_id = 1;
                for(unsigned int i=N1; i<N; ++i) in[i]=m_die();
                typename Transform::result_type in_objects;
                get_objects(c,in_objects);
                get_parameters(in_objects,in);
                double res = m_trans.reverse(in,out);
                typename Transform::argument_type out_objects;
                set_parameters(out,out_objects);
                return m_q_over_1_q*res;
            }
        }

    };
*/

    // make convenience functions

    template<typename Generator>
    uniform_birth_kernel<Generator> make_uniform_birth_kernel(const Generator& g)
    {
	return uniform_birth_kernel<Generator>(g);
    }

    template<typename Kernel0, typename Kernel1>
    binary_kernel<Kernel0,Kernel1> make_binary_kernel(const Kernel0& k0, const Kernel1& k1, double q0=0.5, double q1=0.5)
    {
	return binary_kernel<Kernel0,Kernel1>(k0,k1,q0,q1);
    }

    template<typename Generator>
    binary_kernel<uniform_birth_kernel<Generator>,uniform_death_kernel>
            make_uniform_birth_death_kernel(const Generator& g, double q0=0.5, double q1=0.5)
    {
	return make_binary_kernel(make_uniform_birth_kernel(g),uniform_death_kernel(),q0,q1);
    }



}; // namespace rjmcmc

#endif // BINARY_KERNEL_HPP_
