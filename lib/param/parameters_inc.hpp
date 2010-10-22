#ifndef PARAMETERS_INC_HPP
#define PARAMETERS_INC_HPP
#include <fstream>
#include <iostream>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "param/parameter.hpp"

template<typename T>
struct option_adder {
    po::options_description& m_desc;
    const T& m_p;

    option_adder(po::options_description& desc, const T& p) : m_desc(desc), m_p(p) {}
    typedef void result_type;
    template<typename V> void operator()(V& t) const {
        std::string name(m_p.name());
        char s = m_p.shortcut();
        if(s!='\0') name = (name + ",")+ s;
        m_desc.add_options()(
                name.c_str(),
                po::value<V>(&t)->default_value(t),
                m_p.description().c_str()
		);
    }
};

template<typename T>
void init_description(po::options_description& desc, parameters<T>& param) {
    desc.add_options()
            ("help,h","Help message...")
            ("config,c",po::value<std::string>(), "Configuration file");

    typedef typename parameters<T>::iterator iterator;
    for(iterator it=param.begin(); it!=param.end();++it)
        boost::apply_visitor(option_adder<T>(desc,*it),it->value());
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
    iterator it = find(name);
    if (it==end()) throw unknown_parameter_name();
    m_parameter.erase(it);
}


template<typename T> template<typename V>
void parameters<T>::insert(const std::string& name, char c, const V& v, const std::string& desc) {
    m_parameter.push_back(T(name,c,v,desc));
}

template<typename T> template<typename V>
void parameters<T>::insert(const std::string& name) {
    m_parameter.push_back(T(name,'\0',V(),""));
}

#endif // PARAMETERS_INC_HPP


