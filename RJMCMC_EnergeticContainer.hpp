#ifndef RJMCMC_ENERGETICCONTAINER_HPP
#define RJMCMC_ENERGETICCONTAINER_HPP

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include "RJMCMC_Container.hpp"

template <class NodeType, class PriorEnergyComputer, class DataEnergyComputer>
class RJMCMC_EnergeticContainer : public RJMCMC_Container<NodeType>
{
public:
	typedef typename RJMCMC_Container<NodeType>::vertex_iterator 		vertex_iterator;
	typedef typename RJMCMC_Container<NodeType>::neighboor_and_weight 	neighboor_and_weight;
	typedef typename RJMCMC_Container<NodeType>::edge_iterator 			edge_iterator;
	typedef typename RJMCMC_Container<NodeType>::out_edge_iterator 		out_edge_iterator;

	inline RJMCMC_EnergeticContainer(	shared_ptr<PriorEnergyComputer> priorComputer,
										shared_ptr<DataEnergyComputer> dataComputer) :  
		m_priorComputer(priorComputer), m_dataComputer(dataComputer)
	{
		;
	}
	
	double delta_birth(NodeType &newNode, vertex_iterator oldVertex, std::vector<neighboor_and_weight> &newNeighboors) const
	{
		double delta = this->ComputeDataEnergy(newNode);
		newNode.Weight(delta);
		vertex_iterator it_v = vertices(this->GetGraph()).first, fin_v = vertices(this->GetGraph()).second;
		for (; it_v != fin_v; ++it_v)
		{
			/// Si on est en modification, il ne faut pas prendre en compte l'interaction avec la version non modifee
			if (it_v == oldVertex)
				continue;
			double weight = ComputePriorEnergy( this->GetGraph()[ *it_v ], newNode);
			if ( weight != 0 )
			{
				newNeighboors.push_back(neighboor_and_weight(it_v, weight));
				delta += weight;
			}
		}
		return delta;
	}


	double delta_death(vertex_iterator vertex) const
	{
		double delta = -this->GetGraph()[*vertex].Weight();
		if ( vertex != vertices(  this->GetGraph() ).second && this->GetNbVertices() > 1 )
		{
			std::pair< out_edge_iterator, out_edge_iterator > pair_edges = out_edges( *vertex ,  this->GetGraph());
			for(;pair_edges.first!=pair_edges.second; ++pair_edges.first)
			{
				delta -= this->GetGraph()[ *(pair_edges.first) ].Weight();
			}
		}
		return delta;
	}

	inline double ComputeDataEnergy(const NodeType &n) const
	{
		return m_dataComputer->Compute(n);
	}
	
	inline double ComputePriorEnergy(const NodeType &n1, const NodeType &n2) const
	{
		return m_priorComputer->Compute(n1, n2);
	}
	
	double CheckGraphDataEnergyIntegrity()
	{
		// On parcourt tous les sommets et on verifie que la somme des energies vaut l'energie courante
		double computedEnergy = 0.;
		vertex_iterator it_v = vertices(this->GetGraph()).first, fin_v = vertices(this->GetGraph()).second;
		for (; it_v != fin_v; ++it_v)
		{
			computedEnergy += m_dataComputer->Compute( this->GetGraph()[*it_v] );
		}
		return computedEnergy-this->m_dataEnergy;
	}

	double CheckTotalSurfaceIntegrity()
	{
		// On parcourt tous les sommets et on verifie que la somme des energies vaut l'energie courante
		double totalSurface = 0.;
		vertex_iterator it_v = vertices(this->GetGraph()).first, fin_v = vertices(this->GetGraph()).second;
		for (; it_v != fin_v; ++it_v)
		{
			totalSurface += this->GetGraph()[*it_v].Surface();
		}
		return totalSurface-this->m_totalSurface;
	}

	double CheckGraphPriorEnergyIntegrity()
	{
		// On parcourt toutes les aretes et on verifie que la somme des energies vaut l'energie courante
		double computedEnergy = 0.;
		std::pair< edge_iterator, edge_iterator > it_edges = edges(this->GetGraph());
		for(edge_iterator it = it_edges.first; it != it_edges.second;++it)
			computedEnergy += m_priorComputer->Compute( this->GetGraph()[source(*it,this->GetGraph())], this->GetGraph()[target(*it, this->GetGraph())]);
		return computedEnergy-this->m_priorEnergy;
	}

	unsigned int CheckGraphStructureIntegrity()
	{
		// On parcourt tous les couples de noeuds et on verifie que les aretes qui doivent exister existent bien
		unsigned int nb_err = 0;
		vertex_iterator it_1 = vertices(this->GetGraph()).first;
		vertex_iterator fin = vertices(this->GetGraph()).second;
		for (; it_1 != fin; ++it_1)
		{
			vertex_iterator it_2 = it_1; ++it_2;
			for (; it_2 != fin; ++it_2)
			{
				bool computed = (m_priorComputer->Compute(this->GetGraph()[*it_1], this->GetGraph()[*it_2]) != 0);
				bool stored = edge(*it_1, *it_2, this->GetGraph()).second;
				if (computed != stored)
					++nb_err;
			}
		}
		return nb_err;
	}

private:
	shared_ptr<PriorEnergyComputer> m_priorComputer;
	shared_ptr<DataEnergyComputer> m_dataComputer;
};

#endif // RJMCMC_ENERGETICCONTAINER_HPP
