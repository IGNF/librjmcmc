#include "BuildingsDetectorParameters.hpp"

void BuildingsDetectorParametersSingleton::ParseCmdLine(int argc, char **argv)
{
	if ((argc-1) % 2 != 0)
	Usage();
	for (int i = 1; i<argc; i += 2)
	{
		std::string str(argv[i]);
		if (str == "-img")
		InputDataFilePath(argv[i+1]);
		else if (str == "-nbi")
		NbIterations(atoi(argv[i+1]));
		else if (str == "-nbd")
		NbIterationsDump(atoi(argv[i+1]));
		else if (str == "-sizex")
		InputImageWidth(atoi(argv[i+1]));
		else if (str == "-sizey")
		InputImageHeight(atoi(argv[i+1]));
		else if (str == "-temp")
		InitialTemperature(atof(argv[i+1]));
		else if (str == "-coef")
		DecreaseCoefficient(atof(argv[i+1]));
		else if (str == "-Pbirth")
		ProbaBirth(atof(argv[i+1]));
		else if (str == "-Pdeath")
		ProbaDeath(atof(argv[i+1]));
		else
		Usage();
	}
	ComputeProb();
}

void BuildingsDetectorParametersSingleton::Usage() const
{
	std::cout
			<< "Usage : ./XXXX [-img data.bmp][-nbi 10000000] [-nbd 10000] [-sizex 50] [-sizey 50] [-temp 100] [-coef 0.999999] [-Pbirth 0.1] [-Pdeath 0.1]"
			<< std::endl;
}

void BuildingsDetectorParametersSingleton::GetAsText(std::vector< std::pair<std::string, std::string> > &pileText) const
{
	pileText.push_back(std::make_pair("Nom de l'image de donnees", InputDataFilePath() ) );
	std::ostringstream oss;
	oss << NbIterations();
	pileText.push_back(make_pair("Nombre d'iterations", oss.str() ) );
	oss.str("");
	oss << NbIterationsDump();
	pileText.push_back(make_pair("Nombre d'iterations entre chaque visualisation", oss.str() ) );
	oss.str("");
	oss << InputImageWidth();
	pileText.push_back(make_pair("Taille en X", oss.str() ) );
	oss.str("");
	oss << InputImageHeight();
	pileText.push_back(make_pair("Taille en Y", oss.str() ) );
	oss.str("");
	oss << VarianceGaussianFilter();
	pileText.push_back(make_pair("Variance filtre gaussien", oss.str() ) );
	oss.str("");
	oss << 	RectangleMinimalSize();
	pileText.push_back(make_pair("Rectangle : taille minimale", oss.str() ) );
	oss.str("");
	oss << RectangleMaximalRatio();
	pileText.push_back(make_pair("Rectangle : Ratio L/H maximal", oss.str() ) );
	oss.str("");
	oss << InitialTemperature();
	pileText.push_back(make_pair("Temperature initiale", oss.str() ) );
	oss.str("");
	oss << DecreaseCoefficient();
	pileText.push_back(make_pair("Coefficient de descente", oss.str() ) );
	oss.str("");
	oss << ProbaBirth();
	pileText.push_back(make_pair("Probabilite naissance", oss.str() ) );
	oss.str("");
	oss << ProbaDeath();
	pileText.push_back(make_pair("Probabilite mort", oss.str() ) );
	oss.str("");
	oss << IntersectionSurfacePonderation();
	pileText.push_back(make_pair("Ponderation surface intersection", oss.str() ) );
	oss.str("");
	oss << PointsDistancePonderation();
	pileText.push_back(make_pair("Ponderation distance points", oss.str() ) );
	oss.str("");
	oss << PointsDistanceMax();
	pileText.push_back(make_pair("Maximum distance points", oss.str() ) );
	oss.str("");
	oss << IndividualEnergy();
	pileText.push_back(make_pair("Energie d'existence d'un objet", oss.str() ) );
}

void BuildingsDetectorParametersSingleton::SetFromText(const std::vector< std::pair<std::string, std::string> > &pileText)
{

	for (unsigned int i=0; i<pileText.size(); ++i)
	{
		std::string type = pileText[i].first;
		std::istringstream iss;
		iss.str(pileText[i].second);
		if (type == std::string("Nom de l'image de donnees"))
		{
			std::string nomData;
			iss >> nomData;
			InputDataFilePath(nomData);
		}
		else if (type == std::string("Nombre d'iterations"))
		{
			unsigned int nbi;
			iss >> nbi;
			NbIterations(nbi);
		}
		else if (type == std::string("Nombre d'iterations entre chaque visualisation"))
		{
			unsigned int nbi;
			iss >> nbi;
			NbIterationsDump(nbi);
		}
		else if (type == std::string("Taille en X"))
		{
			unsigned int sizex;
			iss >> sizex;
			InputImageWidth(sizex);
		}
		else if (type == std::string("Taille en Y"))
		{
			unsigned int sizey;
			iss >> sizey;
			InputImageHeight(sizey);
		}
		else if (type == std::string("Variance filtre gaussien"))
		{
			double variance;
			iss >> variance;
			VarianceGaussianFilter(variance);
		}
		else if (type ==std::string("Rectangle : taille minimale"))
		{
			double minSize;
			iss >> minSize;
			RectangleMinimalSize(minSize);
		}
		else if (type ==std::string("Rectangle : Ratio L/H maximal"))
		{
			double ratio;
			iss >> ratio;
			RectangleMaximalRatio(ratio);
		}
		else if (type == std::string("Temperature initiale"))
		{
			double tempInitiale;
			iss >> tempInitiale;
			InitialTemperature(tempInitiale);
		}
		else if (type == std::string("Coefficient de descente"))
		{
			double coefDescente;
			iss >> coefDescente;
			DecreaseCoefficient(coefDescente);

		}
		else if (type == std::string("Probabilite naissance"))
		{
			double probaBirth;
			iss >> probaBirth;
			ProbaBirth(probaBirth);
		}
		else if (type == std::string("Probabilite mort"))
		{
			double probaDeath;
			iss >> probaDeath;
			ProbaDeath(probaDeath);
		}
		else if (type == std::string("Ponderation surface intersection"))
		{
			double ponderation;
			iss >> ponderation;
			IntersectionSurfacePonderation(ponderation);
		}
		else if (type == std::string("Ponderation distance points"))
		{
			double ponderation;
			iss >> ponderation;
			PointsDistancePonderation(ponderation);
		}
		else if (type == std::string("Maximum distance points"))
		{
			double ponderation;
			iss >> ponderation;
			PointsDistanceMax(ponderation);
		}
		else if (type == std::string("Energie d'existence d'un objet"))
		{
			double energy;
			iss >> energy;
			IndividualEnergy(energy);
		}
		else
			std::cout << "Tag " << type << " de valeur " << iss.str() << " non reconnu." << std::endl;
	}
	ComputeProb();
}

BuildingsDetectorParametersSingleton::BuildingsDetectorParametersSingleton() :
	m_initialTemperature(5000.),
	m_nbIterations(15000000),
	m_nbIterationsDump(10000),
	m_decreaseCoefficient(0.999999),
	m_probaBirth(1. / 10.),
	m_probaDeath(1. / 10.),
//	m_inputDataFilePath("data/SaintMichel/MNS_StMichel_sans_veget.tif"),
//	m_inputImageWidth(1695),
//	m_inputImageHeight(1575),
	m_inputDataFilePath("/home/dboldo/noyau/ITKViewer/other/rjmcmc_buildings/data/Marseille/Crop_MNE_Marseille.tif"),
//	m_inputDataFilePath("D:\\Codes\\ITKViewer\\other\\rjmcmc_buildings\\data\\Marseille\\UnBloc_MNE_Marseille.tif"),
	m_inputImageWidth(1000),
	m_inputImageHeight(1000),
	m_varianceGaussianFilter(2.0),
	m_rectangleMinimalSize(5.),
	m_rectangleMaximalRatio(5.),
	m_ponderationSurfaceIntersection(10.),
	m_pointsDistancePonderation(10.),
	m_pointsDistanceMax(10.),
	m_individualEnergy(500.)
			{}

void BuildingsDetectorParametersSingleton::ComputeProb()
{
	m_probas.clear();
	m_probas.push_back(m_probaBirth);
	m_probas.push_back(m_probas.back() + m_probaDeath);
	/// La probabilite de modification est le reste a 1.
	m_probas.push_back(1.);
}
