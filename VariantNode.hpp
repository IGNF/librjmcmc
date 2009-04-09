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

		while (!IsValid(box))
		{
			m_geometry = old;
			boost::apply_visitor(ModifyVisitor(box), m_geometry);
		}
	}

	void RandomInit(const BBox &box)
	{
		boost::variate_generator<RJMCMCRandom&, boost::uniform_smallint<> > die(GetRandom(), boost::uniform_smallint<>(0,1));
		if (die())
			m_geometry = Cercle_2();
		else
			m_geometry = Rectangle_2();

		do
		{
			boost::apply_visitor(InitVisitor(box), m_geometry);
		}
		while (!IsValid(box));
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
class VariantGradientDataEnergy : public ImageGradientEnergyPolicy, public boost::static_visitor<double>
{
	VariantImageExporter m_exporter;

public :
	inline double ComputeDataEnergy(VariantNode<NodeGeometry> const  &n) const
    {
		return 	boost::apply_visitor(*this, n.Geometry());
    }

	inline double operator()(const Rectangle_2 &r) const
    {
		return ImageGradientEnergyPolicy::ComputeDataEnergy(r);
    }

    inline double operator()(const Cercle_2 &c) const
    {
		return ImageGradientEnergyPolicy::ComputeDataEnergy(c);
    }


	void InitExport(const char *filename) const { m_exporter.InitExport(filename); }

	void ExportNode(const NodeGeometry &n) const
	{
		boost::apply_visitor(m_exporter, n);
	}

	void EndExport(const char *filename) const  
	{
		m_exporter.EndExport(filename);
	}
};


template<class VariantNodeGeometry>
class VariantIntersectionPriorEnergy : public boost::static_visitor<double>
{
	double m_coefSurface;
public :
	inline VariantIntersectionPriorEnergy() : m_coefSurface(BuildingsDetectorParametersSingleton::Instance()->IntersectionSurfacePonderation()) {}

	inline double ComputePriorEnergy(VariantNode<VariantNodeGeometry> const  &n1, VariantNode<VariantNodeGeometry> const  &n2) const
	{
		return 	boost::apply_visitor(*this, n1.Geometry(), n2.Geometry());
	}

	template<class NodeGeometry>
    inline double operator()( const NodeGeometry &n1, const NodeGeometry &n2 ) const
    {
        if (n1.do_intersect(n2))
        { 
			return m_coefSurface * n1.intersection_area(n2);
		}
		return 0.;
    }

	inline double operator()( const Cercle_2 &n1, const Rectangle_2 &n2  ) const
    {
		if (squared_distance(n2, n1.center()) < n1.squared_radius())
		{
			return m_coefSurface * std::min(n1.area(), n2.area()); 
		}
		return 0.;
    }


    inline double operator()(const Rectangle_2 &n1, const Cercle_2 &n2) const
    {
        return (*this)(n2, n1); 
    }
};
