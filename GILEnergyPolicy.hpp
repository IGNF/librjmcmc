#include "stdafx.h"

#include "cgal_types.h"
#include "RectangleNode.hpp"
#include "CircleNode.hpp"

class GILPolicyImage
{
public:


	void LoadFile(const std::string &str);

	double ComputeDataEnergy(const RectangleNode &n) const;
	double ComputeSegmentDataEnergy(const Point_2 &gridIn,const Point_2 &gridOut) const;

	double ComputeDataEnergy(const CircleNode &n) const;
	void Add8CirclePoints(int xCenter, int yCenter, int dx, int dy, double & res) const;

	dev2n32F_image_t m_gradients;
	unsigned int m_imageWidth;
	unsigned int m_imageHeight;

	double m_defaultEnergy;

	void InitExport() const;
	void Export8Points(int xCenter, int yCenter, int dx, int dy) const;
	void ExportNode(const CircleNode &n) const;
	void EndExport(const char *filename) const;
};
