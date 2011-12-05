#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP

#include <cstdlib>
#include <list>
#include <boost/filesystem/path.hpp>
#include <boost/variant.hpp>

/**
 * @brief Class for application parameters handling
 * @author O. Tournaire, M. Bredif
 */

struct parameter {
public:
    typedef boost::variant<boost::filesystem::path, std::string, int, double, bool> value_type;

    inline const std::string&        name() const { return m_name; }
    inline const std::string& description() const { return m_description; }
    inline const value_type& value() const { return m_value; }
    inline       value_type& value()       { return m_value; }
    inline       char            shortcut() const { return m_shortcut; }

    template<typename T> inline void set(const T& t) { m_value = t;  }
    template<typename T> inline void get(T& t) const { t = get<T>(); }
    template<typename T> inline void get(T& t)       { t = get<T>(); }
    template<typename T> inline const T& get() const { return boost::get<T>(m_value); }
    template<typename T> inline       T& get()       { return boost::get<T>(m_value); }

    template<typename T>
    parameter(const std::string& n, char c, const T& v, const std::string& d)
        : m_name(n), m_shortcut(c), m_value(v), m_description(d) {}

    private:
        std::string m_name;
        char m_shortcut;
        value_type m_value;
        std::string m_description;
    };

struct unknown_parameter_name{};

struct name_equals {
    std::string m_name;
    name_equals(const std::string& name) : m_name(name) {}
    template<typename T>
    inline bool operator()(const T& t) const { return m_name==t.name(); }
};

template<typename T>
class parameters
{
public:
    typedef T parameter_t;
    
    typedef std::list< T > parameter_list;
    typedef typename parameter_list::iterator iterator;
    typedef typename parameter_list::const_iterator const_iterator;

    inline iterator       find(const std::string& s)       { return std::find_if(m_parameter.begin(),m_parameter.end(),name_equals(s)); }
    inline const_iterator find(const std::string& s) const { return std::find_if(m_parameter.begin(),m_parameter.end(),name_equals(s)); }

    template<typename V> inline const V& get(const std::string& s) const { 
        const_iterator it = find(s);
        if (it==end()) throw unknown_parameter_name();
        return it->get<V>();
    }

    template<typename V> inline void get(const std::string& s, V& v) const {
        v = get<V>(s);
    }

    template<typename V> inline void set(const std::string& s, const V& v) { 
        iterator it = find(s);
        if (it==end()) throw unknown_parameter_name();
        it->set(v);
    }

    bool save (const std::string& filename) const;
    bool parse(const std::string& filename);
    bool parse(int argc, char **argv);
    
    inline iterator begin() { return m_parameter.begin(); }
    inline iterator end() { return m_parameter.end(); }
    inline const_iterator begin() const { return m_parameter.begin(); }
    inline const_iterator end() const { return m_parameter.end(); }

    template<typename V> void insert(const std::string& name, char c, const V& t, const std::string& desc);
    template<typename V> void insert(const std::string& name);

    void erase(const std::string& name);
    void clear() { m_parameter.clear(); }

    inline void caption(const std::string& c) { m_caption=c; }
    inline const std::string& caption() const { return m_caption; }

    parameters() {}

private:
    parameter_list m_parameter;
    std::string m_caption;
    parameters(const parameters &);
};


#include <fstream>
#include <iostream>
#include <limits>

template<typename T>
bool parameters<T>::save(const std::string& filename) const
{
    std::ofstream file(filename.c_str());
    if(!file)
    {
        std::cout << "Unable to open : "<<filename << std::endl;
        return false;
    }
    file.precision(std::numeric_limits<double>::digits10+1);
    for(const_iterator it=m_parameter.begin(); it!=m_parameter.end();++it)
    {
        file << it->name() << " = " << it->value() << std::endl;
    }
    return true;
}

#endif // PARAMETERS_HPP
