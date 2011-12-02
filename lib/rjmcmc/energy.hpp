#ifndef ENERGY_HPP_
#define ENERGY_HPP_

namespace rjmcmc {

struct energy_tag {};

template<typename Value = double>
class energy
{
public:
	typedef Value result_type;
	typedef energy_tag is_energy;
};

}

#endif // ENERGY_HPP_

