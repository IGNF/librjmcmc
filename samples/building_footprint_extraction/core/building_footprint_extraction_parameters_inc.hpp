#ifndef __BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_INC_HPP__
#define __BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_INC_HPP__
#include <fstream>
#include <iostream>

#include "building_footprint_extraction_parameters.hpp"
namespace po = boost::program_options;

template<typename T, typename V>
parameter<T,V>::parameter(const std::string& n, char c, const V& v, const std::string& d)
		: m_name(n), m_shortcut(c), m_value(v), m_description(d), m_control(NULL) {}

struct option_adder {
	po::options_description& m_desc;
	option_adder(po::options_description& desc) : m_desc(desc) {}
	typedef void result_type;
	template<typename T,typename V> void operator()(parameter<T,V>& p) const {
		std::string name(p.name());
		char s = p.shortcut();
		if(s!='\0') name = (name + ",")+ s;
		m_desc.add_options()(
			name.c_str(),
			po::value<V>(&(p.value()))->default_value(p.value()),
			p.description().c_str()
		);
	}
};

template<typename T>
void init_description(po::options_description& desc, parameters<T>& param) {
	desc.add_options()
		("help,h","Message d'aide...")
		("config,c",po::value<std::string>(), "Fichier de configuration");

	typedef typename parameters<T>::iterator iterator;
	option_adder adder(desc);
	for(iterator it=param.begin(); it!=param.end();++it)
		boost::apply_visitor(adder,*it);
}

template<typename T>
bool parameters<T>::parse(const std::string& filename)
{
	po::options_description desc(m_caption);
	init_description(desc,*this);
	po::variables_map vm;
	std::ifstream file(filename.c_str());
	if(!file) 
	{
		std::cout << "Unable to read : "<<filename << std::endl;
		return false;
	}
	po::store(po::parse_config_file(file,desc), vm);

	if ( vm.count("config") && !parse(vm["config"].as<std::string> ()))
		return false;
	po::notify(vm);
	return true;
}

template<typename T>
bool parameters<T>::parse(int argc, char **argv)
{
	po::options_description desc(m_caption);
	init_description(desc,*this);
	po::variables_map vm;
	po::store(po::parse_command_line(argc,argv,desc), vm);
	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		return false;
	}

	if ( vm.count("config") && !parse(vm["config"].as<std::string> ()))
		return false;
	po::notify(vm);
	return true;
}

template<typename T>
void parameters<T>::erase(const std::string& name) {
	typename std::map<std::string,iterator>::iterator it = m_index.find(name);
	if(it==m_index.end()) return;
	m_parameter.erase(it->second);
	m_index.erase(it);
}


template<typename T> template<typename V>
void parameters<T>::insert(const std::string& name, char c, const V& v, const std::string& desc) {
	m_parameter.push_back(parameter<T,V>(name,c,v,desc));
	iterator it=end();
	m_index[name]=--it;
}

template<typename T>
void parameters<T>::update_values() {
	for(iterator it = begin(); it!=end(); ++it)
		boost::apply_visitor(value_updater(),*it);
}


template<typename T>
parameters<T>::parameters()
{
	insert<double>("temp",'t',150,"Temperature initiale");
	insert<int>("nbiter",'I',15000000,"Nombre d'iterations");
	insert<int>("nbdump",'d',10000,"Nombre d'iterations entre chaque affichage");
	insert<int>("nbsave",'S',10000,"Nombre d'iterations entre chaque sauvegarde");
	insert<bool>("dosave",'b',false, "Sauvegarde des resultats intermediaires");
	insert<bool>("test",'b',true, "test");
	insert<double>("deccoef",'C',0.999999,"Coefficient de decroissance");
	insert<double>("pbirth",'B',0.1, "Probabilite de naissance");
	insert<double>("pdeath",'D',0.1, "Probabilite de mort");
	insert<boost::filesystem::path>("dem",'i',"../data/ZTerrain_c3.tif", "Image DEM");
	insert<boost::filesystem::path>("ndvi",'v',"../data/ZTerrain_c3.tif", "Image NDVI");
	insert<int>("xmin",'x',0, "Zone a traiter (xmin)");
	insert<int>("ymin",'y',0, "Zone a traiter (ymin)");
	insert<int>("xmax",'X',652, "Zone a traiter (xmax)");
	insert<int>("ymax",'Y',662, "Zone a traiter (ymax)");
	insert<int>("subsampling",'u',1, "Sous-échantillonnage");
	insert<double>("gaussian",'g',2, "Variance du filtre gaussien en entree");
	insert<double>("sigmaD",'G',1, "Taille du noyau de flou pour le calcul des gradients");
	insert<double>("minsize",'m',5, "Taille minimale d'un rectangle");
	insert<double>("maxratio",'M',5, "Rapport longueur/largeur maximal d'un rectangle");
	insert<double>("surface",'s',10, "Ponderation de la surface d'intersection");
	insert<double>("energy",'e',250, "Energie d'existence d'un objet");
	insert<double>("poisson",'p',200, "Parametre du processus de Poisson");
}

#endif // __BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_INC_HPP__


