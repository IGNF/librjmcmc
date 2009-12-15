#include <fstream>
#include <iostream>

#include "building_footprint_extraction_parameters.hpp"
namespace po = boost::program_options;

void building_footprint_extraction_parameters::store_string_map(std::map<std::string, std::string> &map_options) const
{
	const std::vector<boost::shared_ptr<po::option_description> > & options = m_desc.options();
	for (unsigned int i = 0; i < options.size(); ++i)
	{
		std::string name = options[i]->long_name();
		std::ostringstream value;
		const po::variable_value& v = m_vm[name];
		if (v.empty()) continue;
		if (name == "input") {
			value << v.as<std::string>();
		} else if(name == "temp" || name == "deccoef" || name == "pbirth" || name == "pdeath" ||
		 name == "gaussian" || name == "minsize" || name == "maxratio" || name == "surface" ||
		 name == "energy" || name == "sigmaD") {
			value << v.as<double>();
		} else {
			value << v.as<unsigned int>();
		}
		map_options[options[i]->description()] = value.str();
	}
}

void building_footprint_extraction_parameters::parse_string_map(const std::map<std::string, std::string> &map_options)
{
	m_vm = po::variables_map();
	std::vector<std::string> parser_options;
	const std::vector<boost::shared_ptr<po::option_description> > & options = m_desc.options();
	for (unsigned int i = 0; i < options.size(); ++i)
	{
		std::string desc(options[i]->description());
		std::map<std::string, std::string>::const_iterator it = map_options.find(desc);
		if(it==map_options.end()) continue;
		parser_options.push_back("--" + options[i]->long_name());
		parser_options.push_back(it->second);
	}
	po::command_line_parser parser(parser_options);
	parser.options(m_desc);

	po::store(parser.run(), m_vm);
	po::notify(m_vm);
}

void building_footprint_extraction_parameters::parse_config_file(const char *filename)
{
	std::ifstream config_file(filename);
	po::store(po::parse_config_file(config_file, m_desc), m_vm);

	if (m_vm.count("config")) parse_config_file(m_vm["config"].as<std::string> ().c_str());
	po::notify(m_vm);
}

bool building_footprint_extraction_parameters::parse_command_line(int argc, char **argv)
{
	po::store(po::parse_command_line(argc, argv, m_desc), m_vm);
	if (m_vm.count("help"))
	{
		std::cout << m_desc << "\n";
		return false;
	}

	if (m_vm.count("config")) parse_config_file(m_vm["config"].as<std::string> ().c_str());
	po::notify(m_vm);
	return true;
}

building_footprint_extraction_parameters::building_footprint_extraction_parameters() :
	m_desc("building footprint extraction options")
{
	m_desc.add_options()
	("help,h", "Message d'aide...")
	("config,c", po::value<std::string>(), "Fichier de configuration")
	("temp,t", po::value<double>(&(m_initialTemperature))->default_value(150.), "Temperature initiale")
	("nbiter,I", po::value<unsigned int>(&(m_nbIterations))->default_value(15000000), "Nombre d'iterations")
	("nbdump,d", po::value<unsigned int>(&(m_nbIterationsDump))->default_value(10000), "Nombre d'iterations entre chaque affichage")
	("nbsave,S", po::value<unsigned int>(&(m_nbIterationsSave))->default_value(0), "Nombre d'iterations entre chaque sauvegarde")
	("deccoef,C", po::value<double>(&(m_decreaseCoefficient))->default_value(0.999999), "Coefficient de decroissance")
	("pbirth,B", po::value<double>(&(m_birthProbability))->default_value(0.1), "Probabilite de naissance")
	("pdeath,D", po::value<double>(&(m_deathProbability))->default_value(0.1), "Probabilite de mort")
	("input,i", po::value<std::string>(&(m_inputDataFilePath))->default_value("../data/ZTerrain_c3.tif"), "Fichier image d'entree")
	("xmin,x", po::value<unsigned int>(&(m_runningMinX))->default_value(0), "Zone a traiter (xmin)")
	("ymin,y", po::value<unsigned int>(&(m_runningMinY))->default_value(0), "Zone a traiter (ymin)")
	("xmax,X", po::value<unsigned int>(&(m_runningMaxX))->default_value(652), "Zone a traiter (xmax)")
	("ymax,Y", po::value<unsigned int>(&(m_runningMaxY))->default_value(662), "Zone a traiter (ymax)")
	("subsampling,u", po::value<unsigned int>(&(m_subSampling))->default_value(1), "Sous-échantillonnage")
	("gaussian,g", po::value<double>(&(m_varianceGaussianFilter))->default_value(2), "Variance du filtre gaussien en entrée")
	("sigmaD,G", po::value<double>(&(m_sigmaD))->default_value(1), "taille du noyau de flou pour le calcul des gradients")
	("minsize,m", po::value<double>(&(m_rectangleMinimalSize))->default_value(5), "Taille minimale d'un rectangle")
	("maxratio,M", po::value<double>(&(m_rectangleMaximalRatio))->default_value(5),
			"Rapport longueur / largeur maximal d'un rectangle")
	("surface,s", po::value<double>(&(m_ponderationSurfaceIntersection))->default_value(10), "Ponderation de la surface d'intersection")
	("energy,e", po::value<double>(&(m_individualEnergy))->default_value(250), "Energie d'existence d'un objet");
}
