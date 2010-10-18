#ifndef CONSTANT_UNARY_ENERGY_HPP_
#define CONSTANT_UNARY_ENERGY_HPP_

class constant_unary_energy
{
public:
    typedef double result_type;
    template<typename T> result_type operator()(const T &) const { return m_energy; }
    constant_unary_energy(double energy) { m_energy = energy; }

private:
    double m_energy;
};

#endif /*CONSTANT_UNARY_ENERGY_HPP_*/
