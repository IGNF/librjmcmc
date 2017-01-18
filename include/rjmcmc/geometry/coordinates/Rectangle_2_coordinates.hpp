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

#ifndef RECTANGLE_2_COORDINATES_HPP
#define RECTANGLE_2_COORDINATES_HPP

#include "coordinates.hpp"
#include "rjmcmc/geometry/Rectangle_2.hpp"
#include <boost/iterator/iterator_facade.hpp>
#include <boost/random/uniform_smallint.hpp>


//    void advance(int i) { m_i+=i; }

template<typename K>
struct rectangle_coordinates_iterator
        : public boost::iterator_facade<rectangle_coordinates_iterator<K>, const typename K::FT, boost::forward_traversal_tag>
{

    enum { dimension = 5 };
    rectangle_coordinates_iterator() : m_i(dimension) {}
    template<typename Engine>
    explicit rectangle_coordinates_iterator(const geometry::Rectangle_2<K>& r, Engine& e) : m_i(0) {
        boost::uniform_smallint<> die(0,3);
        init(r.rotate(die(e)));
    }
    explicit rectangle_coordinates_iterator(const geometry::Rectangle_2<K>& r           ) : m_i(0) { init(r); }
    typedef typename K::FT FT;

private:
    void init(const geometry::Rectangle_2<K>& r)
    {
        m_coord[0] = r.center().x();
        m_coord[1] = r.center().y();
        m_coord[2] = r.normal().x();
        m_coord[3] = r.normal().y();
        m_coord[4] = r.ratio();
    }

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
    template<typename Iterator> geometry::Rectangle_2<K> operator()(Iterator it) const {
        typename K::FT x = *it++;
        typename K::FT y = *it++;
        typename K::FT u = *it++;
        typename K::FT v = *it++;
        typename K::Point_2  p(x,y);
        typename K::Vector_2 n(u,v);
        return geometry::Rectangle_2<K>(p,n,*it);
    }
};

#endif // RECTANGLE_2_COORDINATES_HPP
