#ifndef RJMCMC_BUILDINGS_DETECTOR_HPP
#define RJMCMC_BUILDINGS_DETECTOR_HPP

#include <cassert>
#include <cmath>
#include <boost/graph/adjacency_list.hpp>

#include "BBox.hpp"

/** Concepts a verifier
class PriorEnergyPolicy
{
public :
* valeur de retour : enregie d'interaction entre les deux noeuds
	inline double ComputePriorEnergy(const NodeType &n1, const NodeType &n2);
};

class DataEnergyPolicy
{
public :
* valeur de retour : attache aux donnees du noeud
	inline double ComputeDataEnergy(const NodeType &n);
};
*/

template <class NodeType, class PriorEnergyPolicy, class DataEnergyPolicy, unsigned int DIMENSION = 2 >
class RJMCMC_Detector : public PriorEnergyPolicy, public DataEnergyPolicy
{
	class RJMCMC_DetectorEdge
	{
	public:
		inline RJMCMC_DetectorEdge(double w = 0.) : m_weight(w) {;}

		inline const double Weight() const { return m_weight; }
		inline void Weight( double w ) { m_weight = w; }

	protected:
	private:
		double m_weight;
	};

public :
	typedef boost::adjacency_list<boost::vecS, boost::listS, boost::undirectedS,NodeType,RJMCMC_DetectorEdge> GraphType;
	typedef NodeType InternalNodeType;
	typedef typename GraphType::vertex_iterator vertex_iterator;
	typedef typename GraphType::edge_iterator edge_iterator;
	typedef typename GraphType::out_edge_iterator out_edge_iterator;

	typedef std::pair<typename GraphType::vertex_iterator, double> neighboor_and_weight;

private :
	typedef std::pair< typename GraphType::edge_descriptor , bool > edge_descriptor_bool;
	typedef typename GraphType::vertex_descriptor vertex_descriptor;

public:

	inline RJMCMC_Detector(const MultiDimensionnalBBox<DIMENSION> &box) : m_box(box), m_priorEnergy(0.), m_dataEnergy(0.), m_totalSurface(0.) {}
	inline ~RJMCMC_Detector() {}

	inline vertex_descriptor AddNode (const NodeType &node)
	{
		m_dataEnergy += node.Weight();
		m_totalSurface += node.Surface();
		vertex_descriptor n = add_vertex(node, m_graph);
		vertex_iterator it = vertices(m_graph).first, fin = vertices(m_graph).second;
		for (; it != fin; ++it)
			if (*it != n)
			{
				double prior = ComputePriorEnergy( node , m_graph[*it]);
				if ( prior != 0 )
				{
					edge_descriptor_bool new_edge = add_edge(n, *it ,m_graph);
					assert(new_edge.second && "AddNode : impossible de creer une edge !!!");
					m_graph[ new_edge.first ].Weight( prior );
					m_priorEnergy += prior;
				}
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

	double CheckGraphDataEnergyIntegrity()
	{
		// On parcourt tous les sommets et on verifie que la somme des energies vaut l'energie courante
		double computedEnergy = 0.;
		vertex_iterator it_v = vertices(m_graph).first, fin_v = vertices(m_graph).second;
		for (; it_v != fin_v; ++it_v)
		{
			computedEnergy += ComputeDataEnergy( m_graph[*it_v] );
		}
		return computedEnergy-m_dataEnergy;
	}

	double CheckTotalSurfaceIntegrity()
	{
		// On parcourt tous les sommets et on verifie que la somme des energies vaut l'energie courante
		double totalSurface = 0.;
		vertex_iterator it_v = vertices(m_graph).first, fin_v = vertices(m_graph).second;
		for (; it_v != fin_v; ++it_v)
		{
			totalSurface += m_graph[*it_v].Surface();
		}
		return totalSurface-m_totalSurface;
	}

	double CheckGraphPriorEnergyIntegrity()
	{
		// On parcourt toutes les aretes et on verifie que la somme des energies vaut l'energie courante
		double computedEnergy = 0.;
		std::pair< edge_iterator, edge_iterator > it_edges = edges(m_graph);
		for(edge_iterator it = it_edges.first; it != it_edges.second;++it)
			computedEnergy += ComputePriorEnergy( m_graph[source(*it,m_graph)], m_graph[target(*it, m_graph)]);
		return computedEnergy-m_priorEnergy;
	}

	unsigned int CheckGraphStructureIntegrity()
	{
		// On parcourt tous les couples de noeuds et on verifie que les aretes qui doivent exister existent bien
		unsigned int nb_err = 0;
		vertex_iterator it_1 = vertices(m_graph).first;
		vertex_iterator fin = vertices(m_graph).second;
		for (; it_1 != fin; ++it_1)
		{
			vertex_iterator it_2 = it_1; ++it_2;
			for (; it_2 != fin; ++it_2)
			{
				bool computed = (ComputePriorEnergy(m_graph[*it_1], m_graph[*it_2]) != 0);
				bool stored = edge(*it_1, *it_2, m_graph).second;
				if (computed != stored)
					nb_err++;
			}
		}
		return nb_err;
	}

	bool CheckGraphEnergiesIntegrity(std::ostream &out = std::cout, double epsilon = 1e-6)
	{
		bool res = true;
		double deltadata = std::abs(CheckGraphDataEnergyIntegrity());
		if (deltadata > epsilon)
		{
			out << "Ecart energie donnees : "<< deltadata << std::endl;
			res = false;
		}
		double deltaprior = std::abs(CheckGraphPriorEnergyIntegrity());
		if (deltaprior > epsilon)
		{
			out << "Ecart energie a priori : " << deltaprior << std::endl;
			res = false;
		}
		unsigned int nb_err = CheckGraphStructureIntegrity();
		if (nb_err != 0)
		{
			out << "Nombre d'erreurs de structure du graph : " << nb_err << std::endl;
			res = false;
		}
		return res;
	}

	/** Accesseurs
	*/
	inline const double PriorEnergy() const { return m_priorEnergy; }
	inline const double DataEnergy() const { return m_dataEnergy; }
	inline const double TotalSurface() const { return m_totalSurface; }

	inline const MultiDimensionnalBBox<DIMENSION> & GetBox() const { return m_box; }
	inline const GraphType & GetGraph() const {return m_graph;}
	inline const int GetNbVertices() const { return num_vertices(m_graph); }
	inline const int GetNbEdges() const { return num_edges(m_graph); }

private:
	GraphType m_graph;
	MultiDimensionnalBBox<DIMENSION> m_box;
	double m_priorEnergy;
	double m_dataEnergy;
	double m_totalSurface;
};

#endif // RJMCMC_BUILDINGS_DETECTOR_HPP
