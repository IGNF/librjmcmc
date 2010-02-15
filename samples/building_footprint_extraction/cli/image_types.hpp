#ifndef RJMCMC_IMAGE_TYPES_HPP
#define RJMCMC_IMAGE_TYPES_HPP

#include <boost/gil/extension/dynamic_image/any_image.hpp>
#include <boost/gil/extension/matis/float_images.hpp>

namespace rjmcmc {

typedef boost::mpl::vector<
	boost::gil::gray8_image_t, 
	boost::gil::gray16_image_t,
	boost::gil::gray16s_image_t,
	boost::gil::gray32_image_t,
	boost::gil::gray32F_image_t,
	boost::gil::gray64F_image_t > image_types;
typedef boost::gil::any_image<image_types> any_image_t;

typedef any_image_t::const_view_t any_const_view_t;
typedef any_image_t::view_t       any_view_t;
};

#endif
