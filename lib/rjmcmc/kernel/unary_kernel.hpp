/*
#ifndef UNARY_KERNEL_HPP_
#define UNARY_KERNEL_HPP_

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

}

#endif // UNARY_KERNEL_HPP_
*/
