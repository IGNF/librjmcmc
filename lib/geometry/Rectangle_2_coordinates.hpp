#ifndef RECTANGLE_2_COORDINATES_HPP_
#define RECTANGLE_2_COORDINATES_HPP_

#include "rjmcmc/kernel/coordinates.hpp"
#include "geometry/Rectangle_2.h"


//    void advance(int i) { m_i+=i; }

template<typename K>
struct rectangle_coordinates_iterator
        : public boost::iterator_facade<rectangle_coordinates_iterator<K>, const typename K::FT, boost::forward_traversal_tag>
{
    enum { dimension = 5 };
    rectangle_coordinates_iterator() : m_i(dimension) {}
    explicit rectangle_coordinates_iterator(const geometry::Rectangle_2<K>& r) : m_i(0)
    {
        m_coord[0] = r.center().x();
        m_coord[1] = r.center().y();
        m_coord[2] = r.normal().x();
        m_coord[3] = r.normal().y();
        m_coord[4] = r.ratio();
    }
    typedef typename K::FT FT;
private:
    friend class boost::iterator_core_access;
    void increment() { ++m_i; }
    const FT& dereference() const {
        //assert(m_i<dimension);
        return m_coord[m_i];
    }
    FT m_coord[dimension];
    unsigned int m_i;
};

template<typename K>
struct coordinates_iterator< geometry::Rectangle_2<K> >
{
    typedef rectangle_coordinates_iterator<K> type;
    enum { dimension = type::dimension };
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
