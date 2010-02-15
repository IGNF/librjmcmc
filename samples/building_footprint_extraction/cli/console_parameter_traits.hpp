#ifndef CONSOLE_PARAMETER_TRAITS
#define CONSOLE_PARAMETER_TRAITS

#include "core/building_footprint_extraction_parameters.hpp"

struct console_parameter_traits { 
	template<typename T> struct control { 
		typedef void type;
		static inline void set(type *, const T&) {}
		static inline void get(type *, T&) {}
	};
};

typedef parameters<console_parameter_traits> param;

#endif
