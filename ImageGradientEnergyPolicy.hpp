#include <boost/shared_ptr.hpp>

#include "cgal_types.h"
#include "GeometricNode.hpp"

class GILPolicyImage;

class ImageGradientEnergyPolicy
{
public :

	ImageGradientEnergyPolicy();
	~ImageGradientEnergyPolicy();

	double ComputeDataEnergy(const Rectangle_2 &n) const;
	double ComputeDataEnergy(const Cercle_2 &n) const;

	template<class NodeGeometry>
	inline double ComputeDataEnergy(const GeometricNode<NodeGeometry> &n) const
	{
		return ComputeDataEnergy(n.Geometry());
	}

private:
	boost::shared_ptr<GILPolicyImage> m_policy;
};
