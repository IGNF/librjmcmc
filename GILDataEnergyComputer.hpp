#ifndef GILDATAENERGYCOMPUTER_HPP
#define GILDATAENERGYCOMPUTER_HPP

#include <boost/shared_ptr.hpp>

#include "cgal_types.h"
#include "GeometricNode.hpp"

class GILDataEnergyComputer
{
public:
	GILDataEnergyComputer(double defaultEnergy, double coefDefaultEnergy, const std::string &nomIm, const std::string &nomMask = "");

	double Compute(const Rectangle_2 &n) const;
	double Compute(const Cercle_2 &n) const;

	template<class NodeGeometry>
	inline double Compute(const GeometricNode<NodeGeometry> &n) const
	{
		return Compute(n.Geometry());
	}

private :
	double ComputeSegmentDataEnergy(const Point_2 &gridIn,const Point_2 &gridOut) const;

	void Add8CirclePoints(double xCenter, double yCenter, double dx, double dy, double radius, double & res) const;
	void Add4CirclePoints(double xCenter, double yCenter, double dy, double radius, double & res) const;
	double Add1CirclePoints(double xCenter, double yCenter, double dx, double dy) const;

	double m_defaultEnergy;
	double m_coefDefaultEnergy;

	struct input_image_t;
	boost::shared_ptr<input_image_t> m_img;
	struct gradients_image_t;
	boost::shared_ptr<gradients_image_t> m_gradients_cercle;
	boost::shared_ptr<gradients_image_t> m_gradients_rectangle;
};


class ImageExporter
{
public :
	ImageExporter(bool fill=false);

	void InitExport(const char *filename) const;
	void InitExport(int width, int height) const;

	void EndExport(const char *filename) const;
	void Export8Points(int xCenter, int yCenter, int dx, int dy) const;
	void ExportNode(const Cercle_2 &n) const;
	void ExportNode(const Rectangle_2 &n) const;

private :
	bool m_fill;
	struct export_image_t;
	boost::shared_ptr<export_image_t> m_img;
};

#endif //#ifndef GILDATAENERGYCOMPUTER_HPP
