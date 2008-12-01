#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>

class RJMCMC_BuildingsDetectionThread
{
public :
	void *Entry();
};

#define USE_IHM 0
#include "RJMCMC_BuildingsDetectionThread.hpp"

namespace po = boost::program_options;

int main (int argc, char **argv)
{
	BuildingsDetectorParametersSingleton *s = BuildingsDetectorParametersSingleton::Instance();
	po::options_description desc("Allowed options");
	desc.add_options()
	("help,h", "Ce message d'aide...")
	("config,c",	po::value< std::string >(), "Fichier de configuration")	
	("temp,t", 		po::value< double >		(&(s->m_initialTemperature))->default_value(s->m_initialTemperature) , "Temperature initiale")
	("nbiter,I", 	po::value< unsigned int>(&(s->m_nbIterations))->default_value(s->m_nbIterations), "Nombre d'iterations")
	("nbdump,d",	po::value< unsigned int>(&(s->m_nbIterationsDump))->default_value(s->m_nbIterationsDump), "Nombre d'iterations entre chaque affichage")
	("deccoef,C",	po::value< double >		(&(s->m_decreaseCoefficient))->default_value(s->m_decreaseCoefficient), "Coefficient de decroissance")
	("pbirth,B",	po::value< double >		(&(s->m_probaBirth))->default_value(s->m_probaBirth), "Probabilite de naissance")
	("pdeath,D",	po::value< double >		(&(s->m_probaDeath))->default_value(s->m_probaDeath), "Probabilite de mort")
	("input,i",		po::value< std::string >(&(s->m_inputDataFilePath))->default_value(s->m_inputDataFilePath), "Fichier image d'entree")
	("xorig,x",		po::value< unsigned int>(&(s->m_runningOriginX))->default_value(s->m_runningOriginX), "Origine (X)")
	("yorig,y",		po::value< unsigned int>(&(s->m_runningOriginY))->default_value(s->m_runningOriginY), "Origine (Y)")
	("width,w",		po::value< unsigned int>(&(s->m_runningWidth))->default_value(s->m_runningWidth), "Largeur")
	("height,h",	po::value< unsigned int>(&(s->m_runningHeight))->default_value(s->m_runningHeight), "Hauteur")
	("gaussian,g",	po::value< double >		(&(s->m_varianceGaussianFilter))->default_value(s->m_varianceGaussianFilter), "Variance du filtre gaussien en entrée")
	("minsize,m",	po::value< double >		(&(s->m_rectangleMinimalSize))->default_value(s->m_rectangleMinimalSize), "Taille minimale d'un rectangle")
	("maxratio,M",	po::value< double >		(&(s->m_rectangleMaximalRatio))->default_value(s->m_rectangleMaximalRatio), "Rapport longueur / largeur maximal d'un rectangle")
	("surface,s",	po::value< double >		(&(s->m_ponderationSurfaceIntersection))->default_value(s->m_ponderationSurfaceIntersection), "Ponderation de la surface d'intersection")	
	("point,p",		po::value< double >		(&(s->m_ponderationPointsDistance))->default_value(s->m_ponderationPointsDistance), "Ponderation de distance des sommets")	
	("dmax,x",		po::value< double >		(&(s->m_pointsDistanceMax))->default_value(s->m_pointsDistanceMax), "Distance maximale d'inetrcation entre sommets")	
	("energy,e",	po::value< double >		(&(s->m_individualEnergy))->default_value(s->m_individualEnergy), "Energie d'existence d'un objet")	
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	if (vm.count("config"))
	{
		std::cout << vm["config"].as<std::string>() << std::endl;
		std::ifstream config_file(vm["config"].as<std::string>().c_str());
		po::store(po::parse_config_file(config_file, desc), vm);
	}
	po::notify(vm);
	
	std::cout << s->m_initialTemperature  << std::endl;
	
	if (vm.count("help")) 
	{
		std::cout << desc << "\n";
		return 1;
	}


	
	RJMCMC_BuildingsDetectionThread rien;
	rien.Entry();
	return 0;
}
