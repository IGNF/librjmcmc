#ifndef GRADIENT_IMAGE_HPP
#define GRADIENT_IMAGE_HPP

#include <boost/gil/image.hpp>
#include <boost/gil/extension/matis/float_images.hpp>
typedef boost::gil::dev2n32F_image_t gradient_image_t;
typedef boost::gil::dev2n32F_view_t  gradient_view_t;
#include "image.hpp"

template<typename Image, typename View>
void gradient_image(Image& g, const View& v, double sigmaD);

template<typename GradView, typename Image, typename View, typename IsoRectangle>
void gradient_view(oriented<GradView>& grad, Image& img, const oriented<View>& v_in, const IsoRectangle& bbox, double sigmaD);

template<typename View, typename Image, typename IsoRectangle>
void gradient_view(oriented<View>& grad, Image& img, const std::string &file, const IsoRectangle& bbox, double sigmaD);

#endif // GRADIENT_IMAGE_HPP
