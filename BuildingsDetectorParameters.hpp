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

	inline const std::string &InputDataFilePath() const { return m_inputDataFilePath; }
	inline const std::string &OutputFilePath() const { return m_outputFilePath; }

	inline unsigned int RunningOriginX() const { return m_runningOriginX; }
	inline unsigned int RunningOriginY() const { return m_runningOriginY; }
	inline unsigned int RunningWidth() const { return m_runningWidth; }
	inline unsigned int RunningHeight() const { return m_runningHeight; }

	inline double VarianceGaussianFilter() const { return m_varianceGaussianFilter; }
	inline double IntersectionSurfacePonderation() const { return m_ponderationSurfaceIntersection; }

	inline double MinimalSize() const { return m_minimalSize; }
	inline double MaximalSize() const { return m_maximalSize; }
	inline double RectangleMaximalRatio() const { return m_rectangleMaximalRatio; }
	inline double IndividualEnergy() const { return m_individualEnergy; }

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
	std::string m_inputDataFilePath;
	std::string m_outputFilePath;
	unsigned int m_runningOriginX;
	unsigned int m_runningOriginY;
	unsigned int m_runningWidth;
	unsigned int m_runningHeight;
	double m_varianceGaussianFilter;
	double m_minimalSize;
	double m_maximalSize;
	double m_rectangleMaximalRatio;
	double m_ponderationSurfaceIntersection;
	double m_individualEnergy;
//	template<class Archive>
//	void serialize(Archive & ar , const unsigned int &version)
//	{
//		ar & BOOST_SERIALIZATION_NVP(m_initialTemperature)
//		& BOOST_SERIALIZATION_NVP(m_nbIterations)
//		& BOOST_SERIALIZATION_NVP(m_nbIterationsDump)
//		& BOOST_SERIALIZATION_NVP(m_nbIterationsSave)
//		& BOOST_SERIALIZATION_NVP(m_decreaseCoefficient)
//		& BOOST_SERIALIZATION_NVP(m_probaBirth)
//		& BOOST_SERIALIZATION_NVP(m_probaDeath)
//		& BOOST_SERIALIZATION_NVP(m_inputDataFilePath)
//		& BOOST_SERIALIZATION_NVP(m_runningOriginX)
//		& BOOST_SERIALIZATION_NVP(m_runningOriginY)
//		& BOOST_SERIALIZATION_NVP(m_runningWidth)
//		& BOOST_SERIALIZATION_NVP(m_runningHeight)
//		& BOOST_SERIALIZATION_NVP(m_varianceGaussianFilter)
//		& BOOST_SERIALIZATION_NVP(m_ponderationSurfaceIntersection)
//		& BOOST_SERIALIZATION_NVP(m_ponderationPointsDistance)
//		& BOOST_SERIALIZATION_NVP(m_pointsDistanceMax)
//		& BOOST_SERIALIZATION_NVP(m_rectangleMinimalSize)
//		& BOOST_SERIALIZATION_NVP(m_rectangleMaximalRatio)
//		& BOOST_SERIALIZATION_NVP(m_individualEnergy)
//		;
//	}

	BuildingsDetectorParametersSingleton();
	BuildingsDetectorParametersSingleton(const BuildingsDetectorParametersSingleton &);
	// probas cumulees
	std::vector<double> m_probas;

	void ComputeProb();

};

#endif /* BUILDINGSDETECTORPARAMETERS_HPP_ */
