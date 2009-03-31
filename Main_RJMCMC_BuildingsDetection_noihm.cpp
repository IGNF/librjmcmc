#include <iostream>
#include <string>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/barrier.hpp>

#include "BuildingsDetectorParameters.hpp"
#include "RectangleNode.hpp"
#include "RJMCMC_Detector.hpp"
#include "RJMCMC_Sampler.hpp"

/* Temps totaux pour buildings.cfg
 * lisS, lisS
 Total elapsed time (s) :  82.83
5000000	70	28.7297	-96630.6	2723.32	-93907.2
Graph Data energy integrity : 1.64437e-09
Graph Prior energy integrity: -9.64064e-10
Graph Structure integrity : 0
Energy : -93907.2
(Prior : 2723.32	Data : -96630.6)
Nb node : 70
Nb edge : 31

* vecS, lisS
Total elapsed time (s) :  81.64
5000000	74	28.7297	-99035.9	4648.26	-94387.7
Graph Data energy integrity : 1.60071e-10
Graph Prior energy integrity: 6.89397e-10
Graph Structure integrity : 0
Energy : -94387.7
(Prior : 4648.26	Data : -99035.9)
Nb node : 74
Nb edge : 35

 */ 

typedef RJMCMC_Detector<RectangleNode, RectanglePriorEnergyPolicy, ImageGradientEnergyPolicy >	BuildingsDetector;
unsigned int current_iter=0;

int main (int argc, char **argv)
{
	if (!BuildingsDetectorParametersSingleton::Instance()->ParseCmdLine(argc, argv))
		return -1;
	
	BBox::PointType size , origin;
	size[0] = BuildingsDetectorParametersSingleton::Instance()->RunningWidth()-1;
	size[1] = BuildingsDetectorParametersSingleton::Instance()->RunningHeight()-1;
	origin[0] = BuildingsDetectorParametersSingleton::Instance()->RunningOriginX();
	origin[1] = BuildingsDetectorParametersSingleton::Instance()->RunningOriginY();
	Sampler< BuildingsDetector > sampler( BuildingsDetectorParametersSingleton::Instance()->InitialTemperature() , BuildingsDetectorParametersSingleton::Instance()->DecreaseCoefficient(), BuildingsDetectorParametersSingleton::Instance()->CumulatedProbabilities() );

	BuildingsDetector buildingsDetector(BBox(size, origin));

	// Formattage du log sous forme de tableau, ca facilite la creation de graphiques ...
	std::ostringstream my_out_stream;
	my_out_stream << "Iteration\t";
	my_out_stream << "#Objects\t";
	my_out_stream << "TimePerIteration(ms)\t";
	my_out_stream << "Temperature\t";
	my_out_stream << "E_data\t";
	my_out_stream << "E_priori\t";
	my_out_stream << "E_total\t";
	std::cout << my_out_stream.str() << std::endl;

	clock_t clock_debut = clock();
	while (current_iter < BuildingsDetectorParametersSingleton::Instance()->NbIterations())
	{
		sampler.Itere(buildingsDetector);
		++current_iter;
	}
	clock_t clock_fin = clock();

	my_out_stream.str("");
	my_out_stream << "Iterations finished" << std::endl;
	my_out_stream << "Total elapsed time (s) :  " << double(clock_fin - clock_debut) / CLOCKS_PER_SEC << std::endl;
	my_out_stream << current_iter;
	my_out_stream << "\t" << buildingsDetector.GetNbVertices();
	my_out_stream << "\t" << sampler.Temperature();
	my_out_stream << "\t" << buildingsDetector.DataEnergy();
	my_out_stream << "\t" << buildingsDetector.PriorEnergy();
	my_out_stream << "\t" << buildingsDetector.DataEnergy() + buildingsDetector.PriorEnergy() << std::endl;
	my_out_stream << "Graph Data energy integrity : " << buildingsDetector.CheckGraphDataEnergyIntegrity() << std::endl;
	my_out_stream << "Graph Prior energy integrity: " << buildingsDetector.CheckGraphPriorEnergyIntegrity() << std::endl;
	my_out_stream << "Graph Structure integrity : " << buildingsDetector.CheckGraphStructureIntegrity() << std::endl;
	buildingsDetector.Dump(my_out_stream, true);

	std::cout << my_out_stream.str();

	return 0;
}
