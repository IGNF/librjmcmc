#include <fstream>

#include "BuildingsDetectorParameters.hpp"

void BuildingsDetectorParametersSingleton::GetAsText(std::vector< std::pair<std::string, std::string> > &pileText) const
{
	const std::vector< boost::shared_ptr< po::option_description > > & options = m_desc.options();
	for (unsigned int i=0; i<options.size(); ++i)
	{
		if ((options[i]->long_name() == "help") || (options[i]->long_name() == "config") /*|| (options[i]->long_name() == "configxml")*/)
			continue;
		std::string param = options[i]->format_parameter();
		std::string default_value = param.substr(6, param.size()-6-1);
		pileText.push_back(make_pair(options[i]->description(), default_value ) );
	}
}

void BuildingsDetectorParametersSingleton::SetFromText(const std::vector< std::pair<std::string, std::string> > &pileText)
{
	std::vector < std::string > read_options;
	const std::vector< boost::shared_ptr< po::option_description > > & options = m_desc.options();
	for (unsigned int i=0; i<pileText.size(); ++i)
	{
		for (unsigned int j=0; j<options.size(); ++j)
		{
			if (options[j]->description() == pileText[i].first)
			{
				read_options.push_back("--" + options[j]->long_name());
				read_options.push_back(pileText[i].second);
				break;
			}
		}
	}
	po::command_line_parser parser(read_options);
	parser.options(m_desc);

	po::variables_map vm;
	po::store(parser.run(), vm);
	po::notify(vm);

	ComputeProb();
}

void BuildingsDetectorParametersSingleton::ReadConfigFile(const char *filename)
{
	po::variables_map vm;
	std::ifstream config_file(filename);
	po::store(po::parse_config_file(config_file, m_desc), vm);
	po::notify(vm);
	ComputeProb();
}

//void BuildingsDetectorParametersSingleton::ReadXMLConfigFile(const char *filename)
//{
//	// open the archive
//    std::ifstream file(filename);
//    assert(file.good());
//    boost::archive::xml_iarchive archive(file);
//
//    // restore the schedule from the archive
//    archive >> BOOST_SERIALIZATION_NVP(*this);
//    //serialize(archive,0);
//	ComputeProb();
//}

bool BuildingsDetectorParametersSingleton::ParseCmdLine(int argc, char **argv)
{
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, m_desc), vm);
	if (vm.count("help"))
	{
		std::cout << m_desc << "\n";
		return false;
	}

	if (vm.count("config"))
	{
		ReadConfigFile(vm["config"].as<std::string>().c_str());
	}
//	else if (vm.count("configxml"))
//	{
//		ReadXMLConfigFile(vm["configxml"].as<std::string>().c_str());
//	}
	else
	{
		po::notify(vm);
		ComputeProb();
	}
	return true;
}

BuildingsDetectorParametersSingleton::BuildingsDetectorParametersSingleton() :
	m_desc("buildings detector options"),
	m_initialTemperature(350.),
	m_nbIterations(5000000),
	m_nbIterationsDump(10000),
	m_nbIterationsSave(1000000),
	m_decreaseCoefficient(0.9999995),
	m_probaBirth(1. / 10.),
	m_probaDeath(1. / 10.),
//	m_inputDataFilePath("./data/SaintMichel/MNS_StMichel_sans_veget.tif"),
//	m_inputImageWidth(1695),
//	m_inputImageHeight(1575),
//	m_inputDataFilePath("./data/Marseille/Crop_MNE_Marseille.tif"),
//	m_inputDataFilePath("/home/olivier/work/data/MNESaintMande.tif"),
	m_inputDataFilePath("./data/ZTerrain_c3.tif"),
	m_runningOriginX(0),
	m_runningOriginY(0),
	m_runningWidth(653),
	m_runningHeight(663),
	m_varianceGaussianFilter(2.0),
	m_minimalSize(5.),
	m_maximalSize(200.),
	m_rectangleMaximalRatio(5.),
	m_ponderationSurfaceIntersection(1.),
	m_individualEnergy(150.)
{
	m_desc.add_options()
	("help,h", "Message d'aide...")
	("config,c",	po::value< std::string >(), "Fichier de configuration")
	//("configxml,X",	po::value< std::string >(), "Fichier de configuration XML (boost::serialization)")
	("temp,t", 		po::value< double >		(&(m_initialTemperature))->default_value(m_initialTemperature) , "Temperature initiale")
	("nbiter,I", 	po::value< unsigned int>(&(m_nbIterations))->default_value(m_nbIterations), "Nombre d'iterations")
	("nbdump,d",	po::value< unsigned int>(&(m_nbIterationsDump))->default_value(m_nbIterationsDump), "Nombre d'iterations entre chaque affichage")
	("nbsave,S",	po::value< unsigned int>(&(m_nbIterationsSave))->default_value(m_nbIterationsSave), "Nombre d'iterations entre chaque sauvegarde")
	("deccoef,C",	po::value< double >		(&(m_decreaseCoefficient))->default_value(m_decreaseCoefficient), "Coefficient de decroissance")
	("pbirth,B",	po::value< double >		(&(m_probaBirth))->default_value(m_probaBirth), "Probabilite de naissance")
	("pdeath,D",	po::value< double >		(&(m_probaDeath))->default_value(m_probaDeath), "Probabilite de mort")
	("input,i",		po::value< std::string >(&(m_inputDataFilePath))->default_value(m_inputDataFilePath), "Fichier image d'entree")
	("xorig,x",		po::value< unsigned int>(&(m_runningOriginX))->default_value(m_runningOriginX), "Origine (X) de la zone a traiter")
	("yorig,y",		po::value< unsigned int>(&(m_runningOriginY))->default_value(m_runningOriginY), "Origine (Y) de la zone a traiter")
	("width,w",		po::value< unsigned int>(&(m_runningWidth))->default_value(m_runningWidth), "Largeur de la zone a traiter")
	("height,h",	po::value< unsigned int>(&(m_runningHeight))->default_value(m_runningHeight), "Hauteur de la zone a traiter")
	("gaussian,g",	po::value< double >		(&(m_varianceGaussianFilter))->default_value(m_varianceGaussianFilter), "Variance du filtre gaussien en entrée")
	("minsize,m",	po::value< double >		(&(m_minimalSize))->default_value(m_minimalSize), "Taille minimale d'un objet")
	("maxsize,M",	po::value< double >		(&(m_maximalSize))->default_value(m_maximalSize), "Taille maximale d'un objet")
	("maxratio,R",	po::value< double >		(&(m_rectangleMaximalRatio))->default_value(m_rectangleMaximalRatio), "Rapport longueur / largeur maximal d'un rectangle")
	("surface,s",	po::value< double >		(&(m_ponderationSurfaceIntersection))->default_value(m_ponderationSurfaceIntersection), "Ponderation de la surface d'intersection")
	("energy,e",	po::value< double >		(&(m_individualEnergy))->default_value(m_individualEnergy), "Energie d'existence d'un objet")
	;
	ComputeProb();
}

void BuildingsDetectorParametersSingleton::ComputeProb()
{
	m_probas.clear();
	m_probas.push_back(m_probaBirth);
	m_probas.push_back(m_probas.back() + m_probaDeath);
	/// La probabilite de modification est le reste a 1.
	m_probas.push_back(1.);
}
