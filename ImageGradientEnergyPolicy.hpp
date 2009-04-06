#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>

class GILPolicyImage;
class RectangleNode;
class CircleNode;


class ImageGradientEnergyPolicy
{
public :

	ImageGradientEnergyPolicy();
	~ImageGradientEnergyPolicy();
	double ComputeDataEnergy(const RectangleNode &n) const;
	double ComputeDataEnergy(const CircleNode &n) const;

	void InitExport() const;
	void ExportNode(const CircleNode &n) const;
	void EndExport(const char *filename) const;

private:
	boost::shared_ptr<GILPolicyImage> m_policy;
};
