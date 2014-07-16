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

#ifndef GEOMETRY_ISO_RECTANGLE_2_SEGMENT_2_CLIP_HPP
#define GEOMETRY_ISO_RECTANGLE_2_SEGMENT_2_CLIP_HPP

template<typename IsoRectangle, typename Segment>
bool clip(const IsoRectangle& r, Segment& s)
{
    typedef typename Segment::R R;
    typedef typename R::FT FT;
    typedef typename R::Point_2 Point;
    typedef typename R::Vector_2 Vector;

    const Point& p(s.source());
    const Point& q(s.target());
    Vector d(q-p);
    FT a[] = { p.x(), p.y() };
    FT b[] = { q.x(), q.y() };
    FT tmin = 0, tmax = 1;
    for(int i=0; i<2; ++i) {
      if(d[i]==0 && (p[i]<r.min()[i] || p[i]>r.max()[i])) {
        return false;
      } else {
        int j = 1-i, k = (d[i] > 0);
        int l = 1-k;
	FT m[] = {r.min()[i], r.max()[i]}, t;
        t = (m[l]-p[i])/d[i];
        if(t>tmin) { tmin=t; a[i]=m[l];  a[j]=p[j]+t*d[j]; } // a=p+td
        t = (m[k]-p[i])/d[i];
        if(t<tmax) { tmax=t; b[i]=m[k];  b[j]=p[j]+t*d[j]; } // b=p+td
        if(tmax <= tmin) return false;
      }
    }
    Point pa(a[0],a[1]);
    Point pb(b[0],b[1]);
    s = Segment(pa,pb);
    return true;
}

#endif // GEOMETRY_ISO_RECTANGLE_2_SEGMENT_2_CLIP_HPP
