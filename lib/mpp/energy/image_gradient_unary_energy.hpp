#ifndef IMAGE_GRADIENT_UNARY_ENERGY_HPP_
#define IMAGE_GRADIENT_UNARY_ENERGY_HPP_

#include "geometry/integrated_flux.h"

template<typename Image, typename Value = double>
class image_gradient_unary_energy
{
public:
    typedef Value result_type;

    template<typename T>
    inline result_type operator()(const T &t) const
    {
        return integrated_flux(m_image,t);
    }

    image_gradient_unary_energy(const Image& image) : m_image(image) {}

private:
    Image m_image;
};


#endif /*IMAGE_GRADIENT_UNARY_ENERGY_HPP_*/
