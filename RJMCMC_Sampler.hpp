#ifndef RJMCMC_SAMPLER_HPP
#define RJMCMC_SAMPLER_HPP

#include "Random.hpp"

enum eProposition
{
	eBIRTH
	,eDEATH
	,eMODIFY
	,eNONE
};

template < class DetectorType>
class Modification
{
public:
	Modification() {;}

	eProposition Type() const { return m_type; }
	const typename DetectorType::InternalNodeType &Node() const { return m_node; }
	const typename DetectorType::GraphType::vertex_iterator &Vertex() { return m_vertex; }
	void Vertex( typename DetectorType::GraphType::vertex_iterator vertex ) { m_vertex = vertex; }

	void SetBirth(const typename DetectorType::InternalNodeType &node)
	{
		m_type = eBIRTH;
		m_node = node;
	}

	void SetDeath(typename DetectorType::GraphType::vertex_iterator vertex)
	{
		m_type = eDEATH;
		m_vertex = vertex;
	}

	void SetModify(typename DetectorType::GraphType::vertex_iterator vertex, const typename DetectorType::InternalNodeType &node)
	{
		m_type = eMODIFY;
		m_vertex = vertex;
		m_node = node;
	}

	std::vector<typename DetectorType::neighboor_and_weight> m_newNeighboors;

private:
	eProposition m_type;
	typename DetectorType::InternalNodeType m_node;
	typename DetectorType::GraphType::vertex_iterator m_vertex;

};

template < class DetectorType, unsigned int DIMENSION = 2>
class Sampler
{
	MultiDimensionnalBBox<DIMENSION> m_box;
	double m_temp;
	double m_q;
	std::vector<double> m_probas;

public :
	Sampler(const MultiDimensionnalBBox<DIMENSION> &box, double tempInitiale, double coef_descente, const std::vector<double> &probas):
	   m_box(box), m_temp(tempInitiale), m_q(coef_descente), m_probas(probas)
	{}

	double Temperature() const { return m_temp; }
	void Temperature(double temp) { m_temp = temp; }

	double CoefDescente() const { return m_q; }
	void CoefDescente(double coef) { m_q = coef; }

	const std::vector<double> &CumulatedProbabilities() const { return m_probas; }
	void CumulatedProbabilities(const std::vector<double> &p) { m_probas = p; }

	inline const MultiDimensionnalBBox<DIMENSION> & GetBox() const { return m_box; }

	eProposition Itere(DetectorType &detector)
	{
		Modification< DetectorType  > modif;
		ProposeModification(detector, modif);
		if ( AcceptModification(detector, modif) )
		{
			ApplyModification(detector, modif);
			return modif.Type();
		}
		return eNONE;
	}

	void ProposeModification(DetectorType &detector, Modification<DetectorType> &modif)
	{
		int choix = eBIRTH;
		if (num_vertices(detector.GetGraph()) != 0)
		{
			boost::variate_generator<RJMCMCRandom&, boost::uniform_real<> > die(GetRandom(), boost::uniform_real<>(0,1));
			float val = die();

			for (unsigned int i=0;i<m_probas.size();++i)
				if ( val <= m_probas[i] )
				{
					choix = i;
					break;
				}
		}

		switch (choix)
		{
			case eBIRTH :
			{
				typename DetectorType::InternalNodeType node;
				node.RandomInit(GetBox()) ;
				node.Weight(detector.ComputeDataEnergy(node));
				modif.SetBirth( node );
				modif.Vertex( vertices( detector.GetGraph() ).second );
				break;
			}
			case eDEATH :
			{
				modif.SetDeath( ChooseRandomVertex( detector.GetGraph() ) );
				break;
			}
			default : // eMODIFY
			{
				typename DetectorType::GraphType::vertex_iterator vertex = ChooseRandomVertex( detector.GetGraph() );
				typename DetectorType::InternalNodeType node = detector.GetGraph()[ *vertex ];
				node.RandomModify(GetBox()) ;
				node.Weight(detector.ComputeDataEnergy(node));
				modif.SetModify(vertex, node);
				break;
			}
		}
	}

	// true si modif a appliquer, false sinon
	bool AcceptModification(DetectorType &detector, Modification<DetectorType> &modif)
	{
		// Le choix se fait en fonction de m_lastModification
		// En cas de naisssance, on calcule les nouveaux voisins
		//   1. Delta(Ep) = Somme( Ep(new_node,voisins_new_node) )
		//   2. Delta(Ed) = Ed(new_node)
		// En cas de mort, on calcule les aretes impactees
		//   1. Delta(Ep) = - Somme( Ep(removed_node,voisins_removed_node) )
		//   2. Delta(Ed) = - Ed(removed_node)

		double R = 0., delta = 0.;
		switch ( modif.Type() )
		{
			case eBIRTH :
			{
				delta = detector.delta_birth(modif.Node(), modif.Vertex(), modif.m_newNeighboors);
				//R = 1.- detector.GetNbVertices() / double( GetBox().Volume() ) ;
				double surface = detector.TotalSurface() + modif.Node().Surface();
				//double surface = BuildingsDetectorParametersSingleton::Instance()->MinimalSize();
				//surface *= surface*detector.GetNbVertices();
				R = GetBox().Volume() / surface ;
				break;
			}
			case eDEATH :
			{
				delta = detector.delta_death(modif.Vertex());
				//R = detector.GetNbVertices() / double( GetBox().Volume() ) ;
				double surface = detector.TotalSurface();
				//double surface = BuildingsDetectorParametersSingleton::Instance()->MinimalSize();
				//surface *= surface*detector.GetNbVertices();
				R = surface / GetBox().Volume();
				break;
			}
			case eMODIFY :
			{
				delta = detector.delta_death(modif.Vertex());
				delta+= detector.delta_birth(modif.Node(), modif.Vertex(), modif.m_newNeighboors);
				// Ici : delta = 0 => acceptation = 50 %
				R = .5;
				//R = 1.;
				break;
			}
			default :
				throw ;
		}

		// Cas non stochastique : on accepte les descentes
		if (m_temp == 0)
			return (delta <= 0);

		m_temp *= m_q;
		R *= exp ( - delta / m_temp );
		if (R >= 1.)
			return true;
		boost::variate_generator<RJMCMCRandom&, boost::bernoulli_distribution<> > die(GetRandom(), boost::bernoulli_distribution<>(R));
		return die();
	}

	void ApplyModification( DetectorType &detector, Modification<DetectorType> &modif )
	{
		switch ( modif.Type() )
		{
			case eBIRTH :
				detector.AddNode( modif.Node(), modif.m_newNeighboors );
				break;
			case eDEATH :
				detector.RemoveVertex( modif.Vertex() );
				break;
			case eMODIFY :
				detector.ChangeVertex( modif.Vertex(), modif.Node(), modif.m_newNeighboors);
				break;
			default :
				throw;
		}
	}

private :
	typename DetectorType::GraphType::vertex_iterator ChooseRandomVertex(const typename DetectorType::GraphType &graph)
	{
		boost::variate_generator<RJMCMCRandom&, boost::uniform_smallint<> > die(GetRandom(), boost::uniform_smallint<>(0,num_vertices(graph)-1));
		typename DetectorType::GraphType::vertex_iterator it = vertices(graph).first;
		std::advance(it, die());
		return it;
	}
};

#endif // RJMCMC_SAMPLER_HPP
