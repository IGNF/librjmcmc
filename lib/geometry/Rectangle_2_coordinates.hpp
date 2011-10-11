#ifndef RECTANGLE_2_COORDINATES_HPP_
#define RECTANGLE_2_COORDINATES_HPP_

#include "rjmcmc/kernel/coordinates.hpp"
#include "geometry/Rectangle_2.h"

template<typename K>
struct rectangle_coordinates_iterator
    : public boost::iterator_facade<rectangle_coordinates_iterator<K>, const typename K::FT, boost::forward_traversal_tag>
{
    rectangle_coordinates_iterator() : m_rectangle(), m_i(5) {}
    explicit rectangle_coordinates_iterator(const geometry::Rectangle_2<K>& r) : m_rectangle(r), m_i(0) {}
private:
    friend class boost::iterator_core_access;
    void increment() { ++m_i; }
    typename K::FT dereference() const {
        switch(m_i) {
        case 0 : return m_rectangle.center().x();
        case 1 : return m_rectangle.center().y();
        case 2 : return m_rectangle.normal().x();
        case 3 : return m_rectangle.normal().y();
        case 4 : return m_rectangle.ratio();
        default : return typename K::FT();
    }
    }
    const geometry::Rectangle_2<K>& m_rectangle;
    unsigned int m_i;
};

template<typename K>
struct coordinates_iterator< geometry::Rectangle_2<K> >
{
    typedef rectangle_coordinates_iterator<K> type;
    enum { dimension = 5 };
};

template<typename K>
struct object_from_coordinates< geometry::Rectangle_2<K> > {
    template<typename Iterator> geometry::Rectangle_2<K> operator()(Iterator it) {
        typename K::FT x = *it++;
        typename K::FT y = *it++;
        typename K::FT u = *it++;
        typename K::FT v = *it++;
        typename K::Point_2  p(x,y);
        typename K::Vector_2 n(u,v);
        return geometry::Rectangle_2<K>(p,n,*it);
    }
};

#endif // RECTANGLE_2_COORDINATES_HPP_
