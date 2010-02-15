#ifndef __BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_HPP__
#define __BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_HPP__

#include <cstdlib>
#include <list>
#include <boost/variant.hpp>
#include <boost/filesystem/path.hpp>

#include "rjmcmc/pattern_singleton.hpp"

/**
 * @brief Class for application parameters handling
 * @author O. Tournaire
 */

template<typename T, typename V>
struct parameter {
public:
	typedef V value_type;
	typedef typename T::template control<V> control_trait;
	typedef typename control_trait::type control_type;

	inline void control(control_type *c)    { m_control = c; }
	inline void value  (const value_type& t) {
		m_value   = t;
		control_trait::set(m_control,t);
	}
	inline void update_value() { control_trait::get(m_control,m_value); }

	inline const value_type&        value() const { return m_value; }
	inline       value_type&        value()       { return m_value; }
	inline const std::string&        name() const { return m_name; }
	inline const std::string& description() const { return m_description; }
	inline       char            shortcut() const { return m_shortcut; }
	inline       control_type    control()        { return m_control; }

	parameter(const std::string& n, char c, const value_type& v, const std::string& d);

private:
	std::string m_name;
	char m_shortcut;
	value_type m_value;
	std::string m_description;
	control_type *m_control;
};


template<typename T>
class parameters
  : public PatternSingleton<parameters<T> >
{
public:
	typedef boost::variant<
		parameter<T,boost::filesystem::path>,
		parameter<T,std::string>,
		parameter<T,int>,
		parameter<T,double>,
		parameter<T,bool>
	> parameter_types;

	typedef std::list<parameter_types> container;
	typedef typename container::iterator iterator;
	typedef typename container::const_iterator const_iterator;

	friend class PatternSingleton<parameters>;
	inline iterator       param(const std::string& s)       { return m_index.find(s)->second; }
	inline const_iterator param(const std::string& s) const { return m_index.find(s)->second; }

	template<typename V> inline const V& get(const std::string& s) const { 
		return boost::get<parameter<T,V> >(*param(s)).value();
	}
	template<typename V> inline void get(const std::string& s, V& v) const { 
		v = boost::get<parameter<T,V> >(*param(s)).value();
	}
	template<typename V> inline void set(const std::string& s, const V& v) { 
		boost::get<parameter<T,V> >(*param(s)).value(v);
	}

	bool parse(const std::string& filename);
	bool parse(int argc, char **argv);

	inline iterator begin() { return m_parameter.begin(); }
	inline iterator end() { return m_parameter.end(); }
	inline const_iterator begin() const { return m_parameter.begin(); }
	inline const_iterator end() const { return m_parameter.end(); }

	template<typename V>
	void insert(const std::string& name, char c, const V& t, const std::string& desc);
	void erase(const std::string& name);
	void update_values();


	inline void caption(const std::string& c) { m_caption=c; }
	inline const std::string& caption() const { return m_caption; }
private:
	std::map<std::string,iterator> m_index;
	std::string m_caption;
	container m_parameter;
	parameters();
	parameters(const parameters &);
};

#endif /* __BUILDING_FOOTPRINT_EXTRACTION_PARAMETERS_HPP__ */
