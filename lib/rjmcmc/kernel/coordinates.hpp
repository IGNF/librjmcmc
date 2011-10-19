#ifndef COORDINATES_HPP_
#define COORDINATES_HPP_

template<typename T> struct coordinates_iterator
{
    typedef typename T::iterator type;
    const static unsigned int dimension = T::dimension;
};

template<typename T>
typename coordinates_iterator<T>::type coordinates_begin(const T& t)
{
    return typename coordinates_iterator<T>::type(t);
}

template<typename T>
typename coordinates_iterator<T>::type coordinates_end(const T&)
{
    return typename coordinates_iterator<T>::type();
}

template<typename T>
struct object_from_coordinates {
    template<typename Iterator> T operator()(Iterator it) { return T(it); }
};

#endif // COORDINATES_HPP_

