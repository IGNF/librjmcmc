#include <iostream>
#include <strstream>

#include "GILDataEnergyComputer.hpp"

#include "BuildingsDetectorParameters.hpp"
#include "GeometricNode.hpp"
#include "RJMCMC_EnergeticContainer.hpp"
#include "RJMCMC_Sampler.hpp"

#include "VariantNode.hpp"
typedef boost::variant<Rectangle_2, Cercle_2> VariantType;
typedef RJMCMC_EnergeticContainer< VariantNode< VariantType >, VariantIntersectionPriorEnergy<VariantType>, VariantGradientDataEnergy<VariantType> >	BuildingsDetector;
typedef VariantImageExporter<VariantType> ImageExporterType;

//typedef GeometricNode<Rectangle_2> MyNode;
//typedef GeometricNode<Cercle_2> MyNode;
//typedef RJMCMC_Detector<MyNode, IntersectionPriorEnergyPolicy, GILEnergyPolicy >	BuildingsDetector;
//typedef ImageExporter ImageExporterType;

void generate_test_images();
void res_evaluator(const char * nom_in, const char * nom_ref, const char * nom_mask);

int main (int argc, char **argv)
{		
	if (!BuildingsDetectorParametersSingleton::Instance()->ParseCmdLine(argc, argv))
	{
		generate_test_images();
		return 0;
	}
	
	//std::cout << BuildingsDetectorParametersSingleton::Instance()->InitialTemperature() << std::endl;
	//std::cout << BuildingsDetectorParametersSingleton::Instance()->InputDataFilePath() << std::endl;
	//std::cout << BuildingsDetectorParametersSingleton::Instance()->IndividualEnergy() << std::endl;
	
	BBox::PointType size , origin;
	size[0] = BuildingsDetectorParametersSingleton::Instance()->RunningWidth()-1;
	size[1] = BuildingsDetectorParametersSingleton::Instance()->RunningHeight()-1;
	origin[0] = BuildingsDetectorParametersSingleton::Instance()->RunningOriginX();
	origin[1] = BuildingsDetectorParametersSingleton::Instance()->RunningOriginY();
	shared_ptr< VariantGradientDataEnergy<VariantType> > data(new VariantGradientDataEnergy<VariantType>(
		BuildingsDetectorParametersSingleton::Instance()->IndividualEnergy(), 
		BuildingsDetectorParametersSingleton::Instance()->CoefIndividualEnergy(), 
		BuildingsDetectorParametersSingleton::Instance()->InputImageFilePath(), 
		BuildingsDetectorParametersSingleton::Instance()->InputMaskFilePath()));
	shared_ptr< VariantIntersectionPriorEnergy<VariantType> > prior(new VariantIntersectionPriorEnergy<VariantType>(
		BuildingsDetectorParametersSingleton::Instance()->IntersectionSurfacePonderation()));
	BuildingsDetector buildingsDetector(prior, data);
	Sampler< BuildingsDetector > sampler(BBox(size, origin), BuildingsDetectorParametersSingleton::Instance()->InitialTemperature() , BuildingsDetectorParametersSingleton::Instance()->DecreaseCoefficient(), BuildingsDetectorParametersSingleton::Instance()->CumulatedProbabilities() );

	// Formattage du log sous forme de tableau, ca facilite la creation de graphiques ...
	std::ostringstream my_out_stream;
	my_out_stream << "\tIte\t";
	my_out_stream << "NbObj\t";
	my_out_stream << "Time(s)\t";
	my_out_stream << "Temp\t";
	my_out_stream << "E_data\t";
	my_out_stream << "E_prior\t";
	my_out_stream << "E_total\t";
	std::cout << my_out_stream.str() << std::endl;

	clock_t clock_debut = clock(), clock_local = clock();
	for (unsigned int current_iter=0; current_iter <= BuildingsDetectorParametersSingleton::Instance()->NbIterations(); ++current_iter)
	{
		if (current_iter % BuildingsDetectorParametersSingleton::Instance()->NbIterationsDump() == 0)
		{
			my_out_stream.str("");
			my_out_stream << "\t" << current_iter;
			my_out_stream << "\t" << buildingsDetector.GetNbVertices();
			my_out_stream << "\t" << double(clock()- clock_local) / CLOCKS_PER_SEC;
			my_out_stream << "\t" << sampler.Temperature();
			my_out_stream << "\t" << buildingsDetector.DataEnergy();
			my_out_stream << "\t" << buildingsDetector.PriorEnergy();
			my_out_stream << "\t" << buildingsDetector.DataEnergy() + buildingsDetector.PriorEnergy() << std::endl;
			std::cout << my_out_stream.str();
			clock_local = clock();
			ImageExporterType exporter;
			exporter.InitExport(BuildingsDetectorParametersSingleton::Instance()->InputImageFilePath().c_str());
			BuildingsDetector::vertex_iterator it_v = vertices(buildingsDetector.GetGraph()).first, fin_v = vertices(buildingsDetector.GetGraph()).second;
			for (; it_v != fin_v; ++it_v)
				exporter.ExportNode(buildingsDetector.GetGraph()[*it_v].Geometry());
			std::ostringstream oss;
			oss << BuildingsDetectorParametersSingleton::Instance()->OutputFilePath() << current_iter << ".tif";
			exporter.EndExport(oss.str().c_str());
		}
		sampler.Itere(buildingsDetector);
	}
	clock_t clock_fin = clock();

	my_out_stream.str("");
	my_out_stream << "Iterations finished" << std::endl;
	double elapsed = double(clock_fin - clock_debut) / CLOCKS_PER_SEC;
	my_out_stream << "Total elapsed time (s) :  " << elapsed << std::endl;
	my_out_stream << "Mean time per iteration (ms) :  " << elapsed * 1000 / BuildingsDetectorParametersSingleton::Instance()->NbIterations() << std::endl;
	my_out_stream << "\t" << buildingsDetector.GetNbVertices();
	my_out_stream << "\t" << sampler.Temperature();
	my_out_stream << "\t" << buildingsDetector.DataEnergy();
	my_out_stream << "\t" << buildingsDetector.PriorEnergy();
	my_out_stream << "\t" << buildingsDetector.DataEnergy() + buildingsDetector.PriorEnergy() << std::endl;
	my_out_stream << "Graph Data energy integrity : " << buildingsDetector.CheckGraphDataEnergyIntegrity() << std::endl;
	my_out_stream << "Graph Prior energy integrity: " << buildingsDetector.CheckGraphPriorEnergyIntegrity() << std::endl;
	my_out_stream << "Graph Structure integrity : " << buildingsDetector.CheckGraphStructureIntegrity() << std::endl;
	my_out_stream << "Total Surface integrity : " << buildingsDetector.CheckTotalSurfaceIntegrity() << std::endl;
	buildingsDetector.Dump(my_out_stream);
	std::cout << my_out_stream.str();

	ImageExporterType exporter_rec(true), exporter_cir(true);
	exporter_rec.InitExport(1695, 1575);
	exporter_cir.InitExport(1695, 1575);
	BuildingsDetector::vertex_iterator it_v = vertices(buildingsDetector.GetGraph()).first, fin_v = vertices(buildingsDetector.GetGraph()).second;
	for (; it_v != fin_v; ++it_v)
	{
		VariantType v = buildingsDetector.GetGraph()[*it_v].Geometry();
		if (v.which() == 0)
		{
			exporter_rec.ExportNode(v);
		}
		else
			exporter_cir.ExportNode(v);
	}
	exporter_rec.EndExport("fill_rec.tif");
	exporter_cir.EndExport("fill_cir.tif");

	std::cout << "Evaluation des rectangles" << std::endl;
	res_evaluator("fill_rec.tif", "data/MNS-veget/cadastre_saisie_50cm_complete_img.tif", "data/MNS-veget/cadastre_ref_20cm_complete_img_masque.tif");
	std::cout << "Evaluation des cercles" << std::endl;
	res_evaluator("fill_cir.tif", "data/MNS-veget/vegetation_saint_michel.tif", "data/MNS-veget/cadastre_ref_20cm_complete_img_masque.tif");
	return 0;
}
