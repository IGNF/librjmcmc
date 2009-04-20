#ifndef GILENERGYPOLICY_HPP
#define GILENERGYPOLICY_HPP

#include <boost/shared_ptr.hpp>

#include "cgal_types.h"
#include "GeometricNode.hpp"

class GILEnergyPolicy
{
public:
	GILEnergyPolicy();

	void LoadFile(const std::string &str);

	double ComputeDataEnergy(const Rectangle_2 &n) const;
	double ComputeSegmentDataEnergy(const Point_2 &gridIn,const Point_2 &gridOut) const;

	double ComputeDataEnergy(const Cercle_2 &n) const;
	void Add8CirclePoints(float xCenter, float yCenter, float dx, float dy, double & res) const;

	template<class NodeGeometry>
	inline double ComputeDataEnergy(const GeometricNode<NodeGeometry> &n) const
	{
		return ComputeDataEnergy(n.Geometry());
	}

private :
	double m_defaultEnergy;

	struct gradients_image_t;
	boost::shared_ptr<gradients_image_t> m_gradients;
};


class ImageExporter
{
public :
	ImageExporter();

	void InitExport(const char *filename) const;
	void EndExport(const char *filename) const;
	void Export8Points(int xCenter, int yCenter, int dx, int dy) const;
	void ExportNode(const Cercle_2 &n) const;
	void ExportNode(const Segment_2 &s) const;
	void ExportNode(const Rectangle_2 &n) const;

private :
	struct export_image_t;
	boost::shared_ptr<export_image_t> m_img;
};

#endif //#ifndef GILENERGYPOLICY_HPP
