#ifndef __BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_HPP__
#define __BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_HPP__

#include <cstdlib>
#include <vector>
#include <sstream>
#include <boost/program_options.hpp>

#include "core/pattern_singleton.hpp"

class building_footprint_extraction_parameters: public PatternSingleton<building_footprint_extraction_parameters>
{
	friend class PatternSingleton<building_footprint_extraction_parameters>;

public:
	inline double       InitialTemperature    () const { return m_initialTemperature; }
	inline unsigned int NbIterations          () const { return m_nbIterations; }
	inline unsigned int NbIterationsDump      () const { return m_nbIterationsDump; }
	inline unsigned int NbIterationsSave      () const { return m_nbIterationsSave; }
	inline double       DecreaseCoefficient   () const { return m_decreaseCoefficient; }
	inline double       BirthProbability      () const { return m_birthProbability; }
	inline double       DeathProbability      () const { return m_deathProbability; }
	inline double       ModifProbability      () const { return 1 - m_deathProbability - m_birthProbability; }
	inline std::string  InputDataFilePath     () const { return m_inputDataFilePath; }
	inline unsigned int RunningMinX           () const { return m_runningMinX; }
	inline unsigned int RunningMinY           () const { return m_runningMinY; }
	inline unsigned int RunningMaxX           () const { return m_runningMaxX; }
	inline unsigned int RunningMaxY           () const { return m_runningMaxY; }
	inline unsigned int SubSampling           () const { return m_subSampling; }
	inline double       VarianceGaussianFilter() const { return m_varianceGaussianFilter; }
	inline double       IntersectionSurfacePonderation() const { return m_ponderationSurfaceIntersection; }
	inline double       PointsDistancePonderation() const { return m_ponderationPointsDistance; }
	inline double       PointsDistanceMax     () const { return m_pointsDistanceMax; }
	inline double       RectangleMinimalSize  () const { return m_rectangleMinimalSize; }
	inline double       RectangleMaximalRatio () const { return m_rectangleMaximalRatio; }
	inline double       IndividualEnergy      () const { return m_individualEnergy; }
	inline double       SigmaD		  () const { return m_sigmaD; }

	void store_string_map(std::map<std::string, std::string> &map_options) const;
	void parse_string_map(const std::map<std::string, std::string> &string_map);
	void parse_config_file(const char *filename);
	bool parse_command_line(int argc, char **argv);

private:
	boost::program_options::options_description m_desc;
	boost::program_options::variables_map m_vm;

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

	building_footprint_extraction_parameters();
	building_footprint_extraction_parameters(const building_footprint_extraction_parameters &);
};

#endif /* __BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_HPP__ */
