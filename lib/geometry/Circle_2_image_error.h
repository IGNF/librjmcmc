// Author(s)     : Mathieu Bredif

#ifndef GEOMETRY_CIRCLE_2_IMAGE_ERROR_H
#define GEOMETRY_CIRCLE_2_IMAGE_ERROR_H

#include "Circle_2.h"

template<typename OrientedView, typename K>
double image_error(const OrientedView& v, const geometry::Circle_2<K> &c) {
	typedef typename OrientedView::view_t view_t;
    view_t v(v.view());
	int x0 = v.x0();
	int y0 = v.y0();

	double r = geometry::radius(c);
	double x = c.center().x()-x0;
	double y = c.center().y()-y0;
	int i0 = std::max(0,(int) (x-r));
	int i1 = std::min((int) (v.width()-1),(int) (x+r+1));
	int j0 = std::max(0,(int) (y-r));
	int j1 = std::min((int) (v.height()-1),(int) (y+r+1));
	double res = 0;
	for ( int i = i0 ; i<=i1 ; ++i)
	{
		for ( int j = j0 ; j<=j1 ; ++j)
		{
			if ((x-i)*(x-i)+(y-j)*(y-j)>r*r) continue;
			double ndvi = v(i,j);
			res +=(140-ndvi) ;
		}
	}
		
	return res;
}

#endif // GEOMETRY_CIRCLE_2_IMAGE_ERROR_H

