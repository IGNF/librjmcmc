#ifndef CIRCLE_2_COORDINATES_HPP_
#define CIRCLE_2_COORDINATES_HPP_

#include "rjmcmc/kernel/coordinates.hpp"
#include "geometry/Circle_2.h"
#include <boost/iterator/iterator_facade.hpp>

template<typename K>
struct circle_coordinates_iterator
    : public boost::iterator_facade<circle_coordinates_iterator<K>, const typename K::FT, boost::forward_traversal_tag>
{
    circle_coordinates_iterator() : m_circle(), m_i(3) {}
    explicit circle_coordinates_iterator(const geometry::Circle_2<K>& c) : m_circle(c), m_i(0) {}
private:
    friend class boost::iterator_core_access;
    void increment() { ++m_i; }
    typename K::FT dereference() const {
        switch(m_i) {
        case 0 : return m_circle.center().x();
        case 1 : return m_circle.center().y();
        case 2 : return m_circle.radius();
        default : return typename K::FT();
    }
    }
    const geometry::Circle_2<K>& m_circle;
    unsigned int m_i;
};

template<typename K>
struct coordinates_iterator< geometry::Circle_2<K> >
{
    typedef circle_coordinates_iterator<K> type;
    enum { dimension = 3 };
};

template<typename K>
struct object_from_coordinates< geometry::Circle_2<K> > {
    template<typename Iterator> geometry::Circle_2<K> operator()(Iterator it) {
        typename K::FT x = *it++;
        typename K::FT y = *it++;
        typename K::Point_2 p(x,y);
        return geometry::Circle_2<K>(p,*it);
    }
};

#endif // CIRCLE_2_COORDINATES_HPP_
