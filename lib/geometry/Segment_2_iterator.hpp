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

#ifndef GEOMETRY_SEGMENT_2_ITERATOR_HPP
#define GEOMETRY_SEGMENT_2_ITERATOR_HPP

namespace geometry {

// Découpage d'un segment par une grille

template<typename R_> class Segment_2_iterator {
  typedef R_                    R;
  typedef typename R::Segment_2 Segment_2;
  typedef typename R::FT        FT;
public:
  // rasterize the segment s on a pixel grid [i,i+1]x[j,j+1]
  // pixels centers are (i+0.5, j+0.5)
  Segment_2_iterator () : t(1) {}
  Segment_2_iterator (const Segment_2& s) : t(0) {
        for (int j = 0; j < 2; ++j) {
            p[j] = (int) std::floor(/*to_double --> CGAL::to_double*/(s.source()[j]));
                q[j] = (int) std::floor(/*to_double --> CGAL::to_double*/(s.target()[j]));
                if(p[j]==q[j]) { NextCrossingT[j]=2; Step[j]=0; DeltaT[j]=0; continue; }
                int dir = (q[j]>p[j]);
                FT invlength =  1.0f / (s.target()[j]-s.source()[j]);
                NextCrossingT[j] = (p[j]+dir -(s.source()[j]))*invlength;
                Step  [j] = 2*dir-1;
                DeltaT[j] = Step[j] * invlength;
        }
        i = NextCrossingT[1] < NextCrossingT[0];
        if(p[i]==q[i]) NextCrossingT[i]=1;
	if(NextCrossingT[i]<=0) operator++();
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

}; // namespace geometry

#endif // GEOMETRY_SEGMENT_2_ITERATOR_HPP

