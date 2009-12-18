#ifndef __RJMCMC_BUILDING_DETECTION_H__
#define __RJMCMC_BUILDING_DETECTION_H__

// déclaration des types

#include "core/rectangle_node.hpp"
#include "core/building_footprint_extraction_parameters.hpp"

#include "core/rjmcmc_configuration.hpp"
#include "core/rjmcmc_sampler.hpp"
#include "boost/variant.hpp"

typedef building_footprint_extraction_parameters param;

typedef image_gradient_unary_energy building_unary_energy;
typedef intersection_area_binary_energy  building_binary_energy;
typedef trivial_accelerator building_accelerator;
typedef BoxIsValid building_is_valid;
//typedef Rectangle_2 node_types;
//typedef boost::variant<Rectangle_2> node_types;
typedef boost::variant<Rectangle_2,Circle_2> node_types;
typedef graph_configuration<node_types, building_unary_energy, building_binary_energy, building_is_valid,
	building_accelerator > building_configuration;

#define KERNELS uniform_birth_death_kernel,modification_kernel
typedef boost::tuple<KERNELS> kernels;
typedef rjmcmc_sampler<IF_VARIADIC(KERNELS,kernels)> building_sampler;


// déclaration du main

template<typename Visitor> void rjmcmc_building_footprint_extraction(Visitor& visitor) {
	geometric_temperature temperature(
		param::Instance()->m_initial_temperature,
		param::Instance()->m_decrease_coefficient
	);

	uniform_birth_death_kernel birthdeath(
		param::Instance()->m_birth_probability,
		param::Instance()->m_death_probability
	);
	modification_kernel modif;

	building_sampler sampler( IF_VARIADIC(,boost::make_tuple)(birthdeath,modif) );

	bbox_2::point_type pmin, pmax;
	pmin[0] = param::Instance()->m_running_min_x;
	pmin[1] = param::Instance()->m_running_min_y;
	pmax[0] = param::Instance()->m_running_max_x;
	pmax[1] = param::Instance()->m_running_max_y;
	bbox_2 bbox(pmin, pmax);


	building_unary_energy unary_energy(
		param::Instance()->m_individual_energy,
		param::Instance()->m_input_data_file_path,
		bbox,
		param::Instance()->m_sigma_d,
		param::Instance()->m_subsampling
	);
	building_binary_energy binary_energy(
		param::Instance()->m_ponderation_surface_intersection
	);
	building_is_valid is_valid(bbox,
		param::Instance()->m_rectangle_minimal_size,
		param::Instance()->m_rectangle_maximal_ratio
	);

	building_accelerator accelerator;
	building_configuration configuration(
		unary_energy,
		binary_energy,
		is_valid,
		accelerator);

	unsigned int iterations = param::Instance()->m_nb_iterations;
	unsigned int dump = param::Instance()->m_nb_iterations_dump;
	unsigned int save = param::Instance()->m_nb_iterations_save;

	visitor.begin(dump,save);
	for (unsigned int i=1; i<=iterations; ++i, ++temperature)
	{
		sampler(configuration,*temperature);
		if(!visitor.iterate(i,*temperature,configuration,sampler)) break;
	}
	visitor.end(configuration);
}

#endif // __RJMCMC_BUILDING_DETECTION_H__
