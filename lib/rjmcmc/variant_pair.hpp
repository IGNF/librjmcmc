#ifndef __VARIANT_PAIR_HPP__
#define __VARIANT_PAIR_HPP__

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

#endif // __VARIANT_PAIR_HPP__
