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
		param::Instance()->InitialTemperature(),
		param::Instance()->DecreaseCoefficient()
	);

	uniform_birth_death_kernel birthdeath(
		param::Instance()->BirthProbability(),
		param::Instance()->DeathProbability()
	);
	modification_kernel modif;

	building_sampler sampler( IF_VARIADIC(,boost::make_tuple)(birthdeath,modif) );

	bbox_2::point_type pmin, pmax;
	pmin[0] = param::Instance()->RunningMinX();
	pmin[1] = param::Instance()->RunningMinY();
	pmax[0] = param::Instance()->RunningMaxX();
	pmax[1] = param::Instance()->RunningMaxY();
	bbox_2 bbox(pmin, pmax);


	building_unary_energy unary_energy(
		param::Instance()->IndividualEnergy(),
		param::Instance()->InputDataFilePath(),
		bbox,
		param::Instance()->SigmaD(),
		param::Instance()->SubSampling()
	);
	building_binary_energy binary_energy(
		param::Instance()->IntersectionSurfacePonderation()
	);
	building_is_valid is_valid(bbox,
		param::Instance()->RectangleMinimalSize(),
		param::Instance()->RectangleMaximalRatio()
	);

	building_accelerator accelerator;
	building_configuration configuration(unary_energy, binary_energy, is_valid, accelerator);

	unsigned int iterations = param::Instance()->NbIterations();
	unsigned int dump = param::Instance()->NbIterationsDump();
	unsigned int save = param::Instance()->NbIterationsSave();

	visitor.begin(dump,save);
	for (unsigned int i=1; i<=iterations; ++i, ++temperature)
	{
		sampler(configuration,*temperature);
		if(!visitor.iterate(i,*temperature,configuration,sampler)) break;
	}
	visitor.end(configuration);
}

#endif // __RJMCMC_BUILDING_DETECTION_H__
