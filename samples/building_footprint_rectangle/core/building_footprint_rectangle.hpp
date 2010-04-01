#ifndef __BUILDING_FOOTPRINT_RECTANGLE_HPP__
#define __BUILDING_FOOTPRINT_RECTANGLE_HPP__

/************** application-specific types ****************/

#include "core/geometry.h"
typedef Rectangle_2 object;
//#include <boost/variant.hpp>
//typedef boost::variant<Rectangle_2> object;

#include "energy/box_is_valid.hpp"
typedef box_is_valid                         is_valid;

#include "energy/image_gradient_unary_energy.hpp"
typedef image_gradient_unary_energy          unary_energy;

#include "energy/intersection_area_binary_energy.hpp"
typedef intersection_area_binary_energy      binary_energy;

#include "energy/kernels.hpp"
typedef generator<is_valid>          generator_;
typedef modifier <is_valid>          modifier_;

/************** rjmcmc library types ****************/

#include "rjmcmc/temperature.hpp"
typedef rjmcmc::geometric_temperature                    temperature;

#include "rjmcmc/max_iteration_end_test.hpp"
typedef max_iteration_end_test                           end_test;

#include "rjmcmc/configuration.hpp"
typedef rjmcmc::graph_configuration
//typedef rjmcmc::vector_configuration
	<object, unary_energy, binary_energy>            configuration;

#include "rjmcmc/sampler.hpp"
typedef rjmcmc::uniform_birth_kernel<generator_>         birth_kernel;
typedef rjmcmc::uniform_death_kernel                     death_kernel;
typedef rjmcmc::binary_kernel<birth_kernel,death_kernel> birth_death_kernel;
typedef rjmcmc::modification_kernel<modifier_>           modification_kernel;
typedef rjmcmc::sampler<birth_death_kernel,modification_kernel> sampler;

/************** main ****************/

template<typename Param, typename View, typename Configuration>
void create_configuration(Param *p, const View& view, Configuration *&c) {
	Iso_rectangle_2 bbox(
		p->get<int>("xmin"),
		p->get<int>("ymin"),
		p->get<int>("xmax"),
		p->get<int>("ymax")
	);

	// energies
	unary_energy e1(
		p->get<double>("energy"),
                p->get<double>("ponderation_gradient")
	);
	
        e1.gradient(view,bbox);
	
	binary_energy e2(
		p->get<double>("surface")
	);

	// empty initial configuration
	c = new configuration(e1,e2);
}

template<typename Param, typename Sampler>
void create_sampler(Param *p, Sampler *&s) {
	Iso_rectangle_2 bbox(
		p->get<int>("xmin"),
		p->get<int>("ymin"),
		p->get<int>("xmax"),
		p->get<int>("ymax")
	);
	// sampler objects
	is_valid valid(bbox,
		p->get<double>("minsize"),
		p->get<double>("maxratio")
	);

	generator_ birth(valid);
	modifier_  modif(valid);

	birth_kernel        kbirth(birth);
	death_kernel        kdeath(      p->get<double>("poisson"));
	modification_kernel kmodif(modif,p->get<double>("poisson"));

	birth_death_kernel kbirthdeath(
		kbirth, kdeath,
		p->get<double>("pbirth"),
		p->get<double>("pdeath")
	);

	s = new sampler( kbirthdeath, kmodif );
}
template<typename Param, typename Temperature>
void create_temperature(Param *p, Temperature *&t)
{
	t = new temperature(
		p->get<double>("temp"),
		p->get<double>("deccoef")
	);
}

template<typename Param, typename EndTest>
void create_end_test(Param *p, EndTest *&e)
{
	e = new end_test(
		p->get<int>("nbiter")
	);
}

template<typename Param, typename Visitor>
void init_visitor(Param *p, Visitor& v)
{
	v.init( p->get<int>("nbdump"),
		p->get<int>("nbsave")
	);
}

#endif // __BUILDING_FOOTPRINT_RECTANGLE_HPP__
