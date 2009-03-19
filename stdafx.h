#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/matis/float_images.hpp>
#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>
#include <boost/gil/extension/io/tiff_dynamic_io.hpp>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>
#include <boost/gil/image_view_factory.hpp>

using namespace boost::gil;

typedef pixel<float,devicen_layout_t<2> >		dev2n32F_pixel_t;
typedef dev2n32F_pixel_t*						dev2n32F_pixel_ptr_t;
typedef image_type<float,devicen_layout_t<2> >::type	dev2n32F_image_t;
typedef dev2n32F_image_t::view_t				dev2n32F_view_t;

