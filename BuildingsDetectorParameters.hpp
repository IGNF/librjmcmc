#ifndef BUILDINGSDETECTORPARAMETERS_HPP_
#define BUILDINGSDETECTORPARAMETERS_HPP_

#include <cstdlib>
#include <vector>
#include <sstream>

#include "PatternSingleton.hpp"

class BuildingsDetectorParametersSingleton : public PatternSingleton<BuildingsDetectorParametersSingleton>
{
	friend class PatternSingleton<BuildingsDetectorParametersSingleton>;

public:
	double InitialTemperature()  const { return m_initialTemperature; }
	void InitialTemperature( double t ) { m_initialTemperature = t; }
	unsigned int NbIterations()  const { return m_nbIterations; }
	void NbIterations( unsigned int nb ) { m_nbIterations = nb; }
	unsigned int NbIterationsDump() const { return m_nbIterationsDump; }
	void NbIterationsDump( unsigned int nb ) { m_nbIterationsDump = nb; }
	double DecreaseCoefficient() const { return m_decreaseCoefficient; }
	void DecreaseCoefficient( double coef ) { m_decreaseCoefficient = coef; }
	double ProbaBirth() const { return m_probaBirth; }
	void ProbaBirth( double proba ) { m_probaBirth = proba; }
	double ProbaDeath() const { return m_probaDeath; }
	void ProbaDeath( double proba ) { m_probaDeath = proba; }
	double ProbaModification() const { return 1-m_probaDeath-m_probaBirth; }

	std::string InputDataFilePath() const { return m_inputDataFilePath; }
	void InputDataFilePath( const std::string &path ) { m_inputDataFilePath = path; }

	unsigned int RunningOriginX() const { return m_runningOriginX; }
	void RunningOriginX(unsigned int x) { m_runningOriginX = x; }
	unsigned int RunningOriginY() const { return m_runningOriginY; }
	void RunningOriginY(unsigned int y) { m_runningOriginY = y; }
	unsigned int RunningWidth() const { return m_runningWidth; }
	void RunningWidth( unsigned int width ) { m_runningWidth = width; }
	unsigned int RunningHeight() const { return m_runningHeight; }
	void RunningHeight( unsigned int height ) { m_runningHeight = height; }

	const std::vector<double> &CumulatedProbabilities() const { return m_probas; }
	double VarianceGaussianFilter() const { return m_varianceGaussianFilter; }
	void VarianceGaussianFilter(double variance) { m_varianceGaussianFilter = variance; }
	double IntersectionSurfacePonderation() const { return m_ponderationSurfaceIntersection; }
	void IntersectionSurfacePonderation(double coef) { m_ponderationSurfaceIntersection = coef; }
	double PointsDistancePonderation() const { return m_ponderationPointsDistance; }
	void PointsDistancePonderation(double coef) { m_ponderationPointsDistance = coef; }
	double PointsDistanceMax() const { return m_pointsDistanceMax; }
	void PointsDistanceMax(double coef) { m_pointsDistanceMax = coef; }

	double RectangleMinimalSize() const { return m_rectangleMinimalSize; }
	void RectangleMinimalSize(double size) { m_rectangleMinimalSize = size; }
	double RectangleMaximalRatio() const { return m_rectangleMaximalRatio; }
	void RectangleMaximalRatio(double ratio) { m_rectangleMaximalRatio = ratio; }

	double IndividualEnergy() const { return m_individualEnergy; }
	void IndividualEnergy(double energy) { m_individualEnergy = energy; }

	void GetAsText(std::vector< std::pair<std::string, std::string> > &pileText) const;
	void SetFromText(const std::vector< std::pair<std::string, std::string> > &pileText);


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
private:
	BuildingsDetectorParametersSingleton();
	// probas cumulees
	std::vector<double> m_probas;

	void ComputeProb();

};

#endif /* BUILDINGSDETECTORPARAMETERS_HPP_ */
