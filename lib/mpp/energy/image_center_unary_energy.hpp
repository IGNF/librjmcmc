#ifndef IMAGE_CENTER_UNARY_ENERGY_HPP_
#define IMAGE_CENTER_UNARY_ENERGY_HPP_

#include "rjmcmc/energy.hpp"

template<typename Image, typename Value = double>
class image_center_unary_energy : public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;
    template<typename T>
    result_type operator()(const T &t) const
    {
        typename Image::view_t view = m_image.view();
        int x0 = m_image.x0();
        int y0 = m_image.y0();
        int x = int(geometry::to_double(t.center().x())-x0);
        int y = int(geometry::to_double(t.center().y())-y0);
        if(x<0 || y<0 || x>= view.width() || y>= view.height()) return m_out;
        return view(x,y);
    }

    image_center_unary_energy(const Image& image, Value out = 0) : m_image(image), m_out(out) {}

private:
    Image m_image;
    Value m_out;
};


#endif /*IMAGE_CENTER_UNARY_ENERGY_HPP_*/
