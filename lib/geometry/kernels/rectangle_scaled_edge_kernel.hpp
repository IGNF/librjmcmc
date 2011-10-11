#ifndef RECTANGLE_SCALED_EDGE_KERNEL_HPP
#define RECTANGLE_SCALED_EDGE_KERNEL_HPP

#include "geometry/Rectangle_2.h"
#include "rjmcmc/random.hpp"
#include <boost/random/variate_generator.hpp>

namespace geometry {

    template<typename K>
    class rectangle_scaled_edge_kernel
    {
    private:
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_real<> > double_die_type;
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_smallint<> > int_die_type;
        mutable double_die_type m_dief;
        mutable int_die_type   m_die4;
    public:

        rectangle_scaled_edge_kernel() :
                m_dief(rjmcmc::random(), boost::uniform_real<>(0,1)),
                m_die4(rjmcmc::random(), boost::uniform_smallint<>(0,3)) {}

        typedef double result_type;
        typedef geometry::Rectangle_2<K> input_type;
        typedef geometry::Rectangle_2<K> output_type;

        result_type operator()(const input_type &in, output_type &out) const
        {
            out = in.scaled_edge(m_die4(),exp(0.5-m_dief()));
            return 1.; // TODO
        }
    };
}

#endif // RECTANGLE_SCALED_EDGE_KERNEL_HPP
