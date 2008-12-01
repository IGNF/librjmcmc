#ifndef BUILDINGSDETECTORPARAMETERS_HPP_
#define BUILDINGSDETECTORPARAMETERS_HPP_

#include <cstdlib>
#include <vector>
#include <sstream>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "PatternSingleton.hpp"

class BuildingsDetectorParametersSingleton : public PatternSingleton<BuildingsDetectorParametersSingleton>
{
	friend class PatternSingleton<BuildingsDetectorParametersSingleton>;

public:
	double InitialTemperature()  const { return m_initialTemperature; }
	unsigned int NbIterations()  const { return m_nbIterations; }
	unsigned int NbIterationsDump() const { return m_nbIterationsDump; }
	double DecreaseCoefficient() const { return m_decreaseCoefficient; }
	double ProbaBirth() const { return m_probaBirth; }
	double ProbaDeath() const { return m_probaDeath; }
	double ProbaModification() const { return 1-m_probaDeath-m_probaBirth; }

	std::string InputDataFilePath() const { return m_inputDataFilePath; }

	unsigned int RunningOriginX() const { return m_runningOriginX; }
	unsigned int RunningOriginY() const { return m_runningOriginY; }
	unsigned int RunningWidth() const { return m_runningWidth; }
	unsigned int RunningHeight() const { return m_runningHeight; }

	const std::vector<double> &CumulatedProbabilities() const { return m_probas; }
	double VarianceGaussianFilter() const { return m_varianceGaussianFilter; }
	double IntersectionSurfacePonderation() const { return m_ponderationSurfaceIntersection; }
	double PointsDistancePonderation() const { return m_ponderationPointsDistance; }
	double PointsDistanceMax() const { return m_pointsDistanceMax; }

	double RectangleMinimalSize() const { return m_rectangleMinimalSize; }
	double RectangleMaximalRatio() const { return m_rectangleMaximalRatio; }
	double IndividualEnergy() const { return m_individualEnergy; }

	void GetAsText(std::vector< std::pair<std::string, std::string> > &pileText) const;
	void SetFromText(const std::vector< std::pair<std::string, std::string> > &pileText);

	const po::options_description &GetAsProgramOption() const { return m_desc;}

private:
	po::options_description m_desc;
	double m_initialTemperature;
	unsigned int m_nbIterations;
	unsigned int m_nbIterationsDump;
	double m_decreaseCoefficient;
	double m_probaBirth;
	double m_probaDeath;
	std::string m_inputDataFilePath;
	unsigned int m_runningOriginX;
	unsigned int m_runningOriginY;
	unsigned int m_runningWidth;
	unsigned int m_runningHeight;
	double m_varianceGaussianFilter;
	double m_rectangleMinimalSize;
	double m_rectangleMaximalRatio;
	double m_ponderationSurfaceIntersection;
	double m_ponderationPointsDistance;
	double m_pointsDistanceMax;
	double m_individualEnergy;

	BuildingsDetectorParametersSingleton();
	BuildingsDetectorParametersSingleton(const BuildingsDetectorParametersSingleton &);
	// probas cumulees
	std::vector<double> m_probas;

	void ComputeProb();

};

#endif /* BUILDINGSDETECTORPARAMETERS_HPP_ */
