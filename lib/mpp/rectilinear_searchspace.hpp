#ifndef RECTILINEAR_SEARCHSPACE_HPP_
#define RECTILINEAR_SEARCHSPACE_HPP_

#include <boost/fusion/container/map.hpp>
#include <boost/fusion/include/at_key.hpp>

template<typename T>
class rectilinear_searchspace
{
    template<typename U> class minmax_pair : public boost::fusion::pair<U,std::pair<U,U> > {};

    template<typename U>
    class minmax_map : public boost::fusion::map<minmax_pair<U> > {};

    template<BOOST_VARIANT_ENUM_PARAMS(typename U)>
    class minmax_map<boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)> >
        : public boost::fusion::result_of::as_map<
        typename boost::mpl::transform<typename boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)>::types, minmax_pair<boost::mpl::_> >::type>::type
    {
    };

public:
    typedef bool result_type;
    typedef T value_type;

    template<typename U> result_type inside(const U &u) const
    {
        typedef typename coordinates_iterator<U>::type iterator;
        static const int size = coordinates_iterator<U>::size;
        iterator it    = coordinates_begin(u);
        const std::pair<U,U>& minmax = boost::fusion::at_key<U>(m_minmax);
        iterator bmin  = coordinates_begin(minmax.first );
        iterator bmax  = coordinates_begin(minmax.second);
        for(int i=0; i<size; ++i, ++it) if(*it<*bmin || *it>*bmax) return false;
        return true;
    }

    template<typename U> inline const U& min() const { return boost::fusion::at_key<U>(m_minmax).first;  }
    template<typename U> inline const U& max() const { return boost::fusion::at_key<U>(m_minmax).second; }
    template<typename U> inline void min(const U& u) { boost::fusion::at_key<U>(m_minmax).first  = u; }
    template<typename U> inline void max(const U& u) { boost::fusion::at_key<U>(m_minmax).second = u; }

private:
    minmax_map<T> m_minmax;
};

#endif // RECTILINEAR_SEARCHSPACE
