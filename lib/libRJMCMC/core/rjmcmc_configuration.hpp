#ifndef __RJMCMC_CONFIGURATION_HPP__
#define __RJMCMC_CONFIGURATION_HPP__

#include <boost/variant.hpp>

// rjmcmc::apply_visitor generalizes boost::apply_visitor to directly call the function object if the argument is not a variant.

namespace rjmcmc {

template<typename Visitor, typename Variant> 
  inline typename Visitor::result_type apply_visitor(const Visitor &v, const Variant &v1) { return v(v1); }

template<typename Visitor, typename Variant> 
  inline typename Visitor::result_type apply_visitor(const Visitor &v, Variant &v1) { return v(v1); }

template<typename Visitor, typename Variant1, typename Variant2> 
  inline  typename Visitor::result_type apply_visitor(const Visitor &v, const Variant1 &v1, const Variant2 &v2) { return v(v1,v2); }

template<typename Visitor, typename Variant1, typename Variant2> 
  inline  typename Visitor::result_type apply_visitor(const Visitor &v, const Variant1 &v1, Variant2 &v2) { return v(v1,v2); }

template<typename Visitor, BOOST_VARIANT_ENUM_PARAMS(typename T) > 
inline typename Visitor::result_type apply_visitor(const Visitor &v,
	const boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> &v1)
{
	return boost::apply_visitor(v,v1);
}

template<typename Visitor, BOOST_VARIANT_ENUM_PARAMS(typename T) > 
inline typename Visitor::result_type apply_visitor(const Visitor &v,
	boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> &v1)
{
	return boost::apply_visitor(v,v1);
}

template<typename Visitor,
	BOOST_VARIANT_ENUM_PARAMS(typename T),
	BOOST_VARIANT_ENUM_PARAMS(typename U) > 
inline typename Visitor::result_type apply_visitor(const Visitor &v,
	const boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> &v1,
	const boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)> &v2)
{
	return boost::apply_visitor(v,v1,v2);
}


template<typename Visitor,
	BOOST_VARIANT_ENUM_PARAMS(typename T),
	BOOST_VARIANT_ENUM_PARAMS(typename U) > 
inline typename Visitor::result_type apply_visitor(const Visitor &v,
	const boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> &v1,
	boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)> &v2)
{
	return boost::apply_visitor(v,v1,v2);
}

//////////////////////////////////////////////////////////

struct trivial_accelerator {
	template<typename C, typename T> std::pair<typename C::iterator,typename C::iterator> operator()(const C &c, const T &t) const {
		return std::make_pair(c.begin(),c.end());
	}
};

//////////////////////////////////////////////////////////
namespace internal {

template<class Configuration>
class modification
{
	typedef typename Configuration::value_type	value_type;
	typedef typename Configuration::iterator	iterator;
	std::vector<value_type>	m_birth;
	std::vector<iterator>	m_death;
public:
	typedef typename std::vector<value_type>::const_iterator	birth_const_iterator;
	typedef typename std::vector<iterator>::const_iterator	death_const_iterator;

	inline birth_const_iterator birth_begin() const { return m_birth.begin(); }
	inline birth_const_iterator birth_end  () const { return m_birth.end  (); }
	inline death_const_iterator death_begin() const { return m_death.begin(); }
	inline death_const_iterator death_end  () const { return m_death.end  (); }
	template<typename T>
	inline void insert_birth(const T& b) { m_birth.push_back(value_type(b)); }
	inline void insert_death(iterator d)  { m_death.push_back(d); }
	inline unsigned int birth_size() const { return m_birth.size(); }
	inline unsigned int death_size() const { return m_death.size(); }
	inline void clear() { m_birth.clear(); m_death.clear(); }


	template<typename V> class inserter {
		mutable V& m_v;
		public:
		inserter(V& v) : m_v(v) {}
		typedef void result_type;
		template<typename T> void operator()(const T& t) const { m_v.push_back(value_type(t)); }
		template<typename T, typename U> void operator()(const std::pair<T,U>& t) const {
			m_v.push_back(value_type(t.first));
			m_v.push_back(value_type(t.second));
		}
	};

	inline inserter<std::vector<value_type> > birth_inserter() {
		return inserter<std::vector<value_type> > (m_birth);
	}
	inline inserter<std::vector<iterator> > death_inserter() {
		return inserter<std::vector<iterator> >(m_death);
	}
};
}

//////////////////////////////////////////////////////////

template<typename T, typename UnaryEnergy, typename BinaryEnergy, typename IsValid, typename Accelerator=trivial_accelerator>
class vector_configuration
{
	typedef	std::vector<T> container;
	container	m_container;
	UnaryEnergy	m_unary_energy;
	BinaryEnergy	m_binary_energy;
	IsValid		m_is_valid;
	Accelerator	m_accelerator;
public:
	typedef T					value_type;
	typedef vector_configuration<T,UnaryEnergy, BinaryEnergy, IsValid, Accelerator> Self;
	typedef internal::modification<Self>	modification;
	typedef	typename container::const_iterator	const_iterator;
	typedef	typename container::iterator		iterator;

	vector_configuration(UnaryEnergy unary_energy, BinaryEnergy binary_energy, IsValid is_valid, Accelerator accelerator=Accelerator()) : m_unary_energy(unary_energy), m_binary_energy(binary_energy), m_is_valid(is_valid), m_accelerator(accelerator)
	{}

	const IsValid& is_valid() const { return m_is_valid; }
	bool is_valid(const T& t) const { return rjmcmc::apply_visitor(m_is_valid,t); }

	// energy
	double unary_energy() const
	{
		double e = 0.;
		for (const_iterator it = begin(); it != end(); ++it)
			e += rjmcmc::apply_visitor(m_unary_energy, *it );
		return e;
	}

	double binary_energy() const
	{
		double e = 0.;
		for (const_iterator i = begin(); i != end(); ++i)
			for (const_iterator j = i+1; j != end(); ++j)
				e += rjmcmc::apply_visitor(m_binary_energy, *i, *j );
		return e;
	}

	// objects
	inline size_t size() const { return m_container.size(); }
	inline bool empty() const {	return m_container.empty(); }
	inline iterator begin() { return m_container.begin(); }
	inline iterator end  () { return m_container.end(); }
	inline const_iterator begin() const { return m_container.begin(); }
	inline const_iterator end  () const { return m_container.end(); }
	inline const value_type& operator[]( const_iterator v ) const { return *v; }
	inline const value_type& value( const_iterator v ) const { return *v; }
	inline double energy( const_iterator v ) const { return rjmcmc::apply_visitor(m_unary_energy,*v); }

	// evaluators

	template <typename Modification> double delta_energy(const Modification &modif) const
	{
		return delta_birth(modif)+delta_death(modif);
	}

	template <typename Modification> double delta_birth(const Modification &modif) const
	{
		double delta = 0;
		typedef typename Modification::birth_const_iterator bci;
		typedef typename Modification::death_const_iterator dci;
		bci bbeg = modif.birth_begin();
		bci bend = modif.birth_end();
		dci dbeg = modif.death_begin();
		dci dend = modif.death_end();
		for(bci it=bbeg; it!=bend; ++it) {
			delta += rjmcmc::apply_visitor(m_unary_energy,*it);
			for (const_iterator it2=begin(); it2 != end(); ++it2)
				if (std::find(dbeg,dend,it2)==dend)
					delta += rjmcmc::apply_visitor(m_binary_energy, *it, value(it2) );
			for (bci it2=it+1; it2 != bend; ++it2)
				delta += rjmcmc::apply_visitor(m_binary_energy, *it, *it2);
		}
		return delta;
	}

	template <typename Modification> double delta_death(const Modification &modif) const
	{
		double delta = 0;
		typedef typename Modification::death_const_iterator dci;
		dci dbeg = modif.death_begin();
		dci dend = modif.death_end();
		for(dci it=dbeg; it!=dend; ++it) {
			iterator v = *it;
			delta -= energy(v);
			for (const_iterator it2=begin(); it2 != end(); ++it2)
				delta -= rjmcmc::apply_visitor(m_binary_energy, *it, value(it2) );
			for (dci it2=it+1; it2 != dend; ++it2)
				delta -= rjmcmc::apply_visitor(m_binary_energy, *it, *it2);
		}
		return delta;
	}

	// manipulators
	template <typename Modification> void apply(const Modification &modif)
	{
		typedef typename Modification::birth_const_iterator bci;
		typedef typename Modification::death_const_iterator dci;
		dci dbeg = modif.death_begin();
		dci dend = modif.death_end();
		for(dci dit=dbeg; dit!=dend; ++dit) remove(*dit);
		bci bbeg = modif.birth_begin();
		bci bend = modif.birth_end();
		for(bci bit=bbeg; bit!=bend; ++bit) insert(*bit);
	}

	void insert(const value_type &obj) { 
		m_container.push_back(obj);
	}
	void remove( iterator v ) {
		std::swap(*v,m_container.back());
		m_container.pop_back();
	}

	// audit
	inline double audit_unary_energy () const { return unary_energy();}
	inline double audit_binary_energy() const { return binary_energy();}
	inline unsigned int audit_structure() const { return 0; }
};


template<typename T, typename U, typename B, typename V, typename A>
std::ostream& operator<<(std::ostream& o, const vector_configuration<T,U,B,V,A>& c) {
	o << "energy     : " << c.unary_energy() + c.binary_energy();
	o << " = " << c.unary_energy() << " + " << c.binary_energy() << " (Data+Prior)\n";
	o << "Nb objects : " << c.size() << "\n";
	{
		typename vector_configuration<T,U,B,V,A>::const_iterator it = c.begin(), end = c.end();
		for (; it != end; ++it)
			o << *it <<"\t" << c.energy(it)<<"\t" << c[it] << std::endl;
	}
	
	return o;
}
}; // namespace rjmcmc

//////////////////////////////////////////////////////////

#include <boost/graph/adjacency_list.hpp>

namespace rjmcmc {

template<typename T, typename UnaryEnergy, typename BinaryEnergy, typename IsValid, typename Accelerator=trivial_accelerator, typename OutEdgeList = boost::listS, typename VertexList = boost::listS  >
class graph_configuration
{
public:
	typedef graph_configuration<T,UnaryEnergy, BinaryEnergy, IsValid, Accelerator, OutEdgeList, VertexList> Self;
	typedef internal::modification<Self>	modification;
	typedef T	value_type;
private:
	class edge {
	public:
		edge() : m_energy(0) {}
		inline double energy() const { return m_energy; }
		inline void energy(double e) { m_energy = e;    }

	private:
		double m_energy;
	};

	class node {
	public:
		node(const value_type& obj, double e) : m_value(obj), m_energy(e) { }
		inline const value_type& value() const { return m_value; }
		inline double energy() const { return m_energy; }

	private:
		value_type	m_value;
		double		m_energy;
	};
	typedef boost::adjacency_list<OutEdgeList, VertexList, boost::undirectedS, node, edge> graph_type;
	typedef typename graph_type::out_edge_iterator	out_edge_iterator;
	typedef	typename graph_type::vertex_descriptor	vertex_descriptor;
	typedef std::pair< typename graph_type::edge_descriptor , bool > edge_descriptor_bool;

public:
	typedef	typename graph_type::vertex_iterator	iterator;
	typedef	typename graph_type::vertex_iterator	const_iterator;
	typedef typename graph_type::edge_iterator	edge_iterator;
	typedef typename graph_type::edge_iterator	const_edge_iterator;
public:

	// configuration constructors/destructors
	graph_configuration(UnaryEnergy unary_energy, BinaryEnergy binary_energy, IsValid is_valid, Accelerator accelerator=Accelerator()) : m_unary(0.), m_binary(0.), m_unary_energy(unary_energy), m_binary_energy(binary_energy), m_is_valid(is_valid), m_accelerator(accelerator)
	{}
	~graph_configuration()
	{}

	// configuration accessors
	inline double unary_energy () const { return m_unary;}
	inline double binary_energy() const { return m_binary;}

	// values
	inline size_t size() const { return num_vertices(m_graph); }
	inline bool empty() const {	return (num_vertices(m_graph)==0); }
	inline iterator begin() { return vertices(m_graph).first; }
	inline iterator end  () { return vertices(m_graph).second; }
	inline const_iterator begin() const { return vertices(m_graph).first; }
	inline const_iterator end  () const { return vertices(m_graph).second; }
	inline const value_type& operator[]( const_iterator v ) const { return m_graph[ *v ].value(); }
	inline const value_type& value( const_iterator v ) const { return m_graph[ *v ].value(); }
	inline double energy( const_iterator v ) const { return m_graph[ *v ].energy(); }

	// interactions
	inline size_t size_of_interactions   () const { return num_edges(m_graph);    }
	inline edge_iterator interactions_begin() { return edges(m_graph).first; }
	inline edge_iterator interactions_end  () { return edges(m_graph).second; }
	inline const_edge_iterator interactions_begin() const { return edges(m_graph).first; }
	inline const_edge_iterator interactions_end  () const { return edges(m_graph).second; }
	inline double energy( edge_iterator e ) const { return m_graph[ *e ].energy(); }

	// evaluators

	template <typename Modification> double delta_energy(const Modification &modif) const
	{
		return delta_birth(modif)+delta_death(modif);
	}

	template <typename Modification> double delta_birth(const Modification &modif) const
	{
		double delta = 0;
		typedef typename Modification::birth_const_iterator bci;
		typedef typename Modification::death_const_iterator dci;
		bci bbeg = modif.birth_begin();
		bci bend = modif.birth_end();
		dci dbeg = modif.death_begin();
		dci dend = modif.death_end();
		for(bci it=bbeg; it!=bend; ++it) {
			delta += rjmcmc::apply_visitor(m_unary_energy,*it);
			for (const_iterator it2=begin(); it2 != end(); ++it2) // todo accelerator
				if (std::find(dbeg,dend,it2)==dend)
					delta += rjmcmc::apply_visitor(m_binary_energy, *it, value(it2) );
			for (bci it2=bbeg; it2 != it; ++it2)
				delta += rjmcmc::apply_visitor(m_binary_energy, *it, *it2);
		}
		return delta;
	}

	template <typename Modification> double delta_death(const Modification &modif) const
	{
		double delta = 0;
		typedef typename Modification::death_const_iterator dci;
		dci dbeg = modif.death_begin();
		dci dend = modif.death_end();
		for(dci it=dbeg; it!=dend; ++it) {
			iterator v = *it;
			delta -= energy(v);
			out_edge_iterator it2, end;
			for(boost::tie(it2,end) = out_edges( *v, m_graph ); it2!=end; ++it2) {
				vertex_descriptor dtarget = target(*it2, m_graph);
				bool found = false;
				for(dci it3=dbeg; it3!=it && !found; ++it3)
					found = (**it3 == dtarget);
				if (!found)
					delta -= m_graph[ *it2 ].energy();
			 }
		}
		return delta;
	}

	template <typename Modification> void apply(const Modification &modif)
	{
		typedef typename Modification::birth_const_iterator bci;
		typedef typename Modification::death_const_iterator dci;
		dci dbeg = modif.death_begin();
		dci dend = modif.death_end();
		for(dci dit=dbeg; dit!=dend; ++dit) remove(*dit);
		bci bbeg = modif.birth_begin();
		bci bend = modif.birth_end();
		for(bci bit=bbeg; bit!=bend; ++bit) insert(*bit);
	}

	// manipulators
	void insert(const value_type& obj)
	{
		node n(obj, rjmcmc::apply_visitor(m_unary_energy,obj));
		m_unary += n.energy();
		vertex_descriptor d = add_vertex(n, m_graph);
		iterator   it, end;
		
		for (boost::tie(it,end)=m_accelerator(*this,obj); it != end; ++it) {
			if ( *it == d ) continue;
			double e = rjmcmc::apply_visitor(m_binary_energy, obj, value(it) );
			if (   e == 0 ) continue;
			edge_descriptor_bool new_edge = add_edge(d, *it, m_graph );
			m_graph[ new_edge.first ].energy( e );
			m_binary += e;
		}
	}

	void remove( iterator v )
	{
		out_edge_iterator it, end;
		for(boost::tie(it,end) = out_edges( *v, m_graph ); it!=end; ++it)
			m_binary -= m_graph[ *it ].energy();
		m_unary -= m_graph[*v].energy();
		clear_vertex ( *v , m_graph);
		remove_vertex( *v , m_graph);
	}

	// audit
	double audit_unary_energy() const
	{
		double e = 0.;
		for (const_iterator i=begin(); i != end(); ++i)
			e += rjmcmc::apply_visitor(m_unary_energy, value(i) );
		return e;
	}

	double audit_binary_energy() const
	{
		double e = 0.;
		const_edge_iterator it, end;
		for(boost::tie(it,end) = edges( m_graph ); it!=end; ++it)
			e += rjmcmc::apply_visitor(m_binary_energy,	m_graph[source(*it,m_graph)].value() ,
						m_graph[target(*it,m_graph)].value() );
		return e;
	}

	unsigned int audit_structure() const
	{
		unsigned int err = 0;
		for (const_iterator i=begin(); i != end(); ++i)
		{
			const_iterator j = i; 
			for (++j; j != end(); ++j)
			{
				bool computed = (0!= rjmcmc::apply_visitor(m_binary_energy,value(i), value(j)));
				bool stored = boost::edge(*i, *j, m_graph).second;
				if (computed != stored)	++err;
			}
		}
		return err;
	}

	const IsValid& is_valid() const { return m_is_valid; }
	bool is_valid(const T& t) const { return rjmcmc::apply_visitor(m_is_valid,t); }

private:
	graph_type m_graph;
	UnaryEnergy	m_unary_energy;
	BinaryEnergy	m_binary_energy;
	IsValid		m_is_valid;
	Accelerator	m_accelerator;
	double m_unary;
	double m_binary;
};


template<typename T, typename U, typename B, typename V, typename A>
std::ostream& operator<<(std::ostream& o, const graph_configuration<T,U,B,V,A>& c) {
	o << "energy     : " << c.unary_energy() + c.binary_energy();
	o << " = " << c.unary_energy() << " + " << c.binary_energy() << " (Data+Prior)\n";
	o << "Nb objects : " << c.size() << "\n";
	o << "Nb edges   : " << c.size_of_interactions() << std::endl;
	{
		typename graph_configuration<T,U,B,V,A>::const_iterator it = c.begin(), end = c.end();
		for (; it != end; ++it)
			o << *it <<"\t" << c.energy(it)<<"\t" << c[it] << std::endl;
	}
	
	{
		typename graph_configuration<T,U,B,V,A>::const_edge_iterator it = c.interactions_begin(), end = c.interactions_end();
		for (; it != end; ++it)
			o << *it <<"\t:\t" << c.energy(it) << std::endl;
	}
	
	return o;
}

// audit log

template<typename Configuration>
bool audit_configuration(const Configuration& c, std::ostream &out = std::cout, double epsilon = 1e-6)
{
	bool res = true;
	double delta1 = std::abs(c.audit_unary_energy()-c.unary_energy());
	if (delta1 > epsilon)
	{
		out << "Ecart energie donnees : "<< delta1 << std::endl;
		res = false;
	}
	double delta2 = std::abs(c.audit_binary_energy()-c.binary_energy());
	if (delta2 > epsilon)
	{
		out << "Ecart energie a priori : " << delta2 << std::endl;
		res = false;
	}
	unsigned int nb_err = c.audit_structure();
	if (nb_err != 0)
	{
		out << "Nombre d'erreurs de structure du graph : " << nb_err << std::endl;
		res = false;
	}
	return res;
}

}; // namespace rjmcmc

#endif // __RJMCMC_CONFIGURATION_HPP__
