#ifndef __BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_HPP__
#define __BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_HPP__

#include <cstdlib>
#include <list>
#include <sstream>
#include <boost/program_options.hpp>
#include <boost/variant.hpp>
#include <boost/filesystem/path.hpp>

#include "rjmcmc/pattern_singleton.hpp"

/**
 * @brief Class for specific "Building footprint extraction" application parameters handling
 * @author O. Tournaire
 */
struct parameter {
public:
	typedef boost::variant<boost::filesystem::path,std::string,int,double,bool> value_type;
	template<typename T> inline const T& get() const { return boost::get<T >( m_value); }
	template<typename T> inline T& get() { 
		/* this may be used to debug bad_get exceptions...
		T t;
		std::cout << typeid(t).name() << "=="<<m_value.type().name() << std::endl;
		*/
		return boost::get<T>(m_value); }
	template<typename T> inline void set(const T& t) { get<T>()=t; }
	inline const value_type& value() const { return m_value; }
	inline value_type& value() { return m_value; }
	inline const std::string& name() const { return m_name; }
	inline const std::string& description() const { return m_description; }
	inline char shortcut() const { return m_shortcut; }
	template<typename T> inline T* control() const { return static_cast<T*>(m_control); }
	template<typename T> inline void control(T *t) { m_control = static_cast<void*>(t); }
	template<typename T> parameter(const std::string& n, char c, const T&v, const std::string& d)
		: m_name(n), m_shortcut(c), m_value(v), m_description(d), m_control(NULL) {}

	std::string string() const {
		std::ostringstream oss;
		oss << m_value;
		return oss.str();
	}
private:
	std::string m_name;
	char m_shortcut;
	value_type m_value;
	std::string m_description;
	void *m_control;
};

class building_footprint_extraction_parameters: public PatternSingleton<building_footprint_extraction_parameters>
{
public:
	typedef std::list<parameter> container;
	typedef container::iterator iterator;
	typedef container::const_iterator const_iterator;

	friend class PatternSingleton<building_footprint_extraction_parameters>;
	inline iterator       param(const std::string& s)       { return m_index.find(s)->second; }
	inline const_iterator param(const std::string& s) const { return m_index.find(s)->second; }

	template<typename T> inline const T& get(const std::string& s) const { 
		return param(s)->get<T>();
	}
	template<typename T> inline void get(const std::string& s, T& t) const { 
		t = param(s)->get<T>();
	}
	template<typename T> inline void set(const std::string& s, const T& t) { 
		param(s)->set<T>(t);
	}

	bool parse(const std::string& filename);
	bool parse(int argc, char **argv);

	inline iterator begin() { return m_parameter.begin(); }
	inline iterator end() { return m_parameter.end(); }
	inline const_iterator begin() const { return m_parameter.begin(); }
	inline const_iterator end() const { return m_parameter.end(); }


	template<typename T>
	void insert(const std::string& name, char c, const T& t, const std::string& desc) {
		m_parameter.push_back(parameter(name,c,t,desc));
		iterator it=end();
		m_index[name]=--it;
	}
	void erase(const std::string& name);

private:
	std::map<std::string,iterator> m_index;
	std::string m_caption;
	container m_parameter;
	building_footprint_extraction_parameters();
	building_footprint_extraction_parameters(const building_footprint_extraction_parameters &);
};

#endif /* __BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_HPP__ */
