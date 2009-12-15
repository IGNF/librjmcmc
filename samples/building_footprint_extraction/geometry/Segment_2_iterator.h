// Author(s)     : Mathieu Bredif

#ifndef CGAL_SEGMENT_2_ITERATOR_H
#define CGAL_SEGMENT_2_ITERATOR_H

CGAL_BEGIN_NAMESPACE

// Découpage d'un segment par une grille

template<class Seg2> class Segment_2_iterator {
  typedef Seg2             Segment_2;
  typedef typename Segment_2::R      R;
  typedef typename R::FT             FT;
public:
  // rasterize the segment s on a pixel grid [i-f,i+1-f]x[j-f,j+1-f]
  // pixels centers are (i+0.5-f, j+0.5-f)
  Segment_2_iterator () : t(1) {}
  Segment_2_iterator (const Segment_2& s, FT f=0) : t(0) {
        for (int j = 0; j < 2; ++j) {
                p[j] = (int) std::floor(to_double(s.source()[j]+f));
                q[j] = (int) std::floor(to_double(s.target()[j]+f));
                if(p[j]==q[j]) { NextCrossingT[j]=2; Step[j]=0; DeltaT[j]=0; continue; }
                int dir = (q[j]>p[j]);
                FT invlength =  1.0f / (s.target()[j]-s.source()[j]);
                NextCrossingT[j] = (p[j]+dir -(s.source()[j]+f))*invlength;
                Step  [j] = 2*dir-1;
                DeltaT[j] = Step[j] * invlength;
        }
        i = NextCrossingT[1] < NextCrossingT[0];
        if(p[i]==q[i]) NextCrossingT[i]=1;
  }
  Segment_2_iterator operator++( int ) { Segment_2_iterator tmp(*this); operator++(); return tmp; }
  Segment_2_iterator& operator++() {
    t                 = NextCrossingT[i];
    p[i]             += Step[i];
    NextCrossingT[i] += DeltaT[i];
    i = NextCrossingT[1] < NextCrossingT[0];
    if(p[i]==q[i]) NextCrossingT[i]=1;
    return *this;
  }
  inline bool end  () const { return t==1; }
  inline bool begin() const { return t==0; }
  inline int x() const { return p[0]; }
  inline int y() const { return p[1]; }
  inline FT prev  () const { return t; }
  inline FT next  () const { return NextCrossingT[i]; }
  inline FT length() const { return NextCrossingT[i]-t; }
  inline int axis () const { return i; } // 0->x, 1->y
  inline int step () const { return Step[i]; } // + or - 1
  inline int step (int k) const { return Step[k]; }

private :
  int p[2], q[2], Step[2], i;
  FT NextCrossingT[2], DeltaT[2], t;
};


CGAL_END_NAMESPACE

#endif // CGAL_SEGMENT_2_ITERATOR_H
