#ifndef RJMCMC_CONTAINER_HPP
#define RJMCMC_CONTAINER_HPP

#include <cassert>
#include <boost/graph/adjacency_list.hpp>

#include "BBox.hpp"

template <class NodeType>
class RJMCMC_Container
{
	class RJMCMC_ContainerEdge
	{
	public:
		inline RJMCMC_ContainerEdge(double w = 0.) : m_weight(w) {;}

		inline const double Weight() const { return m_weight; }
		inline void Weight( double w ) { m_weight = w; }

	private:
		double m_weight;
	};

public :
	typedef boost::adjacency_list<boost::vecS, boost::listS, boost::undirectedS,NodeType,RJMCMC_ContainerEdge> GraphType;
	typedef NodeType InternalNodeType;
	typedef typename GraphType::vertex_iterator vertex_iterator;
	typedef typename GraphType::edge_iterator edge_iterator;
	typedef typename GraphType::out_edge_iterator out_edge_iterator;

	typedef std::pair<typename GraphType::vertex_iterator, double> neighboor_and_weight;

protected :
	typedef std::pair< typename GraphType::edge_descriptor , bool > edge_descriptor_bool;
	typedef typename GraphType::vertex_descriptor vertex_descriptor;

public:

	inline RJMCMC_Container() : m_priorEnergy(0.), m_dataEnergy(0.), m_totalSurface(0.) {}

	inline vertex_descriptor AddNode (const NodeType &node, const std::vector<neighboor_and_weight> &newNeighboors)
	{
		m_dataEnergy += node.Weight();
		m_totalSurface += node.Surface();
		vertex_descriptor n = add_vertex(node, m_graph);
		typename std::vector<neighboor_and_weight>::const_iterator it = newNeighboors.begin(), fin = newNeighboors.end();
		for (; it != fin; ++it)
		{
			edge_descriptor_bool new_edge = add_edge(n, *(it->first) ,m_graph);
			// On affecte l'énergie d'attache aux donnees a la nouvelle arete
			assert(new_edge.second && "AddNode : impossible de creer une edge !!!");
			m_graph[ new_edge.first ].Weight( it->second);
			m_priorEnergy += it->second;
		}
		return n;
	}

	inline void ChangeVertex (vertex_iterator vi, const NodeType &node, const std::vector<neighboor_and_weight> &newNeighboors)
	{
		m_dataEnergy -= m_graph[*vi].Weight();
		m_totalSurface -= m_graph[*vi].Surface();
		m_graph[*vi] = node;
		m_dataEnergy += node.Weight();
		m_totalSurface += node.Surface();

		// Soustraction des prior terms sur toutes les aretes adjacentes
		std::pair< typename GraphType::out_edge_iterator , typename GraphType::out_edge_iterator > pair_edges = out_edges( *vi , m_graph );
		for(;pair_edges.first != pair_edges.second;++pair_edges.first)
		{
			m_priorEnergy -= m_graph[ *(pair_edges.first) ].Weight();
		}
		// Suppression des aretes adjacentes
		clear_vertex( *vi, m_graph);

		// creation des nouvelles aretes
		typename std::vector<neighboor_and_weight>::const_iterator it = newNeighboors.begin(), fin = newNeighboors.end();
		for (; it != fin; ++it)
		{
			edge_descriptor_bool new_edge = add_edge(*vi, *(it->first) ,m_graph);
			// On affecte l'énergie d'attache aux donnees a la nouvelle arete
			assert(new_edge.second && "AddNode : impossible de creer une edge !!!");
			m_graph[ new_edge.first ].Weight( it->second);
			m_priorEnergy += it->second;
		}
	}

	inline void RemoveVertex( vertex_iterator vi )
	{
		// Suppression des prior terms sur toutes les aretes adjacentes
		std::pair< typename GraphType::out_edge_iterator , typename GraphType::out_edge_iterator > pair_edges = out_edges( *vi , m_graph );
		for(;pair_edges.first != pair_edges.second;++pair_edges.first)
		{
			m_priorEnergy -= m_graph[ *(pair_edges.first) ].Weight();
		}
		m_dataEnergy -= m_graph[*vi].Weight();
		m_totalSurface -= m_graph[*vi].Surface();
		clear_vertex( *vi, m_graph);
		remove_vertex( *vi , m_graph);
	}

	void Dump(std::ostream &o, bool exportNode = false, bool exportEdge=false) const
	{
		o << "Energy : " << PriorEnergy() + DataEnergy() << "\n";
		o << "(Prior : " << PriorEnergy() << "\t";
		o << "Data : " << DataEnergy() << ")\n";
		o << "Nb node : " << GetNbVertices() << "\n";
		o << "Nb edge : " << GetNbEdges() << std::endl;

		if (exportNode)
		{
//			vertex_iterator it_v = vertices(m_graph).first, fin_v = vertices(m_graph).second;
//			for (; it_v != fin_v; ++it_v)
//				o << *it_v <<"\t" << m_graph[*it_v].Geometry() << std::endl;
		}
		if (exportEdge)
		{
			edge_iterator it_e = edges(m_graph).first, fin_e = edges(m_graph).second;
			for (; it_e != fin_e; ++it_e)
				o << *it_e << "\t:\t" << m_graph[*it_e].Weight() << std::endl;
		}
	}

	/** Accesseurs
	*/
	inline const double PriorEnergy() const { return m_priorEnergy; }
	inline const double DataEnergy() const { return m_dataEnergy; }
	inline const double TotalSurface() const { return m_totalSurface; }

	inline const GraphType & GetGraph() const {return m_graph;}
	inline const int GetNbVertices() const { return num_vertices(m_graph); }
	inline const int GetNbEdges() const { return num_edges(m_graph); }

protected :
	GraphType m_graph;
	double m_priorEnergy;
	double m_dataEnergy;
	double m_totalSurface;
};

#endif // RJMCMC_CONTAINER_HPP
