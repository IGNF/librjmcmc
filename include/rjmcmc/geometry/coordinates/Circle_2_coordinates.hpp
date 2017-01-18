/***********************************************************************
This file is part of the librjmcmc project source files.

Copyright : Institut Geographique National (2008-2012)
Contributors : Mathieu Brédif, Olivier Tournaire, Didier Boldo
email : librjmcmc@ign.fr

This software is a generic C++ library for stochastic optimization.

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software. You can use,
modify and/or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty and the software's author, the holder of the
economic rights, and the successive licensors have only limited liability.

In this respect, the user's attention is drawn to the risks associated
with loading, using, modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean that it is complicated to manipulate, and that also
therefore means that it is reserved for developers and experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and, more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

***********************************************************************/

#ifndef CIRCLE_2_COORDINATES_HPP
#define CIRCLE_2_COORDINATES_HPP

#include "coordinates.hpp"
#include "rjmcmc/geometry/Circle_2.hpp"
#include <boost/iterator/iterator_facade.hpp>


template<typename K>
struct circle_coordinates_iterator
        : public boost::iterator_facade<circle_coordinates_iterator<K>, const typename K::FT, boost::forward_traversal_tag>
{
    enum { dimension = 3 };
    circle_coordinates_iterator() : m_i(dimension) {}
    template<typename Engine>
    explicit circle_coordinates_iterator(const geometry::Circle_2<K>& c, Engine& e) : m_i(0) { init(c); }
    explicit circle_coordinates_iterator(const geometry::Circle_2<K>& c           ) : m_i(0) { init(c); }

    typedef typename K::FT FT;

private:
    void init(const geometry::Circle_2<K>& c)
    {
        m_coord[0] = c.center().x();
        m_coord[1] = c.center().y();
        m_coord[2] = c.radius();
    }
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
    template<typename Iterator> geometry::Circle_2<K> operator()(Iterator it) const {
        typename K::FT x = *it++;
        typename K::FT y = *it++;
        typename K::Point_2 p(x,y);
        return geometry::Circle_2<K>(p,*it);
    }
};

#endif // CIRCLE_2_COORDINATES_HPP
