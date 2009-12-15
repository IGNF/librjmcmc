#include "core/building_footprint_extraction_parameters.hpp"
#include "rjmcmc_building_footprint_extraction_thread.hpp"

#include "core/rjmcmc_detector.hpp"
#include "core/rjmcmc_sampler.hpp"

#include "core/rectangle_node.hpp"

typedef building_footprint_extraction_parameters param;

typedef image_gradient_unary_energy building_unary_energy;
typedef intersection_area_binary_energy  building_binary_energy;
typedef trivial_accelerator building_accelerator;
typedef BoxIsValid building_is_valid;
typedef graph_configuration<Rectangle_2, building_unary_energy, building_binary_energy, building_is_valid, building_accelerator > building_configuration;

#define KERNELS uniform_birth_death_kernel,modification_kernel
typedef boost::tuple<KERNELS> kernels;
typedef rjmcmc_sampler<IF_VARIADIC(KERNELS,kernels)> building_sampler;

using namespace std;

#if USE_GUI
Layer::ptrLayerType& operator<<(Layer::ptrLayerType& layer, const building_configuration& config) {
	layer->Clear();
	building_configuration::const_iterator it = config.begin(), end = config.end();
	for (; it != end; ++it)
	{
		building_configuration::value_type r = config[it];
		std::vector<double> vectx, vecty;
		for (unsigned int i=0; i<5;++i)
		{
			vectx.push_back(CGAL::to_double(r[i].x()));
			vecty.push_back(CGAL::to_double(r[i].y()));
		}
		layer->AddPolygon(vectx, vecty );
		std::ostringstream oss;
		oss << config.energy(it);
		layer->AddText( CGAL::to_double(r.center().x()) , CGAL::to_double(r.center().y()) , oss.str() , wxColour(255,0,0) );
	}
	return layer;
}
#endif // USE_GUI


void* rjmcmc_building_footprint_extraction_thread::Entry()
{
	geometric_temperature temperature(
		param::Instance()->InitialTemperature(),
		param::Instance()->DecreaseCoefficient()
	);

	uniform_birth_death_kernel birthdeath(
		param::Instance()->BirthProbability(),
		param::Instance()->DeathProbability()
	);
	modification_kernel modif;

	building_sampler sampler( IF_VARIADIC(,boost::make_tuple)(birthdeath,modif) );

	bbox_2::point_type pmin, pmax;
	pmin[0] = param::Instance()->RunningMinX();
	pmin[1] = param::Instance()->RunningMinY();
	pmax[0] = param::Instance()->RunningMaxX();
	pmax[1] = param::Instance()->RunningMaxY();
	bbox_2 bbox(pmin, pmax);


	building_unary_energy unary_energy(
		param::Instance()->IndividualEnergy(),
		param::Instance()->InputDataFilePath(),
		bbox,
		param::Instance()->SigmaD(),
		param::Instance()->SubSampling()
	);
	building_binary_energy binary_energy(
		param::Instance()->IntersectionSurfacePonderation()
	);
	building_is_valid is_valid(bbox,
		param::Instance()->RectangleMinimalSize(),
		param::Instance()->RectangleMaximalRatio()
	);

	building_accelerator accelerator;
	building_configuration configuration(unary_energy, binary_energy, is_valid, accelerator);

	unsigned int nb_proposed[building_sampler::size];
	unsigned int nb_accepted[building_sampler::size];
	for (unsigned int i=0; i<building_sampler::size; ++i) nb_accepted[i] = nb_proposed[i] = 0;

	ostringstream out;
	out.fill(' ');
	int w = 10;
	out << std::setw(w)<<"Iteration"<< std::setw(w)<< "Objects";
	out << std::setw(w)<<"BirthP"<< std::setw(w)<<"BirthA";
	out << std::setw(w)<<"DeathP"<< std::setw(w)<<"DeathA";
	out << std::setw(w)<<"ModifP"<< std::setw(w)<<"ModifA";
	out << std::setw(w)<<"Accept"<< std::setw(w)<<"Time(ms)";
	out << std::setw(w)<<"Temp"<< std::setw(w)<<"E_data";
	out << std::setw(w)<<"E_priori"<< std::setw(w)<<"E_total";

	unsigned int nb_ite  = param::Instance()->NbIterations();
	unsigned int nb_dump = param::Instance()->NbIterationsDump();
	unsigned int nb_save = param::Instance()->NbIterationsSave();

#if USE_GUI
	wxMutexGuiEnter();
	wxLogMessage( wxString( out.str().c_str(),*wxConvCurrent ).GetData());
	wxMutexGuiLeave();
#else
	cout << out.str() << std::endl;
#endif // USE_GUI

	clock_t clock_debut = clock();
	clock_t clock_local = clock_debut;
	for (unsigned int i=0; i<=nb_ite; ++i)
	{
/*
		if ( i % nb_save == 0 && nb_save != 0 )
		{
#if USE_GUI
			std::ostringstream oss;
			oss.width(8);
			oss.fill('0');
			oss << i;
			m_layer->Save(oss.str() + ".shp");
#endif // USE_GUI
		}
		* */
		if (i % nb_dump == 0)
		{
			out.str("");
			out << i;
			out << std::setw(w) << configuration.size();
			
			unsigned int total_accepted =0;
			for(unsigned int i=0; i<building_sampler::size; ++i)
			{
				//out << std::setw(w) << 100.* nb_proposed[i] / nb_dump;
				//out << std::setw(w) << (nb_proposed[i]?(100.* nb_accepted[i]) / nb_proposed[i]:100.);
				total_accepted += nb_accepted[i];
				nb_accepted[i] = nb_proposed[i] = 0;
			}
			out << std::setw(w) << (100.*total_accepted) / nb_dump;
			clock_t clock_temp = clock();
			out << std::setw(w) << ((clock_temp - clock_local)*1000.)/ nb_dump;
			clock_local = clock_temp;
			out << std::setw(w) << *temperature;
			out << std::setw(w) << configuration.unary_energy();
			out << std::setw(w) << configuration.binary_energy();
			out << std::setw(w) << configuration.unary_energy() + configuration.binary_energy();
#if USE_GUI
			wxMutexGuiEnter();
			m_layer << configuration;
			m_frame->Refresh();
			wxLogMessage( wxString( out.str().c_str(),*wxConvCurrent ).GetData());
			wxMutexGuiLeave();
			if ( TestDestroy() )
			{
				wxLogMessage( _("Computation stopped") );
				break;
			}
#else // #if USE_GUI
			std::cout << out.str();
#endif  // #if USE_GUI
		}

		sampler(configuration,*temperature++);
		nb_proposed[sampler.kernel_id()]++;
		if( sampler.accepted() ) nb_accepted[sampler.kernel_id()]++;

	} // for i

	clock_t clock_fin = clock();
	out.str("");
	out << "Iterations finished" << std::endl;
	out << "Total elapsed time (s) :  " << double(clock_fin - clock_debut) / CLOCKS_PER_SEC << std::endl;
	out << "Graph Data energy integrity : " << configuration.audit_unary_energy() - configuration.unary_energy() << std::endl;
	out << "Graph Prior energy integrity: " << configuration.audit_binary_energy() - configuration.binary_energy()<< std::endl;
	out << "Graph Structure integrity : " << configuration.audit_structure() << std::endl;
	out << configuration;
#if USE_GUI
	wxMutexGuiEnter();
	wxLogMessage( wxString( out.str().c_str(),*wxConvCurrent ).GetData());
	m_frame->OnThreadEnd();
	wxMutexGuiLeave();
#else // #if USE_GUI
	cout << out.str();
#endif  // #if USE_GUI

	return 00;
}
