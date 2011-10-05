#ifndef RECTANGLE_SPLIT_MERGE_KERNEL_HPP
#define RECTANGLE_SPLIT_MERGE_KERNEL_HPP

#include "geometry/Rectangle_2.h"

namespace geometry {


    // size = 10

    template<typename K>
    class rectangle_split_kernel
    {
    private:
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_real<> > double_die_type;
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_smallint<> > int_die_type;
        mutable double_die_type m_dief;
        mutable int_die_type   m_die4;
    public:

        rectangle_split_kernel() :
                m_dief(rjmcmc::random(), boost::uniform_real<>(0,1)),
                m_die4(rjmcmc::random(), boost::uniform_smallint<>(0,3)) {}

        typedef double result_type;
        typedef geometry::Rectangle_2<K> input_type;
        typedef std::pair<geometry::Rectangle_2<K>,geometry::Rectangle_2<K> > output_type;

        result_type operator()(const input_type &in, output_type &out) const
        {
            int i = m_die4();
            double f = m_dief();
            double g = m_dief()*(1-f);
            out.first  = in.scaled_edge(i  ,f);
            out.second = in.scaled_edge(i+2,g);
            return 1.; // TODO
        }
    };

    template<typename K>
    class rectangle_merge_kernel
    {
    private:
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_smallint<> > int_die_type;
        mutable int_die_type   m_die4;
    public:

        rectangle_merge_kernel() :
                m_die4(rjmcmc::random(), boost::uniform_smallint<>(0,3)) {}

        typedef double result_type;
        typedef std::pair<geometry::Rectangle_2<K>,geometry::Rectangle_2<K> > input_type;
        typedef geometry::Rectangle_2<K> output_type;

        result_type operator()(const input_type &in, output_type &out) const
        {
            out = in.first.rotate(m_die4()).merge(in.second.rotate(m_die4()));
            return 1.; // TODO
        }
    };
}

#endif // RECTANGLE_SPLIT_MERGE_KERNEL_HPP
