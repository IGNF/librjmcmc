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
	Modification() : m_deltaEnergy(0.) {;}

	inline eProposition Type() const { return m_type; }
	inline const typename DetectorType::InternalNodeType &Node() const { return m_node; }
	inline const typename DetectorType::GraphType::vertex_iterator &Vertex() const { return m_vertex; }
	inline double DeltaEnergy() const { return m_deltaEnergy; }
	inline double DeltaSurface() const { return m_deltaSurface; }

	inline const std::vector<typename DetectorType::neighboor_and_weight> & NewNeighboors() const { return m_newNeighboors; }

	inline void SetBirth(const typename DetectorType::InternalNodeType &node, typename DetectorType::GraphType::vertex_iterator vertex, const DetectorType &detector)
	{
		m_type = eBIRTH;
		m_node = node;
		m_vertex = vertex;
		m_deltaEnergy = detector.delta_birth(this->m_node, this->Vertex(), this->m_newNeighboors );
		m_deltaSurface = m_node.Surface();
	}

	inline void SetDeath(typename DetectorType::GraphType::vertex_iterator vertex, const DetectorType &detector)
	{
		m_type = eDEATH;
		m_vertex = vertex;
		m_deltaEnergy = detector.delta_death(this->Vertex());
		m_deltaSurface = - detector.GetGraph()[*(this->Vertex())].Surface();
	}

	inline void SetModify(typename DetectorType::GraphType::vertex_iterator vertex, const typename DetectorType::InternalNodeType &node, const DetectorType &detector)
	{
		m_type = eMODIFY;
		m_vertex = vertex;
		m_node = node;
		m_deltaEnergy = detector.delta_death(vertex) + detector.delta_birth(this->m_node, this->m_vertex, this->m_newNeighboors);
		m_deltaSurface = m_node.Surface() - detector.GetGraph()[*(this->Vertex())].Surface();
	}

	void Apply( DetectorType &detector ) const
	{
		switch ( this->Type() )
		{
			case eBIRTH :
				detector.AddNode( this->Node(), this->NewNeighboors() );
				break;
			case eDEATH :
				detector.RemoveVertex( this->Vertex() );
				break;
			case eMODIFY :
				detector.ChangeVertex( this->Vertex(), this->Node(), this->NewNeighboors() );
				break;
			default :
				throw;
		}
	}

private:
	eProposition m_type;
	typename DetectorType::InternalNodeType m_node;
	typename DetectorType::GraphType::vertex_iterator m_vertex;
	double m_deltaEnergy;
	double m_deltaSurface;
	std::vector<typename DetectorType::neighboor_and_weight> m_newNeighboors;
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

	inline double Temperature() const { return m_temp; }
	inline double CoefDescente() const { return m_q; }
	inline const std::vector<double> &CumulatedProbabilities() const { return m_probas; }

	inline const MultiDimensionnalBBox<DIMENSION> & GetBox() const { return m_box; }

	eProposition Itere(DetectorType &detector)
	{
		Modification< DetectorType  > modif;
		ProposeModification(detector, modif);
		if ( AcceptModification(detector, modif) )
		{
			modif.Apply(detector);
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
				typename DetectorType::GraphType::vertex_iterator vertex = vertices( detector.GetGraph() ).second;
				modif.SetBirth( node, vertex, detector );
				break;
			}
			case eDEATH :
			{
				typename DetectorType::GraphType::vertex_iterator vertex = ChooseRandomVertex( detector.GetGraph() );
				modif.SetDeath( vertex, detector );
				break;
			}
			default : // eMODIFY
			{
				typename DetectorType::GraphType::vertex_iterator vertex = ChooseRandomVertex( detector.GetGraph() );
				typename DetectorType::InternalNodeType node = detector.GetGraph()[ *vertex ];
				node.RandomModify(GetBox()) ;
				modif.SetModify(vertex, node, detector);
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

		double R;
		switch ( modif.Type() )
		{
			case eBIRTH :
			{
				//R = 1.- detector.GetNbVertices() / double( GetBox().Volume() ) ;
				double surface = detector.TotalSurface() + modif.DeltaSurface();
				//double surface = BuildingsDetectorParametersSingleton::Instance()->MinimalSize();
				//surface *= surface*detector.GetNbVertices();
				R = GetBox().Volume() / surface ;
				break;
			}
			case eDEATH :
			{
				//R = detector.GetNbVertices() / double( GetBox().Volume() ) ;
				double surface = detector.TotalSurface() - modif.DeltaSurface();
				//double surface = BuildingsDetectorParametersSingleton::Instance()->MinimalSize();
				//surface *= surface*detector.GetNbVertices();
				R = surface / GetBox().Volume();
				break;
			}
			case eMODIFY :
			{
				// Ici : delta = 0 => acceptation = 50 %
//				R = .5;
				//R = 1.;
				double surface = detector.TotalSurface() + modif.DeltaSurface();
				R = surface / GetBox().Volume();
				break;
			}
			default :
				throw ;
		}

		double delta = modif.DeltaEnergy();
		// Cas non stochastique : on accepte les descentes
		if (m_temp == 0)
			return (delta <= 0);

		R *= exp ( - delta / m_temp );
		m_temp *= m_q;
		if (R >= 1.)
			return true;
		boost::variate_generator<RJMCMCRandom&, boost::bernoulli_distribution<> > die(GetRandom(), boost::bernoulli_distribution<>(R));
		return die();
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
