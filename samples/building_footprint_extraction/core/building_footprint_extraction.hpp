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

#include "core/intersection_area_binary_energy.hpp"
typedef intersection_area_binary_energy      binary_energy;

#include "core/kernels.hpp"
typedef generator<is_valid>          generator_;
typedef modifier <is_valid>          modifier_;

#include "core/building_footprint_extraction_parameters.hpp"
typedef building_footprint_extraction_parameters         param;

/************** rjmcmc library types ****************/

#include "core/temperature.hpp"
typedef rjmcmc::geometric_temperature                    temperature;

#include "core/rjmcmc_configuration.hpp"
typedef rjmcmc::graph_configuration
//typedef rjmcmc::vector_configuration
	<object, unary_energy, binary_energy, is_valid>  configuration;

#include "core/rjmcmc_sampler.hpp"
typedef rjmcmc::uniform_birth_kernel<generator_>         birth_kernel;
typedef rjmcmc::uniform_death_kernel                     death_kernel;
typedef rjmcmc::binary_kernel<birth_kernel,death_kernel> birth_death_kernel;
typedef rjmcmc::modification_kernel<modifier_>           modification_kernel;
typedef rjmcmc::sampler<birth_death_kernel,modification_kernel>    sampler;


/************** main ****************/

template<typename Visitor> void rjmcmc_building_footprint_extraction(Visitor& visitor) {
	temperature temp(
		param::Instance()->m_initial_temperature,
		param::Instance()->m_decrease_coefficient
	);

	bbox_2::point_type pmin, pmax;
	pmin[0] = param::Instance()->m_running_min_x;
	pmin[1] = param::Instance()->m_running_min_y;
	pmax[0] = param::Instance()->m_running_max_x;
	pmax[1] = param::Instance()->m_running_max_y;
	bbox_2 bbox(pmin, pmax);

	is_valid valid(bbox,
		param::Instance()->m_rectangle_minimal_size,
		param::Instance()->m_rectangle_maximal_ratio
	);

	modifier_            modif(valid);
	modification_kernel kmodif(modif);

	generator_    birth (valid);
	birth_kernel kbirth(birth);
	death_kernel kdeath;

	birth_death_kernel kbirthdeath(
		kbirth, kdeath,
		param::Instance()->m_birth_probability,
		param::Instance()->m_death_probability
	);

	sampler sample( kbirthdeath, kmodif );

	unary_energy e1(
		param::Instance()->m_individual_energy,
		param::Instance()->m_input_data_file_path,
		bbox,
		param::Instance()->m_sigma_d,
		param::Instance()->m_subsampling
	);
	binary_energy e2(
		param::Instance()->m_ponderation_surface_intersection
	);

	configuration config(e1,e2,valid);

	unsigned int iterations = param::Instance()->m_nb_iterations;
	unsigned int dump = param::Instance()->m_nb_iterations_dump;
	unsigned int save = param::Instance()->m_nb_iterations_save;

	visitor.begin(dump,save);
	for (unsigned int i=1; i<=iterations; ++i, ++temp)
	{
		sample(config,*temp);
		if(!visitor.iterate(i,*temp,config,sample)) break;
	}
	visitor.end(config);
}

#endif // __RJMCMC_BUILDING_DETECTION_H__
