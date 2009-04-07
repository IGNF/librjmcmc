#include "GeometricNode.hpp"
#include <boost/variant.hpp>

class InitVisitor : public boost::static_visitor<>
{
	const BBox & m_bbox;
public:
	InitVisitor(const BBox &bbox) : m_bbox(bbox){}

	void operator()( Rectangle_2 & n) const
	{
		::RandomInit(m_bbox, n);
	}

    void operator()( Cercle_2 & n) const
	{
		::RandomInit(m_bbox, n);
	}
};

class ModifyVisitor : public boost::static_visitor<>
{
	const BBox & m_bbox;
public:
	ModifyVisitor(const BBox &bbox) : m_bbox(bbox){}

	void operator()( Rectangle_2 & n) const
	{
		::RandomModify(m_bbox, n);
	}

    void operator()( Cercle_2 & n) const
	{
		::RandomModify(m_bbox, n);
	}
};

class IsValidVisitor : public boost::static_visitor<bool>
{
	const BBox & m_bbox;
public:
	IsValidVisitor(const BBox &bbox) : m_bbox(bbox){}

	bool operator()(const Rectangle_2 & n) const
	{
		return ::IsValid(m_bbox, n);
	}

    bool operator()(const Cercle_2 & n) const
	{
		return ::IsValid(m_bbox, n);
	}
};

template<class NodeGeometry>
class VariantNode
{
public :
	VariantNode() : m_weight(0) {;}
	VariantNode(const VariantNode &n) : m_geometry(n.m_geometry), m_weight(n.m_weight)
	{}

	void RandomModify(const BBox &box)
	{
		NodeGeometry old = m_geometry;
		boost::apply_visitor(ModifyVisitor(box), m_geometry);

		if (!IsValid(box))
		{
			m_geometry = old;
			RandomModify(box);
		}
	}

	void RandomInit(const BBox &box)
	{
		boost::variate_generator<RJMCMCRandom&, boost::uniform_smallint<> > die(GetRandom(), boost::uniform_smallint<>(0,1));
		if (die())
			m_geometry = Cercle_2();
		else
			m_geometry = Rectangle_2();

		boost::apply_visitor(InitVisitor(box), m_geometry);
		if (!IsValid(box))
			RandomInit(box);
	}

	const double Weight() const { return m_weight; }
	void Weight( double w ) { m_weight = w; }

	const NodeGeometry & Geometry() const { return m_geometry; }

private :
	bool IsValid(const BBox &box)
	{
		return boost::apply_visitor(IsValidVisitor(box), Geometry());
	}
	NodeGeometry m_geometry;
	double m_weight;
};

class ComputeDataEnergyVisitor : public boost::static_visitor<double>, public ImageGradientEnergyPolicy
{
public:
	ComputeDataEnergyVisitor() {}

	double operator()(const Rectangle_2 &r) const
    {
		return ImageGradientEnergyPolicy::ComputeDataEnergy(r);
    }

    double operator()(const Cercle_2 &c) const
    {
		return ImageGradientEnergyPolicy::ComputeDataEnergy(c);
    }

};


class VariantImageExporter : public ImageExporter, public boost::static_visitor<>
{
public :
	void operator()(const Rectangle_2 &r) const
    {
		ExportNode(r);
	}

	void operator()(const Cercle_2 &c) const
    {
		ExportNode(c);
	}
};

template<class NodeGeometry>
class VariantGradientDataEnergy
{
	ComputeDataEnergyVisitor m_dataVisitor;
	VariantImageExporter m_exporter;

public :
	double ComputeDataEnergy(VariantNode<NodeGeometry> const  &n) const
    {
		return 	boost::apply_visitor(m_dataVisitor, n.Geometry());
    }

	void InitExport() const { m_exporter.InitExport(); }

	void ExportNode(const NodeGeometry &n) const
	{
		boost::apply_visitor(m_exporter, n);
	}

	void EndExport(const char *filename) const  
	{
		m_exporter.EndExport(filename);
	}
};

class NeighboorVisitor : public boost::static_visitor<bool>
{
public:

    bool operator()( const Rectangle_2 &n1, const Rectangle_2 &n2 ) const
    {
        return n1.do_intersect(n2); 
    }
    bool operator()( const Rectangle_2 &n1, const Cercle_2 &n2 ) const
    {
        return (*this)(n2, n1); 
    }
    bool operator()( const Cercle_2 &n1, const Cercle_2 &n2 ) const
    {
        return n1.do_intersect(n2); 
    }
// TODO : Faire un meilleur calcul
    bool operator()( const Cercle_2 &n1, const Rectangle_2 &n2 ) const
    {
		Point_2 center = n2.center();
		double r = ::sqrt(std::min(n2.squared_length(0), n2.squared_length(1)));
		Cercle_2 c(center, r);
        return c.do_intersect(n1);
    }
};

class ComputePriorEnergyVisitor : public boost::static_visitor<double>
{
public:
	ComputePriorEnergyVisitor() : m_coefSurface(BuildingsDetectorParametersSingleton::Instance()->IntersectionSurfacePonderation()) {}

    double operator()( const Rectangle_2 &n1, const Rectangle_2 &n2 ) const
    {
		return m_coefSurface * n1.intersection_area(n2);
    }

	double operator()( const Rectangle_2 &n1, const Cercle_2 &n2 ) const
    {
		return m_coefSurface * std::min(n1.area(), n2.area()); 
    }

	double operator()( const Cercle_2 &n1, const Cercle_2 &n2 ) const
    {
        return m_coefSurface * n1.intersection_area(n2);
    }

    double operator()( const Cercle_2 &n1, const Rectangle_2 &n2 ) const
    {
        return (*this)(n2, n1); 
    }

	double m_coefSurface;
};

template<class NodeGeometry>
class VariantIntersectionPriorEnergy
{
public :
	bool AreNeighbor(VariantNode<NodeGeometry> const  &n1, VariantNode<NodeGeometry> const  &n2) const
	{
		return 	boost::apply_visitor(NeighboorVisitor(), n1.Geometry(), n2.Geometry());
	}

	double ComputePriorEnergy(VariantNode<NodeGeometry> const  &n1, VariantNode<NodeGeometry> const  &n2) const
	{
		return 	boost::apply_visitor(ComputePriorEnergyVisitor(), n1.Geometry(), n2.Geometry());
	}
};
/*
#include <iostream>
#include <string>
#include <strstream>

#include "ImageGradientEnergyPolicy.hpp"

#include "BuildingsDetectorParameters.hpp"
#include "RectangleNode.hpp"
#include "CircleNode.hpp"

class ComputeDataEnergyVisitor : public boost::static_visitor<double>
{
	const ImageGradientEnergyPolicy &m_p;

public:
	ComputeDataEnergyVisitor(const ImageGradientEnergyPolicy &p) : m_p(p) {}

	double operator()(const RectangleNode &n) const
    {
        return m_p.ComputeDataEnergy(n);
    }

    double operator()(const CircleNode &n) const
    {
        return m_p.ComputeDataEnergy(n);
    }

};


class InitVisitor : public boost::static_visitor<>
{
	const BBox & m_bbox;
public:
	InitVisitor(const BBox &bbox) : m_bbox(bbox){}

	void operator()( RectangleNode & n) const
	{
		n.RandomInit(m_bbox);
	}

    void operator()( CircleNode & n) const
	{
		n.RandomInit(m_bbox);
	}
};

class ModifyVisitor : public boost::static_visitor<>
{
	const BBox & m_bbox;
public:
	ModifyVisitor(const BBox &bbox) : m_bbox(bbox){}

	void operator()( RectangleNode & n) const
	{
		n.RandomModify(m_bbox);
	}

    void operator()( CircleNode & n) const
	{
		n.RandomModify(m_bbox);
	}
};

class NeighboorVisitor : public boost::static_visitor<bool>
{
public:

    bool operator()( const RectangleNode &n1, const RectangleNode &n2 ) const
    {
        return n1.Geometry().do_intersect(n2.Geometry()); 
    }
    bool operator()( const RectangleNode &n1, const CircleNode &n2 ) const
    {
        return (*this)(n2, n1); 
    }
    bool operator()( const CircleNode &n1, const CircleNode &n2 ) const
    {
        return n1.Geometry().do_intersect(n2.Geometry()); 
    }
    bool operator()( const CircleNode &n1, const RectangleNode &n2 ) const
    {
		Point_2 center = n2.Geometry().center();
		double r = ::sqrt(std::max(n2.Geometry().squared_length(0), n2.Geometry().squared_length(1)));
		Cercle_2 c(center, r);
        return c.do_intersect(n1.Geometry());
    }
};

class ComputePriorEnergyVisitor : public boost::static_visitor<double>
{
	double m_coefSurface;

public:
	ComputePriorEnergyVisitor(double coefSurface) : m_coefSurface(coefSurface) {}

    double operator()( const RectangleNode &n1, const RectangleNode &n2 ) const
    {
        return m_coefSurface * std::abs(n1.Geometry().intersection_area(n2.Geometry()));; 
    }
    double operator()( const RectangleNode &n1, const CircleNode &n2 ) const
    {
        return m_coefSurface * n1.Geometry().area() * n2.Geometry().area(); 
    }
    double operator()( const CircleNode &n1, const CircleNode &n2 ) const
    {
        return m_coefSurface * std::abs(n1.Geometry().intersection_area(n2.Geometry()));
    }
    double operator()( const CircleNode &n1, const RectangleNode &n2 ) const
    {
        return (*this)(n2, n1); 
    }
};

#include "RJMCMC_Detector.hpp"
#include "RJMCMC_Sampler.hpp"

#include "cgal_types.h"

#include <boost/variant.hpp>

typedef RJMCMC_Detector<boost::variant<RectangleNode, CircleNode>, RectanglePriorEnergyPolicy, ImageGradientEnergyPolicy >	BuildingsDetector;


int main1 (int argc, char **argv)
{
	boost::variant<RectangleNode, CircleNode> v1, v2;
//	int n = boost::variant<RectangleNode, CircleNode, int>::size();
	if (true)
	{
		CircleNode n;
		v1 = n;
	}
	else
	{
		RectangleNode n;
		v1 = n;
	}
	BBox bbox;
	InitVisitor iv(bbox);
	boost::apply_visitor(iv, v1);
	boost::apply_visitor(iv, v2);
	boost::apply_visitor(NeighboorVisitor(), v1, v2);
	boost::apply_visitor(ComputePriorEnergyVisitor(10.), v1, v2);

	if (!BuildingsDetectorParametersSingleton::Instance()->ParseCmdLine(argc, argv))
		return -1;
	
	BBox::PointType size , origin;
	size[0] = BuildingsDetectorParametersSingleton::Instance()->RunningWidth()-1;
	size[1] = BuildingsDetectorParametersSingleton::Instance()->RunningHeight()-1;
	origin[0] = BuildingsDetectorParametersSingleton::Instance()->RunningOriginX();
	origin[1] = BuildingsDetectorParametersSingleton::Instance()->RunningOriginY();
	BuildingsDetector buildingsDetector(BBox(size, origin));

	Sampler< BuildingsDetector > sampler( BuildingsDetectorParametersSingleton::Instance()->InitialTemperature() , BuildingsDetectorParametersSingleton::Instance()->DecreaseCoefficient(), BuildingsDetectorParametersSingleton::Instance()->CumulatedProbabilities() );
	std::cout << BuildingsDetectorParametersSingleton::Instance()->InitialTemperature() << std::endl;

	// Formattage du log sous forme de tableau, ca facilite la creation de graphiques ...
	std::ostringstream my_out_stream;
	my_out_stream << "\tIte\t";
	my_out_stream << "NbObjects\t";
	my_out_stream << "Time(s)\t";
	my_out_stream << "Temp\t";
	my_out_stream << "E_data\t";
	my_out_stream << "E_priori\t";
	my_out_stream << "E_total\t";
	std::cout << my_out_stream.str() << std::endl;

	clock_t clock_debut = clock(), clock_local = clock();
	for (unsigned int current_iter=0; current_iter <= BuildingsDetectorParametersSingleton::Instance()->NbIterations(); ++current_iter)
	{
		if (current_iter % BuildingsDetectorParametersSingleton::Instance()->NbIterationsDump() == 0)
		{
			my_out_stream.str("");
			my_out_stream << "\t" << current_iter;
			my_out_stream << "\t" << buildingsDetector.GetNbVertices();
			my_out_stream << "\t" << double(clock()- clock_local) / CLOCKS_PER_SEC;
			my_out_stream << "\t" << sampler.Temperature();
			my_out_stream << "\t" << buildingsDetector.DataEnergy();
			my_out_stream << "\t" << buildingsDetector.PriorEnergy();
			my_out_stream << "\t" << buildingsDetector.DataEnergy() + buildingsDetector.PriorEnergy() << std::endl;
			std::cout << my_out_stream.str();
			clock_local = clock();
		}
		sampler.Itere(buildingsDetector);
	}
	clock_t clock_fin = clock();

	my_out_stream.str("");
	my_out_stream << "Iterations finished" << std::endl;
	double elapsed = double(clock_fin - clock_debut) / CLOCKS_PER_SEC;
	my_out_stream << "Total elapsed time (s) :  " << elapsed << std::endl;
	my_out_stream << "Mean time per iteration (ms) :  " << elapsed * 1000 / BuildingsDetectorParametersSingleton::Instance()->NbIterations() << std::endl;
	my_out_stream << "\t" << buildingsDetector.GetNbVertices();
	my_out_stream << "\t" << sampler.Temperature();
	my_out_stream << "\t" << buildingsDetector.DataEnergy();
	my_out_stream << "\t" << buildingsDetector.PriorEnergy();
	my_out_stream << "\t" << buildingsDetector.DataEnergy() + buildingsDetector.PriorEnergy() << std::endl;
	my_out_stream << "Graph Data energy integrity : " << buildingsDetector.CheckGraphDataEnergyIntegrity() << std::endl;
	my_out_stream << "Graph Prior energy integrity: " << buildingsDetector.CheckGraphPriorEnergyIntegrity() << std::endl;
	my_out_stream << "Graph Structure integrity : " << buildingsDetector.CheckGraphStructureIntegrity() << std::endl;
	buildingsDetector.Dump(my_out_stream);
	std::cout << my_out_stream.str();
	//buildingsDetector.InitExport();
	//BuildingsDetector::vertex_iterator it_v = vertices(buildingsDetector.GetGraph()).first, fin_v = vertices(buildingsDetector.GetGraph()).second;
	//for (; it_v != fin_v; ++it_v)
	//	buildingsDetector.ExportNode(buildingsDetector.GetGraph()[*it_v]);
	//buildingsDetector.EndExport("final_out.tif");
	return 0;
}
*/
