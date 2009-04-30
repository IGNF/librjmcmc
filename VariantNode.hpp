#include "GeometricNode.hpp"
#include <boost/variant.hpp>

class InitVisitor
{
	const BBox & m_bbox;
public:
	typedef void result_type;

	InitVisitor(const BBox &bbox) : m_bbox(bbox){}

	result_type operator()( Rectangle_2 & n) const
	{
		::RandomInit(m_bbox, n);
	}

    result_type operator()( Cercle_2 & n) const
	{
		::RandomInit(m_bbox, n);
	}
};

class ModifyVisitor
{
	const BBox & m_bbox;
public:
	typedef void result_type;

	ModifyVisitor(const BBox &bbox) : m_bbox(bbox){}

	result_type operator()( Rectangle_2 & n) const
	{
		::RandomModify(m_bbox, n);
	}

    result_type operator()( Cercle_2 & n) const
	{
		::RandomModify(m_bbox, n);
	}
};

class IsValidVisitor
{
	const BBox & m_bbox;
public:
	typedef bool result_type;

	IsValidVisitor(const BBox &bbox) : m_bbox(bbox){}

	result_type operator()(const Rectangle_2 & n) const
	{
		return ::IsValid(m_bbox, n);
	}

    result_type operator()(const Cercle_2 & n) const
	{
		return ::IsValid(m_bbox, n);
	}
};

class SurfaceVisitor
{
public :
	typedef double result_type;
	
	SurfaceVisitor() {}
	
	result_type operator()(const Rectangle_2 & n) const
	{
		return n.area();
	}

    result_type operator()(const Cercle_2 & n) const
	{
		return n.area();
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

	double Surface() const 
	{
		return boost::apply_visitor(SurfaceVisitor(), Geometry());
	}

	const NodeGeometry & Geometry() const { return m_geometry; }

private :
	bool IsValid(const BBox &box)
	{
		return boost::apply_visitor(IsValidVisitor(box), Geometry());
	}
	NodeGeometry m_geometry;
	double m_weight;
};

template<class VariantNodeGeometry>
class VariantImageExporter : public ImageExporter
{
public :
	typedef void result_type;

	void ExportNode(const VariantNodeGeometry &n) const
	{
		boost::apply_visitor(*this, n);
	}
	
	result_type operator()(const Rectangle_2 &r) const
    {
		ImageExporter::ExportNode(r);
	}

	result_type operator()(const Cercle_2 &c) const
    {
		ImageExporter::ExportNode(c);
	}
};

template<class VariantNodeGeometry>
class VariantGradientDataEnergy : public GILEnergyPolicy
{
public :
	typedef double result_type;

	inline double ComputeDataEnergy(VariantNode<VariantNodeGeometry> const  &n) const
    {
		return 	boost::apply_visitor(*this, n.Geometry());
    }

	inline result_type operator()(const Rectangle_2 &r) const
    {
		return GILEnergyPolicy::ComputeDataEnergy(r);
    }

    inline result_type operator()(const Cercle_2 &c) const
    {
		return GILEnergyPolicy::ComputeDataEnergy(c);
    }
};


template<class VariantNodeGeometry>
class VariantIntersectionPriorEnergy
{
	double m_coefSurface;
public :
	typedef double result_type;

	inline VariantIntersectionPriorEnergy() : m_coefSurface(BuildingsDetectorParametersSingleton::Instance()->IntersectionSurfacePonderation()) {}

	inline result_type ComputePriorEnergy(VariantNode<VariantNodeGeometry> const  &n1, VariantNode<VariantNodeGeometry> const  &n2) const
	{
		return 	boost::apply_visitor(*this, n1.Geometry(), n2.Geometry());
	}

	template<class NodeGeometry>
    inline result_type operator()( const NodeGeometry &n1, const NodeGeometry &n2 ) const
    {
        if (n1.do_intersect(n2))
        { 
			return m_coefSurface * n1.intersection_area(n2);
		}
		return 0.;
    }

	inline result_type operator()( const Cercle_2 &n1, const Rectangle_2 &n2  ) const
    {
		if (squared_distance(n2, n1.center()) < n1.squared_radius())
		{
			return m_coefSurface * std::min(n1.area(), n2.area()); 
		}
		return 0.;
    }


    inline result_type operator()(const Rectangle_2 &n1, const Cercle_2 &n2) const
    {
        return (*this)(n2, n1); 
    }
};
