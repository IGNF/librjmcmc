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

#ifndef RJMCMC_VARIANT_HPP
#define RJMCMC_VARIANT_HPP

#include <boost/mpl/size.hpp>
#include <boost/variant.hpp>

// rjmcmc::apply_visitor generalizes boost::apply_visitor to directly call the function object if the argument is not a variant.

namespace rjmcmc {

    template<typename U, typename V>
    inline const U * variant_get(const V *v) { return v; }

    template<typename U, BOOST_VARIANT_ENUM_PARAMS(typename T) >
    inline const U * variant_get(const boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> *v)
    {
        return boost::get<U>(v);
    }

    template<typename U, typename V>
    inline U * variant_get(V *v) { return v; }

    template<typename U, BOOST_VARIANT_ENUM_PARAMS(typename T) >
    inline U * variant_get(boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> *v)
    {
        return boost::get<U>(v);
    }

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

#endif // RJMCMC_VARIANT_HPP
