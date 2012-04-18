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

#ifndef GEOMETRY_RECTANGLE_2_INTERSECTION_HPP
#define GEOMETRY_RECTANGLE_2_INTERSECTION_HPP

#include "geometry/Rectangle_2.hpp"

#ifdef CGAL_RECTANGLE_2_COMPILE_BOOLEAN_INTERSECTION
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Boolean_set_operations_2.h>
#endif // CGAL_RECTANGLE_2_COMPILE_BOOLEAN_INTERSECTION

namespace geometry {

namespace Impl {
// intersection area between rectangle [-n2,n2]x[-m2,m2] and triangle (tx,ry),(rx,ry),(tx,ty)
template <class FT> FT triangle_area(FT n2, FT m2, FT tx, FT ty, FT rx, FT ry) {
    if(tx>=n2 || ry>=m2) return 0; // no intersection, above
    if(tx<-n2) { ty = ty+(tx+n2)*(ry-ty)/(tx-rx); tx=-n2; } // crop left
    if(ry<-m2) { rx = rx+(ry+m2)*(rx-tx)/(ty-ry); ry=-m2; } // crop bottom
    if(ty<ry) return 0;  // no intersection, underneath
    if(ty<=m2) {
        if(rx<=n2) return (ty-ry)*(rx-tx)/2; // all inside
        return (1+(rx-n2)/(rx-tx))*(ty-ry)*(n2-tx)/2; // cut right
    }
    if(rx<=n2) return (1+(ty-m2)/(ty-ry))*(rx-tx)*(m2-ry)/2; // cut top
    FT mx = tx+(m2-ty)*(rx-tx)/(ry-ty);
    if(mx>=n2) return (n2-tx)*(m2-ry);  // rectangle
    FT ny = ty+(tx-n2)*(ry-ty)/(tx-rx);
    return (mx-tx)*(m2-ry)+((m2+ny)/2-ry)*(n2-mx); // rectangle with the upper right corner cut
}

//alternative implementations

#ifdef CGAL_RECTANGLE_2_COMPILE_BOOLEAN_INTERSECTION
  template<class K> Object intersection_unoptimized(const Rectangle_2<K> &a, const Rectangle_2<K> &b) {
    typedef K::Point_2                        Point_2;
    typedef std::vector<Point_2>              Container;
    typedef Polygon_2<K, Container>           Polygon_2;
    typedef Polygon_with_holes_2<K,Container> Polygon_with_holes_2;

    if(a.is_degenerate() || b.is_degenerate()) return Object();
    Container pa(a.center()ontainer(true)), pb(b.center()ontainer(true));
    std::vector<Polygon_with_holes_2> res;
    Polygon_2 polya(pa.begin(),pa.end()), polyb(pb.begin(),pb.end());
    intersection(polya,polyb,std::back_inserter(res));
    return (res.empty())?Object():make_object(res.back().outer_boundary().container());
  }
#endif // CGAL_RECTANGLE_2_COMPILE_BOOLEAN_INTERSECTION


/* Intersection area */
  template<class K> typename K::FT intersection_area_two_slabs(const Rectangle_2<K>& a, const Rectangle_2<K> &b) {
    typedef typename K::Vector_2 Vector_2;
    typedef typename K::Line_2 Line_2;
    typedef typename K::Point_2 Point_2;
    typedef typename K::FT FT;
    Origin O;
    if(a.is_degenerate() || b.is_degenerate()) return 0;
    std::vector<Vector_2> v;
    std::vector<Line_2> l;
    for(unsigned int i=0; i<4; ++i)    { v.push_back(b[i]-O); l.push_back(b.line(i)); }
    const Vector_2& n = a.normal();
    FT r = a.ratio();
    FT n2 = n.squared_length();
    FT rn2 = r*r*n2;
    Vector_2 vc(a.center()-O);
    Vector_2 trn(-r*n.y(),r*n.x());
    FT ctrn = vc*trn;
    FT cn = vc*n;

    Vector_2 ln[] = {  n, trn };
    FT lc[] = { cn+n2, ctrn+rn2, cn-n2, ctrn-rn2 };

 // basically it is the iterative intersection of the convex polygon initialized with b
 // with the 2 slabs line0/line2 and line1/line3.
    for(unsigned int i=0; i<2; ++i)    {
        int begin0=-1, end0=-1;
        int begin1=-1, end1=-1;
        unsigned int n=v.size();
        for(unsigned int j=0; j<n; ++j) {
            FT dot = ln[i]*v[j];
            if( dot>lc[i]) {
                if(end0==-1 && begin0!=-1) end0=j;
                if(begin1<=end1) begin1=j;
            } else {
                if(begin0<=end0) begin0=j;
                if(dot<lc[i+2]) {
                    if(end1==-1 && begin1!=-1) end1=j;
                } else {
                    if(begin1<=end1) begin1=j;
                }
            }
        }
        if(begin0==-1 || begin1==-1 ) return 0; // outside the slab
        if(end0  ==-1) {
            if(begin0!=0) end0=0;
            else begin0=begin1;
        }
        if(end1  ==-1) {
            if(begin1!=0) end1=0;
            else {
                if(end0==-1) continue; // inside the slab
                begin1=begin0;
            }
        }

        std::vector<Vector_2> w;
        std::vector<Line_2> m;
        if(end0!=-1) {  // cut outside line(i+1)
            for(int j=begin1; j!=end0; j=(j+1)%n) {
                 w.push_back(v[j]);
                 m.push_back(l[j]);
            }
            Point_2 inter(O);
            Line_2 li(ln[i].x(),ln[i].y(),-lc[i]);
            intersection(l[(end0+n-1)%n],li).assign(inter);
            w.push_back(inter-O);
            m.push_back(li);
            m.push_back(l[(begin0+n-1)%n]);
            intersection(li,m.back()).assign(inter);
            w.push_back(inter-O);
        }
        if(end1!=-1) { // cut outside line(i+3)
            for(int j=begin0; j!=end1; j=(j+1)%n) {
                w.push_back(v[j]);
                m.push_back(l[j]);
            }
            Point_2 inter(O);
            Line_2 li(ln[i].x(),ln[i].y(),-lc[i+2]);
            intersection(l[(end1+n-1)%n],li).assign(inter);
            w.push_back(inter-O);
            m.push_back(li);
            m.push_back(l[(begin1+n-1)%n]);
            intersection(li,m.back()).assign(inter);
            w.push_back(inter-O);
        }
        std::swap(v,w);
        std::swap(l,m);
    }
    std::vector<Point_2> p;
    for(unsigned int i=0; i<v.size(); ++i)    { p.push_back(O+v[i]); }
    FT area;
    area_2(p.begin(),p.end(),area);
    return abs(area);
  }

/*
  FT intersection_area(const Triangle_2 &b) const {
    if(is_degenerate() || b.is_degenerate()) return 0;
    FT n2  = n.squared_length();
    FT m2  = abs(r)*n2;
    Vector_2 m(-n.y(),n.x());
    Vector_2 v[] = { b[0]-c, b[1]-c, b[2]-c };
    FT x[] = { n*v[0], n*v[1], n*v[2] };
    FT y[] = { m*v[0], m*v[1], m*v[2] };
    if(x[0]>x[1]) { std::swap(x[0],x[1]); std::swap(y[0],y[1]); }
    if(x[0]>x[2]) { std::swap(x[0],x[2]); std::swap(y[0],y[2]); }
    if(x[1]>x[2]) { std::swap(x[1],x[2]); std::swap(y[1],y[2]); }
    if(       x[0]>=n2 || -x[2]>=n2 ||
            ( y[0]>=m2 &&  y[1]>=m2 &&  y[2]>=m2) ||
            (-y[0]>=m2 && -y[1]>=m2 && -y[2]>=m2) ) return 0;
    //TODO
  }
*/


} // namespace Impl

/****************************************/
/* intersection      free function      */
/****************************************/
/*
  template<class K> Object intersection(const Rectangle_2<K>& a, const typename K::Line_2 &b) {
    typedef typename K::Vector_2 Vector_2;
    typedef typename K::Segment_2 Segment_2;
    typedef typename K::Point_2 Point_2;
    typedef typename K::FT FT;

    Vector_2 m(-a.ratio()*a.normal().y(),a.ratio()*a.normal().x());
    FT dc(b.a()*a.center().x()+b.b()*a.center().y()+b.center()());
    FT dn(b.a()*a.normal().x()+b.b()*a.normal().y());
    FT dm(b.a()*m.x()+b.b()*m.y());
    if(dm==0) {
        if(dc*dc>dn*dn) return Object();
        Point_2 p(a.center()-(dc/dn)*a.normal());
        return make_object(Segment_2(p-m,p+m));
    }
    if(dn==0) {
        if(dc*dc>dm*dm) return Object();
        Point_2 p(a.center()-(dc/dm)*m);
        return make_object(Segment_2(p-a.normal(),p+a.normal()));
    }
    if(dc) { dc=-dc; dn=-dn; dm=-dm; }
    FT diff(dc-abs(dm)-abs(dn));
    if(diff >0) return Object();
    int i = (dm>0)?((dn>0)?2:3):((dn>0)?1:0);
    if(diff==0) return make_object(a.point(i));
    FT dist[] = { dc-dn-dm, dc+dn-dm, dc+dn+dm, dc-dn+dm };
    // dist[i]>0, dist[(i+2)%4]<0
    Point_2 p(ORIGIN), q(ORIGIN);
    bool ok = true;
    if(dist[(i+1)%4]<0) { // inter line(i),b
        ok = ok && intersection(a.line(i  ),b).assign(p);
    } else { // inter line(i+1),b
        ok = ok && intersection(a.line(i+1),b).assign(p);
    }
    if(dist[(i+3)%4]>0) { // inter line(i+2),b
        ok = ok && intersection(a.line(i+2),b).assign(p);
    } else { // inter line(i+3),b
        ok = ok && intersection(a.line(i+3),b).assign(p);
    }
    return ok?Object():make_object(Segment_2(p,q));
  }

  template<class K> Object intersection(const Rectangle_2<K>& a, const typename K::Segment_2 &b) {
    typedef typename K::Segment_2 Segment_2;
    typedef typename K::Point_2 Point_2;
    Object obj(intersection(a,b.supporting_line()));
    if (const Segment_2 *s = object_cast<Segment_2>(&obj)) return intersection(b,*s);
    if (const Point_2 *p = object_cast<Point_2>(&obj) && b.has_on(*p)) return obj;
    return Object();
  }
  template<class K> Object intersection(const Rectangle_2<K>& a, const typename K::Ray_2 &b) {
    typedef typename K::Segment_2 Segment_2;
    typedef typename K::Point_2 Point_2;
    Object obj(intersection(a,b.supporting_line()));
    if (const Segment_2 *s = object_cast<Segment_2>(&obj)) return intersection(b,*s);
    if (const Point_2 *p = object_cast<Point_2>(&obj)) return (b.has_on(*p))?obj:Object();
    return Object();
  }

  template<class K> Object intersection(const Rectangle_2<K>& a, const typename K::Triangle_2 &b) {
    typedef typename K::Line_2 Line_2;
    typedef typename K::Vector_2 Vector_2;
    typedef typename K::Point_2 Point_2;
    typedef typename K::FT FT;

    if(a.is_degenerate()) return Object();
    std::vector<Vector_2> v;
    std::vector<Line_2> l;
    for(unsigned int i=0; i<3; ++i)    { v.push_back(b[i]-ORIGIN); l.push_back(Line_2(b[i],b[i+1])); }
    FT n2 = a.normal().squared_length();
    FT rn2 = a.ratio()*a.ratio()*n2;
    Vector_2 vc(a.center()-ORIGIN);
    Vector_2 trn(-a.ratio()*a.normal().y(),a.ratio()*a.normal().x());
    FT ctrn = vc*trn;
    FT cn = vc*a.normal();

    Vector_2 ln[] = {  a.normal(), trn };
    FT lc[] = { cn+n2, ctrn+rn2, cn-n2, ctrn-rn2 };

 // basically it is the iterative intersection of the convex polygon initialized with b
 // with the 2 slabs line0/line2 and line1/line3.
    for(unsigned int i=0; i<2; ++i)    {
        int begin0=-1, end0=-1;
        int begin1=-1, end1=-1;
        unsigned int n=v.size();
        for(unsigned int j=0; j<n; ++j) {
            FT dot = ln[i]*v[j];
            if( dot>lc[i]) {
                if(begin1<=end1) begin1=j;
                if(end0==-1 && begin0!=-1) end0=j;
            } else {
                if(begin0<=end0) begin0=j;
                if(dot<lc[i+2]) {
                    if(end1==-1 && begin1!=-1) end1=j;
                } else {
                    if(begin1<=end1) begin1=j;
                }
            }
        }
        if(begin0==-1 || begin1==-1 ) return Object(); // outside the slab
        if(end0  ==-1) {
            if(begin0!=0) end0=0;
            else begin0=begin1;
        }
        if(end1  ==-1) {
            if(begin1!=0) end1=0;
            else {
                if(end0==-1) continue; // inside the slab
                begin1=begin0;
            }
        }

        std::vector<Vector_2> w;
        std::vector<Line_2> m;
        if(end0!=-1) {  // cut outside line(i+1)
            for(int j=begin1; j!=end0; j=(j+1)%n) {
                w.push_back(v[j]);
                m.push_back(l[j]);
            }
            Point_2 inter(ORIGIN);
            Line_2 li(ln[i].x(),ln[i].y(),-lc[i]);
            intersection(l[(end0+n-1)%n],li).assign(inter);
            w.push_back(inter-ORIGIN);
            m.push_back(li);
            m.push_back(l[(begin0+n-1)%n]);
            intersection(li,m.back()).assign(inter);
            w.push_back(inter-ORIGIN);
        }
        if(end1!=-1) { // cut outside line(i+3)
            for(int j=begin0; j!=end1; j=(j+1)%n) {
                w.push_back(v[j]);
                m.push_back(l[j]);
            }
            Point_2 inter(ORIGIN);
            Line_2 li(ln[i].x(),ln[i].y(),-lc[i+2]);
            intersection(l[(end1+n-1)%n],li).assign(inter);
            w.push_back(inter-ORIGIN);
            m.push_back(li);
            m.push_back(l[(begin1+n-1)%n]);
            intersection(li,m.back()).assign(inter);
            w.push_back(inter-ORIGIN);
        }
        std::swap(v,w);
        std::swap(l,m);
    }
    std::vector<Point_2> p;
    for(unsigned int i=0; i<v.size(); ++i)    { p.push_back(ORIGIN+v[i]); }
    return make_object(p);
  }

  template<class K> Object intersection(const Rectangle_2<K>& a, const Rectangle_2<K> &b) {
    typedef typename K::Line_2 Line_2;
    typedef typename K::Vector_2 Vector_2;
    typedef typename K::Point_2 Point_2;
    typedef typename K::FT FT;

    if(a.is_degenerate() || b.is_degenerate()) return Object();
    std::vector<Vector_2> v;
    std::vector<Line_2> l;
    for(unsigned int i=0; i<4; ++i)    { v.push_back(b[i]-ORIGIN); l.push_back(b.line(i)); }
    FT n2 = a.normal().squared_length();
    FT rn2 = a.ratio()*a.ratio()*n2;
    Vector_2 vc(a.center()-ORIGIN);
    Vector_2 trn(-a.ratio()*a.normal().y(),a.ratio()*a.normal().x());
    FT ctrn = vc*trn;
    FT cn = vc*a.normal();

    Vector_2 ln[] = {  a.normal(), trn };
    FT lc[] = { cn+n2, ctrn+rn2, cn-n2, ctrn-rn2 };

 // basically it is the iterative intersection of the convex polygon initialized with b
 // with the 2 slabs line0/line2 and line1/line3.
    for(unsigned int i=0; i<2; ++i)    {
        int begin0=-1, end0=-1;
        int begin1=-1, end1=-1;
        unsigned int n=v.size();
        for(unsigned int j=0; j<n; ++j) {
            FT dot = ln[i]*v[j];
            if( dot>lc[i]) {
                if(begin1<=end1) begin1=j;
                if(end0==-1 && begin0!=-1) end0=j;
            } else {
                if(begin0<=end0) begin0=j;
                if(dot<lc[i+2]) {
                    if(end1==-1 && begin1!=-1) end1=j;
                } else {
                    if(begin1<=end1) begin1=j;
                }
            }
        }
        if(begin0==-1 || begin1==-1 ) return Object(); // outside the slab
        if(end0  ==-1) {
            if(begin0!=0) end0=0;
            else begin0=begin1;
        }
        if(end1  ==-1) {
            if(begin1!=0) end1=0;
            else {
                if(end0==-1) continue; // inside the slab
                begin1=begin0;
            }
        }

        std::vector<Vector_2> w;
        std::vector<Line_2> m;
        if(end0!=-1) {  // cut outside line(i+1)
            for(int j=begin1; j!=end0; j=(j+1)%n) {
                w.push_back(v[j]);
                m.push_back(l[j]);
            }
            Point_2 inter(ORIGIN);
            Line_2 li(ln[i].x(),ln[i].y(),-lc[i]);
            intersection(l[(end0+n-1)%n],li).assign(inter);
            w.push_back(inter-ORIGIN);
            m.push_back(li);
            m.push_back(l[(begin0+n-1)%n]);
            intersection(li,m.back()).assign(inter);
            w.push_back(inter-ORIGIN);
        }
        if(end1!=-1) { // cut outside line(i+3)
            for(int j=begin0; j!=end1; j=(j+1)%n) {
                w.push_back(v[j]);
                m.push_back(l[j]);
            }
            Point_2 inter(ORIGIN);
            Line_2 li(ln[i].x(),ln[i].y(),-lc[i+2]);
            intersection(l[(end1+n-1)%n],li).assign(inter);
            w.push_back(inter-ORIGIN);
            m.push_back(li);
            m.push_back(l[(begin1+n-1)%n]);
            intersection(li,m.back()).assign(inter);
            w.push_back(inter-ORIGIN);
        }
        std::swap(v,w);
        std::swap(l,m);
    }
    std::vector<Point_2> p;
    for(unsigned int i=0; i<v.size(); ++i)    { p.push_back(ORIGIN+v[i]); }
    return make_object(p);
  }
*/

  template<class K> typename K::FT intersection_area(const Rectangle_2<K>& a, const Rectangle_2<K> &b) {
    typedef typename K::Vector_2 Vector_2;
    typedef typename K::FT FT;

    if(a.is_degenerate() || b.is_degenerate()) return 0;
        Vector_2 v(b.center()-a.center());
        Vector_2 m(-a.normal().y(),a.normal().x());
        FT n2  = a.normal().squared_length();
        FT m2  = abs(a.ratio())*n2;
    FT br(abs(b.ratio()));
        FT cx = a.normal()*v;
        FT cy = m*v;
        FT nx = a.normal()*b.normal();
        FT ny = m*b.normal();
    switch(sign(nx)) {
    case ZERO : { // m and b.normal() are collinear
        ny = abs(ny);
        FT mx = br*ny;
        FT lx = cx-mx;
        FT rx = cx+mx;
        FT by = cy-ny;
        FT ty = cy+ny;
            if(rx<=-n2 || n2<=lx || ty<=-m2 || m2<=by) return 0;
        return (min(n2,rx)-max(-n2,lx))*(min(m2,ty)-max(-m2,by))/n2;
        }
    case NEGATIVE : // b.normal() =rotate180(b.normal())
        { nx = -nx; ny = -ny; }
    default : ;
    }
        FT mx =-br*ny;
        FT my = br*nx;
    switch(sign(ny)) {
    case ZERO : { // n and b.normal() are collinear
        FT lx = cx-nx;
        FT rx = cx+nx;
        FT by = cy-my;
        FT ty = cy+my;
            if(rx<=-n2 || n2<=lx || ty<=-m2 || m2<=by) return 0;
        return (min(n2,rx)-max(-n2,lx))*(min(m2,ty)-max(-m2,by))/n2;
        }
    case NEGATIVE : // b.normal() =rotate90(b.normal())
        nx = -nx; std::swap(nx,mx);
        ny = -ny; std::swap(ny,my);
    default : { // nx>0, ny>0, mx<0 and my>0 case
        FT sumx(nx+mx), sumy(ny+my);
        FT difx(nx-mx), dify(ny-my);
        FT x[] = { cx-sumx, cx+difx, cx+sumx, cx-difx }; // bottom, right, top, left
        FT y[] = { cy-sumy, cy+dify, cy+sumy, cy-dify };
            if(y[0]>=m2 || y[2]<=-m2 || x[3]>=n2 || x[1]<=-n2) return 0; // one edge of "this" separates the 2 rectangles
        FT area =  Impl::triangle_area(n2, m2, x[2], y[2], x[1], y[1])
            +  Impl::triangle_area(m2, n2, y[3],-x[3], y[2],-x[2])
            +  Impl::triangle_area(n2, m2,-x[0],-y[0],-x[3],-y[3])
            +  Impl::triangle_area(m2, n2,-y[1], x[1],-y[0], x[0]);
        // iso-rectangle area
        FT lx = x[0], by = y[1];
        FT rx = x[2], ty = y[3];
        FT s  = 1;
        if(lx>rx) { std::swap(lx,rx); s=-s; }
        if(by>ty) { std::swap(by,ty); s=-s; }
        if(by>=m2 || ty<=-m2 || lx>=n2 || rx<=-n2) return area/n2;
        return (area + s*(min(n2,rx)-max(-n2,lx))*(min(m2,ty)-max(-m2,by)))/n2;
    }
    }

}
/*
template <class K> inline Object intersection(const typename K::Iso_rectangle_2 &s, const Rectangle_2<K> &r) {
  return intersection(r,s);
}
template <class K> inline Object intersection(const typename K::Triangle_2 &s, const Rectangle_2<K> &r) {
  return intersection(r,s);
}
template <class K> inline Object intersection(const typename K::Segment_2 &s, const Rectangle_2<K> &r) {
  return intersection(r,s);
}
template <class K> inline Object intersection(const typename K::Line_2 &s, const Rectangle_2<K> &r) {
  return intersection(r,s);
}
template <class K> inline Object intersection(const typename K::Ray_2 &s, const Rectangle_2<K> &r) {
  return intersection(r,s);
}
*/

/****************************************/
/* intersection_area free function      */
/****************************************/

/*
template <class K> typename K::FT area(const Object &obj, const K& k) {
  typedef typename K::FT                FT;
  typedef typename K::Point_2           Point_2;
  typedef typename K::Iso_rectangle_2   Iso_rectangle_2;
  typedef typename K::Triangle_2        Triangle_2;
  typedef std::vector<Point_2>          Point_2_vector;
//  typedef Polygon_2<K,Point_2_vector>   Polygon_2;
  if (const Iso_rectangle_2 * r = object_cast<Iso_rectangle_2>(&obj)) return r->area();
  if (const Triangle_2      * t = object_cast<Triangle_2>     (&obj)) return t->area();
  if (const Point_2_vector  * v = object_cast<Point_2_vector> (&obj)) return polygon_area_2(v->begin(),v->end(),k);
//  if (const Polygon_2       * p = object_cast<Polygon_2     > (&obj)) return p->area();
  return 0;
}

template <class P> typename P::R::FT area(const std::vector<P> &v) {
  typename P::R::FT res;
  area_2(v.begin(),v.end(),res);
  return res;
}

template <class T, class U> inline typename T::R::FT intersection_area(const T &t, const U &u) {
  return area(intersection(t,u),typename T::R());
}
*/

/****************************************************/
/* do_intersect free functions             */
/* return  intersection_area(a,b)>0;                */
/****************************************************/

template <class K> inline bool do_intersect(const Rectangle_2<K> &a, const Rectangle_2<K> &b)
{
    typedef typename K::Vector_2 Vector_2;
    typedef typename K::FT FT;
    Vector_2 an(a.normal());
    Vector_2 bn(b.normal());
    Vector_2 v(a.center(),b.center());

    FT det = abs(an.x()*bn.y()-an.y()*bn.x());
    FT dot = abs(an*bn);

    FT an2  = an.squared_length();
    FT br(abs(b.ratio()));
    FT ax0 = an*v;
    FT dax = dot+br*det+an2;
    if(ax0*ax0>=dax*dax) return false;

    FT ar(abs(a.ratio()));
    FT ay0 = an.x()*v.y()-an.y()*v.x();
    FT day = det+br*dot+ar*an2;
    if(ay0*ay0>=day*day) return false;

    FT bn2 = bn.squared_length();
    FT bx0 = bn*v;
    FT dbx = dot+ar*det+bn2;
    if(bx0*bx0>=dbx*dbx) return false;

    FT by0 = v.x()*bn.y()-v.y()*bn.x();
    FT dby = det+ar*dot+br*bn2;
    return(by0*by0<dby*dby);
}

/*
template <class K> inline bool do_intersect(const Rectangle_2<K> &r, const typename K::Iso_rectangle_2 &b)
{
    typedef typename K::Vector_2 Vector_2;
    typedef typename K::Point_2 Point_2;
    typedef typename K::FT FT;
    Vector_2 n(r.normal());
    Point_2  c(r.center());
    FT dot(abs(n.x()));
    FT det(abs(n.y()));
    FT ar (abs(r.ratio()));

    FT dx(dot+ar*det);
    FT dy(det+ar*dot);
    if(c.x()+dx<=b.xmin() || c.x()-dx>=b.xmax() || c.y()+dy<=b.ymin() || c.y()-dy>=b.ymax())
        return false;
    Point_2  bc(midpoint(b.min(),b.max()));
    Vector_2 v(c,bc), bn(bc,b.max());

    FT n2  = n.squared_length();
    FT ax0 = n*v;
    FT dax = dot*bn.x()+det*bn.y()+n2;
    if(ax0*ax0>=dax*dax) return false;

    FT ay0 = n.x()*v.y()-n.y()*v.x();
    FT day = det*bn.x()+dot*bn.y()+ar*n2;
    return (ay0*ay0<day*day);
}
template <class K> inline bool do_intersect(const Rectangle_2<K> &r, const typename K::Triangle_2 &t)
{
    typedef typename K::Vector_2 Vector_2;
    typedef typename K::Point_2 Point_2;
    typedef typename K::FT FT;
    // optimized version of :
    // return intersection_area(b)>0;
        Vector_2 n(r.normal());
        Vector_2 m(-n.y(),n.x());
        Point_2  c(r.center());
        FT n2  = n.squared_length();
        FT m2  = abs(r.ratio())*n2;
        Vector_2 v[] = { t[0]-c, t[1]-c, t[2]-c };
        FT x[] = { n*v[0], n*v[1], n*v[2] };
        FT y[] = { m*v[0], m*v[1], m*v[2] };
        if(        ( x[0]>=n2 &&  x[1]>=n2 &&  x[2]>=n2) ||
                (-x[0]>=n2 && -x[1]>=n2 && -x[2]>=n2) ||
                ( y[0]>=m2 &&  y[1]>=m2 &&  y[2]>=m2) ||
                (-y[0]>=m2 && -y[1]>=m2 && -y[2]>=m2) ) return false; // b.bbox_aligned_with(this) does not intersect this

        // test not necessary, might speed up (early exit)
        if(        ( x[0]<n2 && -x[0]<n2 &&  y[0]<m2 && -y[0]<m2) ||
                ( x[1]<n2 && -x[1]<n2 &&  y[1]<m2 && -y[1]<m2) ||
                ( x[2]<n2 && -x[2]<n2 &&  y[2]<m2 && -y[2]<m2) ) return true; // one point of b is inside this

        // test edges of b as separating axes
        FT ta(y[0]-y[1]), tb(x[1]-x[0]), tc(-ta*x[0]-tb*y[0]);
        FT tdet = x[2]*ta+y[2]*tb+tc;
        if(tdet*tc<=0 && abs(tc)>=abs(n2*ta)+abs(m2*tb)) return false;

        ta = y[1]-y[2]; tb = x[2]-x[1]; tc = -ta*x[1]-tb*y[1];
        if(tdet*tc<=0 && abs(tc)>=abs(n2*ta)+abs(m2*tb)) return false;

        ta = y[2]-y[0]; tb = x[0]-x[2]; tc = -ta*x[2]-tb*y[2];
        return (tdet*tc>0 || abs(tc)<abs(n2*ta)+abs(m2*tb));
}

template <class K> inline bool do_intersect(const typename K::Triangle_2 &t1, const typename K::Triangle_2 &t2)
{
  if(t1.is_degenerate() || t2.is_degenerate()) return false;
  return do_intersect(t1,t2); // TODO fix overdetection : returns true if boundaries intersect and not the interior
}

template <class K> inline bool do_intersect(const typename K::Triangle_2 &t, const Rectangle_2<K> &r)
{
  return do_intersect(r,t);
}
template <class K> inline bool do_intersect(const typename K::Iso_rectangle_2 &i, const Rectangle_2<K> &r)
{
  return do_intersect(r,i);
}
*/

}; // namespace geometry

#endif // GEOMETRY_RECTANGLE_2_INTERSECTION_HPP
