#ifndef __RJMCMC_BUILDING_DETECTION_H__
#define __RJMCMC_BUILDING_DETECTION_H__

// déclaration des types

#include "core/rectangle_node.hpp"
#include "core/building_footprint_extraction_parameters.hpp"

#include "core/rjmcmc_configuration.hpp"
#include "core/rjmcmc_sampler.hpp"

typedef building_footprint_extraction_parameters param;

typedef image_gradient_unary_energy building_unary_energy;
typedef intersection_area_binary_energy  building_binary_energy;
typedef trivial_accelerator building_accelerator;
typedef BoxIsValid building_is_valid;
typedef graph_configuration<Rectangle_2, building_unary_energy, building_binary_energy, building_is_valid, building_accelerator > building_configuration;

#define KERNELS uniform_birth_death_kernel,modification_kernel
typedef boost::tuple<KERNELS> kernels;
typedef rjmcmc_sampler<IF_VARIADIC(KERNELS,kernels)> building_sampler;


// déclaration du main

template<typename Visitor> void rjmcmc_building_footprint_extraction(Visitor& visitor) {
	geometric_temperature temperature(
		param::Instance()->m_initialTemperature,
		param::Instance()->m_decreaseCoefficient
	);

	uniform_birth_death_kernel birthdeath(
		param::Instance()->m_birthProbability,
		param::Instance()->m_deathProbability
	);
	modification_kernel modif;

	building_sampler sampler( IF_VARIADIC(,boost::make_tuple)(birthdeath,modif) );

	bbox_2::point_type pmin, pmax;
	pmin[0] = param::Instance()->m_runningMinX;
	pmin[1] = param::Instance()->m_runningMinY;
	pmax[0] = param::Instance()->m_runningMaxX;
	pmax[1] = param::Instance()->m_runningMaxY;
	bbox_2 bbox(pmin, pmax);


	building_unary_energy unary_energy(
		param::Instance()->m_individualEnergy,
		param::Instance()->m_inputDataFilePath,
		bbox,
		param::Instance()->m_sigmaD,
		param::Instance()->m_subSampling
	);
	building_binary_energy binary_energy(
		param::Instance()->m_ponderationSurfaceIntersection
	);
	building_is_valid is_valid(bbox,
		param::Instance()->m_rectangleMinimalSize,
		param::Instance()->m_rectangleMaximalRatio
	);

	building_accelerator accelerator;
	building_configuration configuration(unary_energy, binary_energy, is_valid, accelerator);

	unsigned int iterations = param::Instance()->m_nbIterations;
	unsigned int dump = param::Instance()->m_nbIterationsDump;
	unsigned int save = param::Instance()->m_nbIterationsSave;

	visitor.begin(dump,save);
	for (unsigned int i=1; i<=iterations; ++i, ++temperature)
	{
		sampler(configuration,*temperature);
		if(!visitor.iterate(i,*temperature,configuration,sampler)) break;
	}
	visitor.end(configuration);
}

#endif // __RJMCMC_BUILDING_DETECTION_H__
