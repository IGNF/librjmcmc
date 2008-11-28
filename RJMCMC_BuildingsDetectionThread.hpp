#ifndef USE_IHM
#error DEFINE USE_IHM TO 1 OR 0
#endif // #ifndef USE_IHM

#include <iostream>

#ifndef WIN32
#include <tr1/array>
#include <tr1/tuple>
#else // WIN32
#include <boost/tr1/array.hpp>
#include <boost/tr1/tuple.hpp>
#endif // WIN32

#include "BuildingsDetectorParameters.hpp"
#if USE_IHM
#include "RJMCMC_BuildingsDetectionFrame.hpp"
#endif //#if USE_IHM

//#include "Simple2DNode.hpp"
#include "RectangleNode.hpp"

#include "RJMCMC_Detector.hpp"
#include "RJMCMC_Sampler.hpp"

//typedef RJMCMC_Detector<RectangleNode, RectanglePriorEnergyPolicy, SurfaceRectangleDataEnergyPolicy >	BuildingsDetector;
//typedef RJMCMC_Detector<RectangleNode, RectanglePriorEnergyPolicy, ImageRectangleDataEnergyPolicy >	BuildingsDetector;
//typedef RJMCMC_Detector<RectangleNode, RectanglePriorEnergyPolicy, ImageGradientEnergyPolicy >	BuildingsDetector;
typedef RJMCMC_Detector<RectangleNode, RectanglePointsPriorEnergyPolicy, ImageGradientEnergyPolicy >	BuildingsDetector;



void* RJMCMC_BuildingsDetectionThread::Entry()
{
	BBox::PointType size , origin;
	size[0] = BuildingsDetectorParametersSingleton::Instance()->RunningWidth()-1;
	size[1] = BuildingsDetectorParametersSingleton::Instance()->RunningHeight()-1;
	origin[0] = BuildingsDetectorParametersSingleton::Instance()->RunningOriginX();
	origin[1] = BuildingsDetectorParametersSingleton::Instance()->RunningOriginY();
	BuildingsDetector buildingsDetector(BBox(size, origin));
	Sampler< BuildingsDetector > sampler( BuildingsDetectorParametersSingleton::Instance()->InitialTemperature() , BuildingsDetectorParametersSingleton::Instance()->DecreaseCoefficient(), BuildingsDetectorParametersSingleton::Instance()->CumulatedProbabilities() );

	unsigned int nb_accepted[4];
	for (unsigned int i=0; i<4; ++i)
		nb_accepted[i] = 0;
	clock_t clock_debut = clock();
	clock_t clock_local = clock_debut;

	unsigned int nb_ite = BuildingsDetectorParametersSingleton::Instance()->NbIterations();
	unsigned int nb_dump = BuildingsDetectorParametersSingleton::Instance()->NbIterationsDump();

	for (unsigned int i=0; i<=nb_ite; ++i)
	{
		if (i % nb_dump == 0)
		{
			std::ostringstream my_out_stream;
			my_out_stream << "Iteration : " << i << "\n";
			my_out_stream << "Rates (%) proposition \t acceptation : \n";
			my_out_stream << "Birth : " << BuildingsDetectorParametersSingleton::Instance()->ProbaBirth()*100 << "\t" << 100.* nb_accepted[0] / double(BuildingsDetectorParametersSingleton::Instance()->NbIterationsDump()) << "\n";
			my_out_stream << "Death : " << BuildingsDetectorParametersSingleton::Instance()->ProbaDeath()*100 << "\t" << 100.* nb_accepted[1] / double(BuildingsDetectorParametersSingleton::Instance()->NbIterationsDump()) << "\n";
			my_out_stream << "Modif : " << BuildingsDetectorParametersSingleton::Instance()->ProbaModification()*100 << "\t" << 100.* nb_accepted[2] / double(BuildingsDetectorParametersSingleton::Instance()->NbIterationsDump()) << "\n";
/*
			unsigned int sum_accepted = 0;
			for (unsigned int i=0;i<3;++i)
				sum_accepted+=nb_accepted[i];

			if (sum_accepted > 0)
			{
					float rate_accepted[3];
					for (unsigned int i=0;i<3;++i)
						rate_accepted[i] = nb_accepted[i] * 1. / sum_accepted;
					bool modif = false;
					if (rate_accepted[0] < 10 * BuildingsDetectorParametersSingleton::Instance()->ProbaBirth())
					{
						BuildingsDetectorParametersSingleton::Instance()->ProbaBirth(BuildingsDetectorParametersSingleton::Instance()->ProbaBirth() / 2.);
						modif = true;
					}
					if (rate_accepted[1] < 10 * BuildingsDetectorParametersSingleton::Instance()->ProbaDeath())
					{
						BuildingsDetectorParametersSingleton::Instance()->ProbaDeath(BuildingsDetectorParametersSingleton::Instance()->ProbaDeath() / 2.);
						modif = true;
					}

					if (modif)
						sampler.CumulatedProbabilities(BuildingsDetectorParametersSingleton::Instance()->CumulatedProbabilities());
			}
*/
			for (unsigned int i=0; i<4; ++i)
				nb_accepted[i] = 0;
			clock_t clock_temp = clock();
			my_out_stream << "Time per iteration (ms) :  "<< double(clock_temp - clock_local)*1000./ (1.*CLOCKS_PER_SEC*BuildingsDetectorParametersSingleton::Instance()->NbIterationsDump()) << "\n";
			clock_local = clock_temp;

			my_out_stream << "Temperature : " << sampler.Temperature() << "\n";
			buildingsDetector.Dump(my_out_stream);

#if USE_IHM
			wxMutexGuiEnter();
			m_layer->Clear();
			BuildingsDetector::vertex_iterator it = vertices( buildingsDetector.GetGraph() ).first, fin = vertices( buildingsDetector.GetGraph() ).second;
			for (; it != fin; ++it)
			{
				RectangleNode n = buildingsDetector.GetGraph()[*it];
				std::vector<double> vectx, vecty;
				for (unsigned int i=0; i<5;++i)
				{
					vectx.push_back(CGAL::to_double(n.Rect()[i].x()));
					vecty.push_back(CGAL::to_double(n.Rect()[i].y()));
				}
				m_layer->AddPolygon(vectx, vecty );
				std::ostringstream oss;
				oss << n.Weight();
				m_layer->AddText( CGAL::to_double(n.Rect().center().x()) , CGAL::to_double(n.Rect().center().y()) , oss.str() , wxColour(255,0,0) );
			}
			m_frame->Refresh();
			wxLogMessage( wxString( my_out_stream.str().c_str(),*wxConvCurrent ).GetData());
			wxMutexGuiLeave();
#else // #if USE_IHM
			std::cout << my_out_stream.str();
#endif  // #if USE_IHM
		}

		eProposition prop =sampler.Itere(buildingsDetector);
		nb_accepted[prop]++;
	}
	clock_t clock_fin = clock();
	std::ostringstream my_out_stream;
	my_out_stream << "Iterations finished" << std::endl;
	my_out_stream << "Total elapsed time (s) :  " << double(clock_fin - clock_debut) / CLOCKS_PER_SEC << std::endl;
	my_out_stream << "Graph Data energy integrity : " << buildingsDetector.CheckGraphDataEnergyIntegrity() << std::endl;
	my_out_stream << "Graph Prior energy integrity: " << buildingsDetector.CheckGraphPriorEnergyIntegrity() << std::endl;
	my_out_stream << "Graph Structure integrity : " << buildingsDetector.CheckGraphStructureIntegrity() << std::endl;
	buildingsDetector.Dump(my_out_stream, true);
#if USE_IHM
	wxMutexGuiEnter();
	wxLogMessage( wxString( my_out_stream.str().c_str(),*wxConvCurrent ).GetData());
	m_frame->OnThreadEnd();
	wxMutexGuiLeave();
#else // #if USE_IHM
	std::cout << my_out_stream.str();
#endif  // #if USE_IHM

	return 00;
}
