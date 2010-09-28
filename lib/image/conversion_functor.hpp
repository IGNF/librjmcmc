#ifndef CONVERSION_FUNCTOR_HPP
#define CONVERSION_FUNCTOR_HPP

#include <boost/gil/algorithm.hpp>

struct static_cast_color_converter
{
    template<class Src, class Dst>
    void operator()(const Src &src, Dst &dst) const
    {
        typedef typename boost::gil::channel_type<Dst>::type type;
        boost::gil::at_c<0>(dst) = static_cast<type>(boost::gil::at_c<0>(src));
    }
};

struct conversion_functor
{
public:
    typedef void result_type;
    template<typename Image, typename View>
    result_type operator()(Image& g, const View& v) const {
        g.recreate(v.dimensions());
        boost::gil::copy_and_convert_pixels(v,boost::gil::view(g),static_cast_color_converter());
    }
};



#endif // CONVERSION_FUNCTOR_HPP
