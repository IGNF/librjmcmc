/***********************************************************************
This file is part of the librjmcmc project source files.

Copyright : Institut Geographique National (2008-2012)
Contributors : Mathieu Brédif, Olivier Tournaire, Didier Boldo
email : librjmcmc@ign.fr

This software is a generic C++ library for stochastic optimization.

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software. You can use,
modify and/or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty and the software's author, the holder of the
economic rights, and the successive licensors have only limited liability.

In this respect, the user's attention is drawn to the risks associated
with loading, using, modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean that it is complicated to manipulate, and that also
therefore means that it is reserved for developers and experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and, more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

***********************************************************************/

#ifndef GEOMETRY_RECTANGLE_2_POINT_ITERATOR_HPP
#define GEOMETRY_RECTANGLE_2_POINT_ITERATOR_HPP

namespace geometry {

// énumération des points de coordonnées entières contenus dans le rectangle

template<class Rect2> class Rectangle_2_point_iterator {
  typedef Rect2                      Rectangle_2;
  typedef typename Rectangle_2::R    R;
  typedef typename R::FT             FT;
  typedef typename R::Point_2        Point_2;

public:
  Rectangle_2_point_iterator () {}
  Rectangle_2_point_iterator (const Rectangle_2& rec) {
    int b = rec.bottom_vertex(), t = b+2;
    int l = rec.  left_vertex(), r = l+2;
    Point_2 p[] = { rec[b],rec[l],rec[r],rec[t] };
    y_ = (int) std::ceil(to_double(p[0].y()));
    ly = (int) std::ceil(to_double(p[1].y()));
    ry = (int) std::ceil(to_double(p[2].y()));
    ty = (int) std::ceil(to_double(p[3].y()));
    if(rec.is_iso_rectangle()) {
	dxl[0]=dxl[1]=dxr[0]=dxr[1]=0;

	xl[0] = xl[1] = p[1].x();
	xr[0] = xr[1] = p[2].x();
    } else {
	dxl[0]=(p[1].x()-p[0].x())/(p[1].y()-p[0].y());
	dxl[1]=(p[3].x()-p[1].x())/(p[3].y()-p[1].y());
	dxr[0]=(p[2].x()-p[0].x())/(p[2].y()-p[0].y());
	dxr[1]=(p[3].x()-p[2].x())/(p[3].y()-p[2].y());

	xl[0] = p[0].x()+(y_-p[0].y())*dxl[0];
	xl[1] = p[1].x()+(ly-p[1].y())*dxl[1];
	xr[0] = p[0].x()+(y_-p[0].y())*dxr[0];
	xr[1] = p[2].x()+(ry-p[2].y())*dxr[1];
    }
    x_=rx=0; --y_;
    operator++();
  }
  Rectangle_2_point_iterator operator++( int ) { Rectangle_2_point_iterator tmp(*this); operator++(); return tmp; }
  Rectangle_2_point_iterator& operator++() {
    ++x_;
    while(x_>rx && y_<ty) {
    	++y_;
        int i=(y_>=ly);	x_ = (int) std::ceil (xl[i]); xl[i]+=dxl[i];
        int j=(y_>=ry);	rx = (int) std::floor(xr[j]); xr[j]+=dxr[j];
    }
    return *this;
  }
  inline bool end() const { return y_>=ty; }
  inline int x() const { return x_; }
  inline int y() const { return y_; }

private :
  int x_, y_, rx, ly, ry, ty;
  FT xl[2], xr[2], dxl[2], dxr[2];
};

/*
// non débugé : énumération des pixels carrés intersectant au moins partiellement le rectangle

template<class Rect2> class Rectangle_2_square_iterator {
  typedef Rect2                      Rectangle_2;
  typedef typename Rectangle_2::R    R;
  typedef typename R::Segment_2      Segment_2;
  typedef Segment_2_iterator<Segment_2>  Iterator;
  typedef typename R::Point_2        Point_2;

public:
  Rectangle_2_square_iterator (const Rectangle_2& rec) {
    int b = rec.bottom_vertex(), t = b+2;
    int l = rec.  left_vertex(), r = l+2;
    Point_2 p[] = { rec[b],rec[l],rec[r],rec[t] };
    il[0] = Iterator(Segment_2(p[0],p[1]));
    il[1] = Iterator(Segment_2(p[1],p[3]));
    ir[0] = Iterator(Segment_2(p[0],p[2]));
    ir[1] = Iterator(Segment_2(p[2],p[3]));
    x_ = (int) std::floor(to_double(p[0].x()));
    y_ = (int) std::floor(to_double(p[0].y()));
    operator++();
  }
  Rectangle_2_square_iterator operator++( int ) { Rectangle_2_square_iterator tmp(*this); operator++(); return tmp; }
  Rectangle_2_square_iterator& operator++() {
    ++x_;
    while(!end() && (x_>=ir[0].x() || x_>=ir[1].x() ) ) {
	while(!il[0].end() && y_==il[0].y()  ) ++(il[0]);
	while(!ir[0].end() && y_==ir[0].y()  ) ++(ir[0]);
	while(!il[1].end() && y_+1>=il[1].y()) ++(il[1]);
	while(!ir[1].end() && y_+1>=ir[1].y()) ++(ir[1]);
	++y_;
	x_=max(il[0].x(),il[1].x())+1;
    }
    return *this;
  }
  inline bool end() const { return il[1].end() || ir[1].end(); }
  inline int x() const { return x_; }
  inline int y() const { return y_; }

private :
  int x_, y_;
  Iterator il[2], ir[2];
};
*/

}; // namespace geometry

#endif // GEOMETRY_RECTANGLE_ITERATOR_2_HPP
