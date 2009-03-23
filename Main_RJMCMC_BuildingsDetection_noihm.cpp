#include <iostream>
#include <string>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/barrier.hpp>

#include "BuildingsDetectorParameters.hpp"
#include "RectangleNode.hpp"
#include "RJMCMC_Detector.hpp"
#include "RJMCMC_Sampler.hpp"

typedef RJMCMC_Detector<RectangleNode, RectanglePointsPriorEnergyPolicy, ImageGradientEnergyPolicy >	BuildingsDetector;

const unsigned int nb_threads = 4;
unsigned int current_iter=0;
boost::mutex global_configuration_mutex;
boost::barrier global_barrier(nb_threads);
bool global_locked = false;

void run(BuildingsDetector &buildingsDetector, Sampler< BuildingsDetector > &sampler)
{
	while (current_iter < BuildingsDetectorParametersSingleton::Instance()->NbIterations())
	{
		Modification< BuildingsDetector  > modif;
		sampler.ProposeModification(buildingsDetector, modif);
		bool should_accept = sampler.AcceptModification(buildingsDetector, modif);
		bool will_accept = false;
		if (should_accept)
		{
			will_accept = global_configuration_mutex.try_lock();	
			if (will_accept)
				global_locked = true;
		}
		
		if (global_locked)
		{
			global_barrier.wait();
			if ( will_accept )
			{
				sampler.ApplyModification(buildingsDetector, modif);
				global_configuration_mutex.unlock();
				global_locked = false;
			}
			global_barrier.wait();
		}
		++current_iter;
	}
}



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

	std::vector<boost::thread *> all_threads;
	for (unsigned int i=0;i<nb_threads; ++i)
		all_threads.push_back( new boost::thread(run, boost::ref(buildingsDetector), boost::ref(sampler)) );
	while (current_iter < BuildingsDetectorParametersSingleton::Instance()->NbIterations())
	{
		sleep(2);
		my_out_stream.str("");
		my_out_stream << current_iter;
		my_out_stream << "\t" << buildingsDetector.GetNbVertices();
		my_out_stream << "\t" << sampler.Temperature();
		my_out_stream << "\t" << buildingsDetector.DataEnergy();
		my_out_stream << "\t" << buildingsDetector.PriorEnergy();
		my_out_stream << "\t" << buildingsDetector.DataEnergy() + buildingsDetector.PriorEnergy();

		std::cout << my_out_stream.str() << std::endl;
	}

	my_out_stream.str("");
	my_out_stream << "Iterations finished" << std::endl;
	my_out_stream << current_iter;
	my_out_stream << "\t" << buildingsDetector.GetNbVertices();
	my_out_stream << "\t" << sampler.Temperature();
	my_out_stream << "\t" << buildingsDetector.DataEnergy();
	my_out_stream << "\t" << buildingsDetector.PriorEnergy();
	my_out_stream << "\t" << buildingsDetector.DataEnergy() + buildingsDetector.PriorEnergy();
	my_out_stream << "Graph Data energy integrity : " << buildingsDetector.CheckGraphDataEnergyIntegrity() << std::endl;
	my_out_stream << "Graph Prior energy integrity: " << buildingsDetector.CheckGraphPriorEnergyIntegrity() << std::endl;
	my_out_stream << "Graph Structure integrity : " << buildingsDetector.CheckGraphStructureIntegrity() << std::endl;
	buildingsDetector.Dump(my_out_stream, true);

	std::cout << my_out_stream.str();

	return 0;
}
