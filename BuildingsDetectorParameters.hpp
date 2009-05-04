#ifndef BUILDINGSDETECTORPARAMETERS_HPP_
#define BUILDINGSDETECTORPARAMETERS_HPP_

#include <cstdlib>
#include <vector>
#include <sstream>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

// boost::serialization
//#include <boost/serialization/nvp.hpp>
//#include <boost/serialization/utility.hpp>
//#include <boost/serialization/list.hpp>
//#include <boost/serialization/version.hpp>

#include "PatternSingleton.hpp"

class BuildingsDetectorParametersSingleton : public PatternSingleton<BuildingsDetectorParametersSingleton>
{
	friend class PatternSingleton<BuildingsDetectorParametersSingleton>;
//	friend class boost::serialization::access;

public:
	inline double InitialTemperature()  const { return m_initialTemperature; }
	inline unsigned int NbIterations()  const { return m_nbIterations; }
	inline double DecreaseCoefficient() const { return m_decreaseCoefficient; }
	inline double ProbaBirth() const { return m_probaBirth; }
	inline double ProbaDeath() const { return m_probaDeath; }
	inline double ProbaModification() const { return 1-m_probaDeath-m_probaBirth; }
	inline const std::vector<double> &CumulatedProbabilities() const { return m_probas; }

	inline const std::string &InputImageFilePath() const { return m_inputImageFilePath; }
	inline const std::string &InputMaskFilePath() const { return m_inputMaskFilePath; }
	inline const std::string &OutputFilePath() const { return m_outputFilePath; }

	inline unsigned int RunningOriginX() const { return m_runningOriginX; }
	inline unsigned int RunningOriginY() const { return m_runningOriginY; }
	inline unsigned int RunningWidth() const { return m_runningWidth; }
	inline unsigned int RunningHeight() const { return m_runningHeight; }

	inline double VarianceGaussianFilter() const { return m_varianceGaussianFilter; }
	inline double IntersectionSurfacePonderation() const { return m_ponderationSurfaceIntersection; }

	inline double RectangleMaximalRatio() const { return m_rectangleMaximalRatio; }
	inline double RectangleMinimalSize() const { return m_rectangleMinimalSize; }
	inline double RectangleMaximalSize() const { return m_rectangleMaximalSize; }
	inline double CircleMinimalSize() const { return m_circleMinimalSize; }
	inline double CircleMaximalSize() const { return m_circleMaximalSize; }

	inline double IndividualEnergy() const { return m_individualEnergy; }
	inline double CoefIndividualEnergy() const { return m_coefIndividualEnergy; }

	inline unsigned int NbIterationsDump() const { return m_nbIterationsDump; }
	inline unsigned int NbIterationsSave() const { return m_nbIterationsSave; }

	void GetAsText(std::vector< std::pair<std::string, std::string> > &pileText) const;
	void SetFromText(const std::vector< std::pair<std::string, std::string> > &pileText);
	bool ParseCmdLine(int argc, char **argv);
	void ReadConfigFile(const char *filename);
//	void ReadXMLConfigFile(const char *filename);

private:
	po::options_description m_desc;
	double m_initialTemperature;
	unsigned int m_nbIterations;
	unsigned int m_nbIterationsDump;
	unsigned int m_nbIterationsSave;
	double m_decreaseCoefficient;
	double m_probaBirth;
	double m_probaDeath;
	std::string m_inputImageFilePath;
	std::string m_inputMaskFilePath;
	std::string m_outputFilePath;
	unsigned int m_runningOriginX;
	unsigned int m_runningOriginY;
	unsigned int m_runningWidth;
	unsigned int m_runningHeight;
	double m_varianceGaussianFilter;
	double m_rectangleMaximalRatio;
	double m_rectangleMinimalSize;
	double m_rectangleMaximalSize;
	double m_circleMinimalSize;
	double m_circleMaximalSize;
	double m_ponderationSurfaceIntersection;
	double m_individualEnergy;
	double m_coefIndividualEnergy;

	BuildingsDetectorParametersSingleton();
	BuildingsDetectorParametersSingleton(const BuildingsDetectorParametersSingleton &);
	// probas cumulees
	std::vector<double> m_probas;

	void ComputeProb();

};

#endif /* BUILDINGSDETECTORPARAMETERS_HPP_ */
