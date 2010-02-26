#ifndef __BUILDING_FOOTPRINT_EXTRACTION_HPP__
#define __BUILDING_FOOTPRINT_EXTRACTION_HPP__

/************** application-specific types ****************/

#include "core/geometry.h"
#include <boost/variant.hpp>
//typedef Rectangle_2 object;
//typedef boost::variant<Rectangle_2> object;
typedef boost::variant<Rectangle_2,Circle_2> object;

#include "energy/box_is_valid.hpp"
typedef box_is_valid                         is_valid;

//#include "energy/image_gradient_unary_energy.hpp"
//typedef image_gradient_unary_energy          unary_energy;
#include "core/global_reconstruction_unary_energy.hpp"
typedef global_reconstruction_unary_energy          unary_energy;

#include "energy/intersection_area_binary_energy.hpp"
typedef intersection_area_binary_energy      binary_energy;

#include "energy/kernels.hpp"
typedef generator<is_valid>          generator_;
typedef modifier <is_valid>          modifier_;

/************** rjmcmc library types ****************/

#include "rjmcmc/temperature.hpp"
typedef rjmcmc::geometric_temperature                    temperature;

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

template<typename Visitor, typename View>
void building_footprint_extraction(Visitor& visitor, const View& view) {
	param *p = param::Instance();

	Iso_rectangle_2 bbox(
		p->get<int>("xmin"),
		p->get<int>("ymin"),
		p->get<int>("xmax"),
		p->get<int>("ymax")
	);

	// energies
	unary_energy e1(
		p->get<double>("energy"),
		p->get<double>("ponderation_gradient"), 
		p->get<double>("ponderation_ndvi")
	);
	
	e1.gradient(view,bbox); 
	e1.ndvi(p->get<boost::filesystem::path>("ndvi").string(),bbox);
	
	binary_energy e2(
		p->get<double>("surface")
	);

	// empty initial configuration
	configuration config(e1,e2);

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

	sampler sample( kbirthdeath, kmodif );

	// simulated annealing
	temperature temp(
		p->get<double>("temp"),
		p->get<double>("deccoef")
	);
	// visitation initialization
	visitor.begin(
		p->get<int>("nbdump"),
		p->get<int>("nbsave"),
		*temp,
		config
	);

	// main loop
	unsigned int iterations = p->get<int>("nbiter");
	for (unsigned int i=1; i<=iterations; ++i, ++temp)
	{
		sample(config,*temp);
		if(!visitor.iterate(i,*temp,config,sample)) break;
	}
	visitor.end(config);
}

#endif // __BUILDING_FOOTPRINT_EXTRACTION_HPP__
