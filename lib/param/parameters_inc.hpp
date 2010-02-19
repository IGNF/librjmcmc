#ifndef PARAMETERS_INC_HPP
#define PARAMETERS_INC_HPP
#include <fstream>
#include <iostream>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "param/parameters.hpp"

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

struct value_updater {
	typedef void result_type;
	template<typename T> void operator()(T& t) const { t.update_value(); }
};

template<typename T>
void parameters<T>::update_values() {
	for(iterator it = begin(); it!=end(); ++it)
		boost::apply_visitor(value_updater(),*it);
}

#endif // PARAMETERS_INC_HPP


