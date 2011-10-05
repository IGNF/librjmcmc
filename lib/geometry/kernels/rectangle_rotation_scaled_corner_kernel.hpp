#ifndef RECTANGLE_ROTATION_SCALED_CORNER_KERNEL_HPP
#define RECTANGLE_ROTATION_SCALED_CORNER_KERNEL_HPP

#include "geometry/Rectangle_2.h"

namespace geometry {

    template<typename K>
    class rectangle_rotation_scaled_corner_kernel
    {
    private:
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_real<> > double_die_type;
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_smallint<> > int_die_type;
        mutable double_die_type m_dief;
        mutable int_die_type   m_die4;
        double m_dx, m_dy;
    public:

        rectangle_rotation_scaled_corner_kernel(double dx = 20, double dy = 20) :
                m_dief(rjmcmc::random(), boost::uniform_real<>(-1,1)),
                m_die4(rjmcmc::random(), boost::uniform_smallint<>(0,3)),
		m_dx(dx), m_dy(dy) {}

        typedef double result_type;
        typedef geometry::Rectangle_2<K> input_type;
        typedef geometry::Rectangle_2<K> output_type;

        result_type operator()(const input_type &in, output_type &out) const
        {
            typename K::Vector_2 v(m_dief()*m_dx,m_dief()*m_dy);
            out = in.rotation_scaled_corner(m_die4(),v);
            return 1.; // TODO
        }
    };
}

#endif // RECTANGLE_ROTATION_SCALED_CORNER_KERNEL_HPP
