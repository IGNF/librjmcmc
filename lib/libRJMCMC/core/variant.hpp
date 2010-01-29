#ifndef __RJMCMC_VARIANT_HPP__
#define __RJMCMC_VARIANT_HPP__

#include <boost/variant.hpp>

// rjmcmc::apply_visitor generalizes boost::apply_visitor to directly call the function object if the argument is not a variant.

namespace rjmcmc {

template<typename Visitor, typename Variant> 
  inline typename Visitor::result_type apply_visitor(const Visitor &v, const Variant &v1) { return v(v1); }

template<typename Visitor, typename Variant> 
  inline typename Visitor::result_type apply_visitor(const Visitor &v, Variant &v1) { return v(v1); }

template<typename Visitor, typename Variant1, typename Variant2> 
  inline  typename Visitor::result_type apply_visitor(const Visitor &v, const Variant1 &v1, const Variant2 &v2) { return v(v1,v2); }

template<typename Visitor, typename Variant1, typename Variant2> 
  inline  typename Visitor::result_type apply_visitor(const Visitor &v, const Variant1 &v1, Variant2 &v2) { return v(v1,v2); }

template<typename Visitor, BOOST_VARIANT_ENUM_PARAMS(typename T) > 
inline typename Visitor::result_type apply_visitor(const Visitor &v,
	const boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> &v1)
{
	return boost::apply_visitor(v,v1);
}

template<typename Visitor, BOOST_VARIANT_ENUM_PARAMS(typename T) > 
inline typename Visitor::result_type apply_visitor(const Visitor &v,
	boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> &v1)
{
	return boost::apply_visitor(v,v1);
}

template<typename Visitor,
	BOOST_VARIANT_ENUM_PARAMS(typename T),
	BOOST_VARIANT_ENUM_PARAMS(typename U) > 
inline typename Visitor::result_type apply_visitor(const Visitor &v,
	const boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> &v1,
	const boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)> &v2)
{
	return boost::apply_visitor(v,v1,v2);
}


template<typename Visitor,
	BOOST_VARIANT_ENUM_PARAMS(typename T),
	BOOST_VARIANT_ENUM_PARAMS(typename U) > 
inline typename Visitor::result_type apply_visitor(const Visitor &v,
	const boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> &v1,
	boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)> &v2)
{
	return boost::apply_visitor(v,v1,v2);
}


template<typename Variant> struct variant_size {
	enum { value = 1 };
};
template<BOOST_VARIANT_ENUM_PARAMS(typename T)>
struct variant_size<boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> > {
	typedef boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> T;
	enum { value = boost::mpl::size<typename T::types>::value };
};

}; // namespace rjmcmc

#endif // __RJMCMC_VARIANT_HPP__
