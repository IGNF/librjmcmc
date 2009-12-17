#ifndef __BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_HPP__
#define __BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_HPP__

#include <cstdlib>
#include <vector>
#include <sstream>
#include <boost/program_options.hpp>

#include "core/pattern_singleton.hpp"

struct building_footprint_extraction_parameters: public PatternSingleton<building_footprint_extraction_parameters>
{
	friend class PatternSingleton<building_footprint_extraction_parameters>;

	void store_string_map(std::map<std::string, std::string> &map_options) const;
	void parse_string_map(const std::map<std::string, std::string> &string_map);
	void parse_config_file(const char *filename);
	bool parse_command_line(int argc, char **argv);
	
	std::string long_name_from_description( const std::string &description );
	std::string description_from_long_name( const std::string &long_name );

	double m_initialTemperature;
	unsigned int m_nbIterations;
	unsigned int m_nbIterationsDump;
	unsigned int m_nbIterationsSave;
	unsigned int m_subSampling;
	double m_decreaseCoefficient;
	double m_birthProbability;
	double m_deathProbability;
	std::string m_inputDataFilePath;
	unsigned int m_runningMinX;
	unsigned int m_runningMinY;
	unsigned int m_runningMaxX;
	unsigned int m_runningMaxY;
	double m_varianceGaussianFilter;
	double m_rectangleMinimalSize;
	double m_rectangleMaximalRatio;
	double m_ponderationSurfaceIntersection;
	double m_ponderationPointsDistance;
	double m_pointsDistanceMax;
	double m_individualEnergy;
	double m_sigmaD;

private:
	boost::program_options::options_description m_desc;
	boost::program_options::variables_map m_vm;

	building_footprint_extraction_parameters();
	building_footprint_extraction_parameters(const building_footprint_extraction_parameters &);
};

#endif /* __BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_HPP__ */
