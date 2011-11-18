#ifndef IMAGE_GRADIENT_UNARY_ENERGY_HPP_
#define IMAGE_GRADIENT_UNARY_ENERGY_HPP_

template<typename Image, typename Value = double>
class image_gradient_unary_energy
{
public:
    typedef Value result_type;

    template<typename T>
    result_type operator()(const T &t) const;

    image_gradient_unary_energy(const Image& image);

private:
    Image m_image;
};


#endif /*IMAGE_GRADIENT_UNARY_ENERGY_HPP_*/
