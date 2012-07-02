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

#ifndef ENERGY_OPERATORS_HPP
#define ENERGY_OPERATORS_HPP

#include "rjmcmc/energy.hpp"

template<typename Energy, typename Operator>
class unary_operator_energy : public rjmcmc::energy<typename Operator::result_type>
{
public:
    typedef typename Energy::result_type result_type0;
    typedef typename Operator::result_type result_type;  // todo: check convertibility or equality (?) of the result_types
    template<typename T> result_type operator()(const T &t) const { return m_operator(m_energy(t)); }
    template<typename T,typename U> result_type operator()(const T &t, const U &u) const { return m_operator(m_energy(t,u)); }
    unary_operator_energy(const Energy& energy, Operator op=Operator())
        : m_energy(energy), m_operator(op) {}

private:
    Energy m_energy;
    Operator m_operator;
};

template<typename Energy0, typename Energy1, typename Operator>
struct binary_operator_energy : public rjmcmc::energy<typename Operator::result_type>
{
public:
    typedef typename Energy0::result_type result_type0;
    typedef typename Energy1::result_type result_type1;
    typedef typename Operator::result_type result_type;  // todo: check convertibility or equality (?) of the result_types
    template<typename T> result_type operator()(const T &t) const { return m_operator(m_energy0(t),m_energy1(t)); }
    template<typename T,typename U> result_type operator()(const T &t, const U &u) const { return m_operator(m_energy0(t,u),m_energy1(t,u)); }
    binary_operator_energy(const Energy0& energy0, const Energy1& energy1, Operator op=Operator())
        : m_energy0(energy0), m_energy1(energy1), m_operator(op) {}

private:
    Energy0 m_energy0;
    Energy1 m_energy1;
    Operator m_operator;
};


#include <functional>

template<typename Energy>
struct negate_energy : public unary_operator_energy<Energy,std::negate<typename Energy::result_type> >
{
    negate_energy(const Energy& energy)
        :  unary_operator_energy<Energy,std::negate<typename Energy::result_type> >(energy) {}
};

template<typename Energy0, typename Energy1>
struct plus_energy : public binary_operator_energy<Energy0,Energy1,std::plus<typename Energy0::result_type> >
{
    plus_energy(const Energy0& energy0, const Energy1& energy1)
        :  binary_operator_energy<Energy0,Energy1,std::plus<typename Energy0::result_type> >(energy0,energy1) {}
};

template<typename Energy0, typename Energy1>
struct minus_energy : public binary_operator_energy<Energy0,Energy1,std::minus<typename Energy0::result_type> >
{
    minus_energy(const Energy0& energy0, const Energy1& energy1)
        :  binary_operator_energy<Energy0,Energy1,std::minus<typename Energy0::result_type> >(energy0,energy1) {}
};

template<typename Energy0, typename Energy1>
struct multiplies_energy : public binary_operator_energy<Energy0,Energy1,std::multiplies<typename Energy0::result_type> >
{
    multiplies_energy(const Energy0& energy0, const Energy1& energy1)
        :  binary_operator_energy<Energy0,Energy1,std::multiplies<typename Energy0::result_type> >(energy0,energy1) {}
};

template<typename Energy0, typename Energy1>
struct divides_energy : public binary_operator_energy<Energy0,Energy1,std::divides<typename Energy0::result_type> >
{
    divides_energy(const Energy0& energy0, const Energy1& energy1)
        :  binary_operator_energy<Energy0,Energy1,std::divides<typename Energy0::result_type> >(energy0,energy1) {}
};

template<typename Energy0, typename Energy1>
struct modulus_energy : public binary_operator_energy<Energy0,Energy1,std::modulus<typename Energy0::result_type> >
{
    modulus_energy(const Energy0& energy0, const Energy1& energy1)
        :  binary_operator_energy<Energy0,Energy1,std::modulus<typename Energy0::result_type> >(energy0,energy1) {}
};

// operators

#include "constant_energy.hpp"

// SFINAE utility
template<typename Test, typename Value> struct enable { typedef Value type; };
#define ENERGY(E) typename enable<typename E::result_type,E>::type

template<typename E0, typename E1> plus_energy      <ENERGY(E0),ENERGY(E1)> operator+(const E0& e0, const E1& e1) { return plus_energy      <E0,E1>(e0,e1); }
template<typename E0, typename E1> minus_energy     <ENERGY(E0),ENERGY(E1)> operator-(const E0& e0, const E1& e1) { return minus_energy     <E0,E1>(e0,e1); }
template<typename E0, typename E1> multiplies_energy<ENERGY(E0),ENERGY(E1)> operator*(const E0& e0, const E1& e1) { return multiplies_energy<E0,E1>(e0,e1); }
template<typename E0, typename E1> divides_energy   <ENERGY(E0),ENERGY(E1)> operator/(const E0& e0, const E1& e1) { return divides_energy   <E0,E1>(e0,e1); }
template<typename E0, typename E1> modulus_energy   <ENERGY(E0),ENERGY(E1)> operator%(const E0& e0, const E1& e1) { return modulus_energy   <E0,E1>(e0,e1); }


template<typename E0> plus_energy      <ENERGY(E0),constant_energy<typename E0::result_type> >
operator+(const E0& e0, typename E0::result_type e1) { return plus_energy      <E0,constant_energy<typename E0::result_type> >(e0,e1); }
template<typename E0> minus_energy     <ENERGY(E0),constant_energy<typename E0::result_type> >
operator-(const E0& e0, typename E0::result_type e1) { return minus_energy     <E0,constant_energy<typename E0::result_type> >(e0,e1); }
template<typename E0> multiplies_energy<ENERGY(E0),constant_energy<typename E0::result_type> >
operator*(const E0& e0, typename E0::result_type e1) { return multiplies_energy<E0,constant_energy<typename E0::result_type> >(e0,e1); }
template<typename E0> divides_energy   <ENERGY(E0),constant_energy<typename E0::result_type> >
operator/(const E0& e0, typename E0::result_type e1) { return divides_energy   <E0,constant_energy<typename E0::result_type> >(e0,e1); }
template<typename E0> modulus_energy   <ENERGY(E0),constant_energy<typename E0::result_type> >
operator%(const E0& e0, typename E0::result_type e1) { return modulus_energy   <E0,constant_energy<typename E0::result_type> >(e0,e1); }

template<typename E1> plus_energy      <constant_energy<typename E1::result_type>,ENERGY(E1)>
operator+(typename E1::result_type e0, const E1& e1) { return plus_energy      <constant_energy<typename E1::result_type>,E1>(e0,e1); }
template<typename E1> minus_energy     <constant_energy<typename E1::result_type>,ENERGY(E1)>
operator-(typename E1::result_type e0, const E1& e1) { return minus_energy     <constant_energy<typename E1::result_type>,E1>(e0,e1); }
template<typename E1> multiplies_energy<constant_energy<typename E1::result_type>,ENERGY(E1)>
operator*(typename E1::result_type e0, const E1& e1) { return multiplies_energy<constant_energy<typename E1::result_type>,E1>(e0,e1); }
template<typename E1> divides_energy   <constant_energy<typename E1::result_type>,ENERGY(E1)>
operator/(typename E1::result_type e0, const E1& e1) { return divides_energy   <constant_energy<typename E1::result_type>,E1>(e0,e1); }
template<typename E1> modulus_energy   <constant_energy<typename E1::result_type>,ENERGY(E1)>
operator%(typename E1::result_type e0, const E1& e1) { return modulus_energy   <constant_energy<typename E1::result_type>,E1>(e0,e1); }

template<typename E> negate_energy<ENERGY(E)> operator-(const E& e) { return negate_energy<E>(e); }

#endif /*ENERGY_OPERATORS_HPP*/
