#ifndef BOX_HPP_
#define BOX_HPP_

#include <boost/array.hpp>

template<unsigned int Dimension=2> 
class MultiDimensionnalBBox
{
public :
	typedef boost::array<float,Dimension> PointType;

	MultiDimensionnalBBox(){}
	MultiDimensionnalBBox(const PointType& size , const PointType& origin)
	{
		m_volume = 1.;
		for(unsigned int i=0;i<Dimension;++i)
		{
			m_origin[i] = origin[i];
			m_max[i] = m_origin[i] + size[i];
			m_volume *= size[i];
		}
	}

	const PointType &Origin() const { return m_origin; }
	//void Origin(const PointType& origin) { m_origin = origin; }
	const PointType &Min() const { return m_origin; }
	//void Min(const PointType& origin) { m_origin = origin; }
	const PointType &Max() const { return m_max; }

	bool IsInside(const PointType& point) const
	{
		typename PointType::const_iterator itbegin = m_origin.begin(), itend = m_origin.end();
		typename PointType::const_iterator it2begin = m_max.begin();
		typename PointType::const_iterator itpbegin = point.begin();
		for(;itbegin!=itend;++itbegin,++it2begin,++itpbegin)
		{
			if( *itpbegin < *itbegin || *itpbegin > *it2begin )
				return false;
		}
		return true;
	}

	const float Volume() const { return m_volume; }

private:
	PointType m_origin;
	PointType m_max;
	float m_volume;
};

typedef MultiDimensionnalBBox<> BBox;

#endif //#ifndef BOX_HPP_
