#include <boost/gil/image.hpp>
#include <boost/gil/extension/matis/float_images.hpp>

using namespace boost::gil;

typedef pixel<float,devicen_layout_t<2> >		dev2n32F_pixel_t;
typedef dev2n32F_pixel_t*						dev2n32F_pixel_ptr_t;
typedef image_type<float,devicen_layout_t<2> >::type	dev2n32F_image_t;
typedef dev2n32F_image_t::view_t				dev2n32F_view_t;

#include "cgal_types.h"

class GILPolicyImage
{
public:
	void LoadFile(const std::string &str);

	double ComputeDataEnergy(const Rectangle_2 &n) const;
	double ComputeSegmentDataEnergy(const Point_2 &gridIn,const Point_2 &gridOut) const;

	double ComputeDataEnergy(const Cercle_2 &n) const;
	void Add8CirclePoints(int xCenter, int yCenter, int dx, int dy, double & res) const;

	dev2n32F_image_t m_gradients;
	unsigned int m_imageWidth;
	unsigned int m_imageHeight;

	double m_defaultEnergy;

	void InitExport() const;
	void Export8Points(int xCenter, int yCenter, int dx, int dy) const;
	void ExportNode(const Cercle_2 &n) const;
	void EndExport(const char *filename) const;
};
