// Author(s)     : Mathieu Bredif

#ifndef CGAL_RECTANGLE_2_SQUARE_DISTANCE_H
#define CGAL_RECTANGLE_2_SQUARE_DISTANCE_H

CGAL_BEGIN_NAMESPACE

  template<class K> typename K::FT squared_distance(const Rectangle_2<K> &r, const typename K::Point_2& q) {
    typedef typename K::Vector_2 Vector_2;
    typedef typename K::FT FT;
   Vector_2 v(q-r.center());
   FT n2  = r.normal().squared_length();
   FT x = max(0.,abs(r.normal()*v)-n2);
   FT y = max(0.,abs((r.normal().x()*v.y()-r.normal().y()*v.x()))-abs(r.ratio())*n2);
   return (x*x+y*y)/n2;
  }

  template<class K> inline typename K::FT squared_distance(const typename K::Point_2& q, const Rectangle_2<K> &r) {
   return squared_distance(r,q);
  }


   /* equivalent, but at least twice as fast as
   return
        min(min(
                min(squared_distance(b.point(0)),squared_distance(b.point(1))),
                min(squared_distance(b.point(2)),squared_distance(b.point(3)))
        ),min(
                min(b.squared_distance(point(0)),b.squared_distance(point(1))),
                min(b.squared_distance(point(2)),b.squared_distance(point(3)))
        ));
   */
  template<class K> typename K::FT squared_distance(const Rectangle_2<K> &a, const Rectangle_2<K>& b) {
    typedef typename K::Vector_2 Vector_2;
    typedef typename K::FT FT;
   Vector_2 v(a.center(),b.center());
   const Vector_2& an(a.normal());
   const Vector_2& bn(b.normal());
   FT dot = an*bn;
   FT det = bn.x()*an.y()-bn.y()*an.x();
   FT an2 = an.squared_length();
   FT bn2 = bn.squared_length();
   FT ar(abs(a.ratio())), arn2(ar*an2), ax(an*v), ay(an.x()*v.y()-an.y()*v.x());
   FT br(abs(b.ratio())), brn2(br*bn2), bx(bn*v), by(bn.x()*v.y()-bn.y()*v.x());
   FT ardot (ar*dot), ardet (ar*det), brdot (br*dot), brdet (br*det);
   FT ax0(dot+brdet), ax1(dot-brdet), bx0(dot-ardet), bx1(dot+ardet);
   FT ay0(det-brdot), ay1(det+brdot), by0(det+ardot), by1(det-ardot);

   FT x[] = {
        max(0.,abs(ax-ax0)-an2), max(0.,abs(ax+ax0)-an2),
        max(0.,abs(ax-ax1)-an2), max(0.,abs(ax+ax1)-an2),
        max(0.,abs(bx-bx0)-bn2), max(0.,abs(bx+bx0)-bn2),
        max(0.,abs(bx-bx1)-bn2), max(0.,abs(bx+bx1)-bn2)
   };
   FT y[] = {
        max(0.,abs(ay+ay0)-arn2), max(0.,abs(ay-ay0)-arn2),
        max(0.,abs(ay+ay1)-arn2), max(0.,abs(ay-ay1)-arn2),
        max(0.,abs(by-by0)-brn2), max(0.,abs(by+by0)-brn2),
        max(0.,abs(by-by1)-brn2), max(0.,abs(by+by1)-brn2)
   };
   FT d2[8];
   for (int i=0;i<8;++i) d2[i]=x[i]*x[i]+y[i]*y[i];
   return
        min(    min(min(d2[0],d2[1]),min(d2[2],d2[3]))/an2,
                min(min(d2[4],d2[5]),min(d2[6],d2[7]))/bn2);
 }

CGAL_END_NAMESPACE

#endif // CGAL_RECTANGLE_2_SQUARE_DISTANCE_H
