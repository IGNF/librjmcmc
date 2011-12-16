#ifndef CIRCLE_2_COORDINATES_HPP_
#define CIRCLE_2_COORDINATES_HPP_

#include "rjmcmc/kernel/coordinates.hpp"
#include "geometry/Circle_2.h"
#include <boost/iterator/iterator_facade.hpp>


template<typename K>
struct circle_coordinates_iterator
        : public boost::iterator_facade<circle_coordinates_iterator<K>, const typename K::FT, boost::forward_traversal_tag>
{
    enum { dimension = 3 };
    circle_coordinates_iterator() : m_i(dimension) {}
    explicit circle_coordinates_iterator(const geometry::Circle_2<K>& c) : m_i(0)
    {
        m_coord[0] = c.center().x();
        m_coord[1] = c.center().y();
        m_coord[2] = c.radius();
    }

    typedef typename K::FT FT;

private:
    friend class boost::iterator_core_access;
    void increment() { ++m_i; }
    void advance(int i) { m_i+=i; }

    const FT& dereference() const {
        //assert(m_i<dimension);
        return m_coord[m_i];
    }

    FT m_coord[dimension];
    unsigned int m_i;
};

template<typename K>
struct coordinates_iterator< geometry::Circle_2<K> >
{
    typedef circle_coordinates_iterator<K> type;
    enum { dimension = type::dimension };
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
