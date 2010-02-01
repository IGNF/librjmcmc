#ifndef __RJMCMC_BUILDING_DETECTION_H__
#define __RJMCMC_BUILDING_DETECTION_H__

/************** application-specific types ****************/

#include "geometry/geometry.h"
#include <boost/variant.hpp>
//typedef Rectangle_2 object;
//typedef boost::variant<Rectangle_2> object;
typedef boost::variant<Rectangle_2,Circle_2> object;

#include "core/box_is_valid.hpp"
typedef box_is_valid                         is_valid;

#include "core/image_gradient_unary_energy.hpp"
typedef image_gradient_unary_energy          unary_energy;
//#include "core/global_reconstruction_unary_energy.hpp"
//typedef global_reconstruction_unary_energy          unary_energy;

#include "core/intersection_area_binary_energy.hpp"
typedef intersection_area_binary_energy      binary_energy;

#include "core/kernels.hpp"
typedef generator<is_valid>          generator_;
typedef modifier <is_valid>          modifier_;

#include "core/building_footprint_extraction_parameters.hpp"
typedef building_footprint_extraction_parameters         param;

/************** rjmcmc library types ****************/

#include "rjmcmc/temperature.hpp"
typedef rjmcmc::geometric_temperature                    temperature;

#include "rjmcmc/rjmcmc_configuration.hpp"
typedef rjmcmc::graph_configuration
//typedef rjmcmc::vector_configuration
	<object, unary_energy, binary_energy>            configuration;

#include "rjmcmc/rjmcmc_sampler.hpp"
typedef rjmcmc::uniform_birth_kernel<generator_>         birth_kernel;
typedef rjmcmc::uniform_death_kernel                     death_kernel;
typedef rjmcmc::binary_kernel<birth_kernel,death_kernel> birth_death_kernel;
typedef rjmcmc::modification_kernel<modifier_>           modification_kernel;
typedef rjmcmc::sampler<birth_death_kernel,modification_kernel> sampler;


/************** main ****************/

template<typename Visitor> void rjmcmc_building_footprint_extraction(Visitor& visitor) {
	param *p = param::Instance();

	bbox_2::point_type pmin, pmax;
	pmin[0] = p->m_running_min_x;
	pmin[1] = p->m_running_min_y;
	pmax[0] = p->m_running_max_x;
	pmax[1] = p->m_running_max_y;
	bbox_2 bbox(pmin, pmax);

	// energies
	unary_energy e1(
		p->m_individual_energy,
		p->m_input_data_file_path,
		bbox,
		p->m_sigma_d,
		p->m_subsampling
	);
	binary_energy e2(
		p->m_ponderation_surface_intersection
	);

	// empty initial configuration
	configuration config(e1,e2);

	// sampler objects
	is_valid valid(bbox,
		p->m_rectangle_minimal_size,
		p->m_rectangle_maximal_ratio
	);

	generator_ birth(valid);
	modifier_  modif(valid);

	birth_kernel        kbirth(birth);
	death_kernel        kdeath(      p->m_poisson);
	modification_kernel kmodif(modif,p->m_poisson);

	birth_death_kernel kbirthdeath(
		kbirth, kdeath,
		p->m_birth_probability,
		p->m_death_probability
	);

	sampler sample( kbirthdeath, kmodif );

	// visitation initialization
	visitor.begin(
		p->m_nb_iterations_dump,
		p->m_nb_iterations_save
	);

	// simulated annealing
	temperature temp(
		p->m_initial_temperature,
		p->m_decrease_coefficient
	);
	unsigned int iterations = p->m_nb_iterations;

	// main loop
	for (unsigned int i=1; i<=iterations; ++i, ++temp)
	{
		sample(config,*temp);
		if(!visitor.iterate(i,*temp,config,sample)) break;
	}
	visitor.end(config);
}

#endif // __RJMCMC_BUILDING_DETECTION_H__
