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

        inline double probability() const { return m_p; }

        template<typename Configuration, typename Modification>
        double operator()(double p, Configuration& c, Modification& modif) const
        {
            if(p<m_q[0]) {
                m_kernel_id = 0;
                double x = m_q[0]*m_kernel0            (c,modif); // pdf of the sampled modification
                double y = m_q[1]*m_kernel1.reverse_pdf(c,modif); // pdf of the reverse modification
                return y/x;
            } else {
                m_kernel_id = 1;
                double x = m_q[1]*m_kernel1            (c,modif); // pdf of the sampled modification
                double y = m_q[0]*m_kernel0.reverse_pdf(c,modif); // pdf of the reverse modification
                return y/x;
            }
        }
    };

}; // namespace rjmcmc

#endif // BINARY_KERNEL_HPP_
