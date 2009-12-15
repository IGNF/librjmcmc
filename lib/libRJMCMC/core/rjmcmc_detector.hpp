#ifndef __RJMCMC_BUILDINGS_DETECTOR_HPP__
#define __RJMCMC_BUILDINGS_DETECTOR_HPP__

//////////////////////////////////////////////////////////

#include "core/bbox.hpp"

class BoxIsValid
{
public:
	BoxIsValid(const bbox_2 &box, double min_size, double max_ratio) : m_box(box), m_squared_min_size(min_size*min_size), m_max_ratio(max_ratio) {}
	template<typename T> bool operator()(const T &n) const;
	const bbox_2& bbox() const { return m_box; }
private:
	bbox_2 m_box;
	double m_squared_min_size;
	double m_max_ratio;
};

template<typename T> bool BoxIsValid::operator()(const T &n) const
{
	for (unsigned int i = 0; i < 4; ++i)
	{
		bbox_2::point_type ptb;
		ptb[0] = n[i].x();
		ptb[1] = n[i].y();

		if (!m_box.is_inside(ptb)) return false;
	}
	if (n.squared_length(0) < m_squared_min_size || n.squared_length(1) < m_squared_min_size)
		return false;

	float ratio = std::abs(n.ratio());
	return (ratio <= m_max_ratio && 1 <= m_max_ratio*ratio );
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
	value_type	m_birth;
	iterator	m_death;
public:
	inline const value_type& birth() const { return m_birth; }
	inline iterator          death() const { return m_death; }
	inline void birth(const value_type& b) { m_birth = b; }
	inline void death(iterator d)  { m_death=d; }
	inline unsigned int birth_size(const Configuration &c) const { return (m_birth !=value_type()) ? 1 : 0; }
	inline unsigned int death_size(const Configuration &c) const { return (m_death !=c.end()) ? 1 : 0; }
};
}

//////////////////////////////////////////////////////////

template<typename T, typename UnaryEnergy, typename BinaryEnergy, typename IsValid, typename Accelerator>
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

	vector_configuration(UnaryEnergy unary_energy, BinaryEnergy binary_energy, IsValid is_valid, Accelerator accelerator) : m_unary_energy(unary_energy), m_binary_energy(binary_energy), m_is_valid(is_valid), m_accelerator(accelerator)
	{}

	const IsValid& is_valid() const { return m_is_valid; }
	bool is_valid(const T& t) const { return m_is_valid(t); }

	// energy
	double unary_energy() const
	{
		double e = 0.;
		for (const_iterator it = begin(); it != end(); ++it)
			e += m_unary_energy( *it );
		return e;
	}

	double binary_energy() const
	{
		double e = 0.;
		for (const_iterator i = begin(); i != end(); ++i)
			for (const_iterator j = i+1; j != end(); ++j)
				e += m_binary_energy( *i, *j );
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
	inline double energy( const_iterator v ) const { return m_unary_energy(*v); }

	// evaluators

	template <typename Modification> double delta_energy(const Modification &modif) const
	{
		return delta_birth(modif)+delta_death(modif);
	}

	template <typename Modification> double delta_birth(const Modification &modif) const
	{
		const_iterator v	 = modif.death();
		const value_type& obj    = modif.birth();
		if(obj==value_type()) return 0;
		double delta             = m_unary_energy(obj);
		for (const_iterator it=begin(); it != end(); ++it)
			if (it != v) delta += m_binary_energy( obj, value(it) );
		return delta;
	}

	template <typename Modification> double delta_death(const Modification &modif) const
	{
		const_iterator v = modif.death();
		if ( v == end() ) return 0.;
		const value_type& obj = value(v);
		double delta = - energy(v);
		for (const_iterator it=begin(); it != end(); ++it)
			if (it != v) delta -= m_binary_energy( obj, value(it) );
		return delta;
	}

	// manipulators
	template <typename Modification> void apply(const Modification &modif)
	{
		remove(modif.death());
		insert(modif.birth());
	}

	void insert(const value_type &obj) { 
		if(obj == value_type()) return;
		m_container.push_back(obj);
	}
	void remove( iterator v ) {
		if(v==end()) return;
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


//////////////////////////////////////////////////////////

#include <boost/graph/adjacency_list.hpp>

template<typename T, typename UnaryEnergy, typename BinaryEnergy, typename IsValid, typename Accelerator, typename OutEdgeList = boost::listS, typename VertexList = boost::listS  >
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
/*
	class candidate_node : public node, public std::list<node> {
	public:
		candidate_node(const value_type& obj) : node(obj,0) { }
	};

	class modification {
		std::vector<vertex_descriptor> m_death;
		std::vector<candidate_node>    m_birth;
		double m_delta_energy;
	public:
		inline double& delta_energy() { return m_delta_energy; }
		inline double  delta_energy() const { return m_delta_energy; }
		void push_back(const value_type& obj) { m_birth.push_back(candidate_node(obj)); }
		void push_back(vertex_descriptor v) { m_death.push_back(v); }
	};
*/

public:

	// configuration constructors/destructors
	graph_configuration(UnaryEnergy unary_energy, BinaryEnergy binary_energy, IsValid is_valid, Accelerator accelerator) : m_unary(0.), m_binary(0.), m_unary_energy(unary_energy), m_binary_energy(binary_energy), m_is_valid(is_valid), m_accelerator(accelerator)
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
		iterator v 		= modif.death();
		const value_type& obj   = modif.birth();
		if ( obj == value_type() ) return 0.;
		double delta             = m_unary_energy(obj);
		for (iterator it=begin(); it != end(); ++it)
			if (it != v) delta += m_binary_energy( obj, value(it) );
		return delta;
	}

	template <typename Modification> double delta_death(const Modification &modif) const
	{
		iterator v = modif.death();
		if ( v == end() ) return 0.;
		double delta = - energy(v);
		out_edge_iterator it, end;
		for(boost::tie(it,end) = out_edges( *v, m_graph ); it!=end; ++it)
			delta -= m_graph[ *it ].energy();
		return delta;
	}

	template <typename Modification> void apply(const Modification &modif)
	{
		remove(modif.death());
		insert(modif.birth());
	}

	// manipulators
	void insert(const value_type& obj)
	{
		if(obj == value_type()) return;
		node n(obj, m_unary_energy(obj));
		m_unary += n.energy();
		vertex_descriptor d = add_vertex(n, m_graph);
		iterator   it, end;
		
		for (boost::tie(it,end)=m_accelerator(*this,obj); it != end; ++it) {
			if ( *it == d ) continue;
			double e = m_binary_energy( obj, value(it) );
			if (   e == 0 ) continue;
			edge_descriptor_bool new_edge = add_edge(d, *it, m_graph );
			m_graph[ new_edge.first ].energy( e );
			m_binary += e;
		}
	}

	void remove( iterator v )
	{
		if ( v == end() ) return;
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
			e += m_unary_energy( value(i) );
		return e;
	}

	double audit_binary_energy() const
	{
		double e = 0.;
		const_edge_iterator it, end;
		for(boost::tie(it,end) = edges( m_graph ); it!=end; ++it)
			e += m_binary_energy(	m_graph[source(*it,m_graph)].value() ,
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
				bool computed = (0!=m_binary_energy(value(i), value(j)));
				bool stored = boost::edge(*i, *j, m_graph).second;
				if (computed != stored)	++err;
			}
		}
		return err;
	}

	const IsValid& is_valid() const { return m_is_valid; }
	bool is_valid(const T& t) const { return m_is_valid(t); }

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


#endif // __RJMCMC_BUILDINGS_DETECTOR_HPP__
