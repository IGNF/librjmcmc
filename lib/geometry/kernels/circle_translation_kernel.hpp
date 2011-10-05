#ifndef CIRCLE_TRANSLATION_KERNEL_HPP
#define CIRCLE_TRANSLATION_KERNEL_HPP

#include "geometry/Circle_2.h"

namespace geometry {
	
    template<typename K>
    class circle_translation_kernel
    {
    private:
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_real<> > die_type;
        mutable die_type m_die;
        double m_dx, m_dy;
    public:

        circle_translation_kernel(double dx = 20, double dy = 20) :
                m_die(rjmcmc::random(), boost::uniform_real<>(-1,1)),
                m_dx(dx), m_dy(dy) {}

        typedef double result_type;
        typedef geometry::Circle_2<K> input_type;
        typedef geometry::Circle_2<K> output_type;

        result_type operator()(const input_type &in, output_type &out) const
        {
            typedef typename K::Vector_2 Vector_2;
            Vector_2 v(m_dx*m_die(),m_dy*m_die());
            double d2 = v.squared_length();
            double d = sqrt(d2);
            if (m_die()>0) d = -d;
            out = geometry::Circle_2<K>(in.center()+v,geometry::radius(in)+d);
            return 1.; // TODO
        }
    };
}

#endif // CIRCLE_TRANSLATION_KERNEL_HPP
