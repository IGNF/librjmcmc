#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>
#include <boost/gil/extension/io/tiff_dynamic_io.hpp>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>
#include <boost/gil/image_view_factory.hpp>

/*
namespace boost { namespace gil {

namespace detail {
template <>
struct tiff_read_support_private<float,gray_t> {
    BOOST_STATIC_CONSTANT(bool,is_supported=true);
    BOOST_STATIC_CONSTANT(int,bit_depth=32);
    BOOST_STATIC_CONSTANT(int,color_type=PHOTOMETRIC_MINISBLACK);
};
template <>
struct tiff_write_support_private<float,gray_t> {
    BOOST_STATIC_CONSTANT(bool,is_supported=true);
    BOOST_STATIC_CONSTANT(int,bit_depth=32);
    BOOST_STATIC_CONSTANT(int,color_type=PHOTOMETRIC_MINISBLACK);
};

};
};};
*/

using namespace boost::gil;

typedef pixel<float,gray_layout_t>				gray32F_pixel_t;
typedef gray32F_pixel_t*						gray32F_pixel_ptr_t;
typedef image_type<float,gray_layout_t>::type	gray32F_image_t;
typedef gray32F_image_t::view_t				gray32F_view_t;

typedef pixel<float,devicen_layout_t<2> >		dev2n32F_pixel_t;
typedef dev2n32F_pixel_t*						dev2n32F_pixel_ptr_t;
typedef image_type<float,devicen_layout_t<2> >::type	dev2n32F_image_t;
typedef dev2n32F_image_t::view_t				dev2n32F_view_t;

