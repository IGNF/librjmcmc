#ifndef ISO_RECTANGLE_2_SEGMENT_2_CLIP_H
#define ISO_RECTANGLE_2_SEGMENT_2_CLIP_H

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

#endif // ISO_RECTANGLE_2_SEGMENT_2_CLIP_H
