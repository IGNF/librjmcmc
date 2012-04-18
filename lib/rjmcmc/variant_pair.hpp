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

#ifndef VARIANT_PAIR_HPP
#define VARIANT_PAIR_HPP

#include <boost/mpl/at.hpp>
#include <boost/mpl/insert_range.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/variant.hpp>
#include <utility> //std::pair

namespace rjmcmc {

    template <typename T,typename U>
            struct std_pair { typedef std::pair<T,U> type; };

    template <typename T>
            struct variant_pairs { typedef boost::variant<T,std::pair<T,T> > type; };

    template<typename S, typename I>
    struct variant_pairs_detail {
        typedef typename boost::mpl::pop_front<I>::type tail;
        typedef typename boost::mpl::front<I>::type     head;
        typedef typename variant_pairs_detail<S,tail>::type T;
        typedef typename boost::mpl::end<T>::type T_end;
        typedef std_pair<head,boost::mpl::_1> make_std_pair;
        typedef typename boost::mpl::transform<S,make_std_pair>::type pair_types;
        typedef typename boost::mpl::insert_range<T,T_end,pair_types>::type type;
    };

    template<typename S>
    struct variant_pairs_detail<S,boost::mpl::l_end> {
        typedef S type;
    };


    /*  variant_pairs<boost::variant<Rectangle_2,Circle_2> >::type is a variant over
	Rectangle_2,
	Circle_2,
	std::pair<Rectangle_2,Rectangle_2>,
	std::pair<Circle_2,Rectangle_2>,
	std::pair<Circle_2,Circle_2>,
	std::pair<Rectangle_2,Circle_2>
*/
    template<BOOST_VARIANT_ENUM_PARAMS(typename T)>
    struct variant_pairs<boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> > {
        typedef boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> T;
        typedef typename T::types types;
        typedef typename variant_pairs_detail<types,types>::type pairs;
        typedef typename boost::make_variant_over<pairs>::type type;
    };

    template<typename V>
    struct make_variant_pair
    {
        typedef typename variant_pairs<V>::type result_type;
        template<typename T,typename U>
        inline result_type operator()(const T& t, const U& u) const {
            return result_type(std::make_pair(t,u));
        }
    };

}; // namespace rjmcmc

#endif // VARIANT_PAIR_HPP
