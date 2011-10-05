#ifndef GEOMETRY_KERNELS_HPP
#define GEOMETRY_KERNELS_HPP

/*
#if defined(GEOMETRY_CIRCLE_2_H) && defined(GEOMETRY_RECTANGLE_2_H)

  template<typename K> result_type operator()(
                                               const geometry::Rectangle_2<K> &r,
                                               geometry::Circle_2<K> &c
                                             ) const
  {
    double radius = sqrt(r.normal().squared_length());
    double ratio  = std::abs(r.ratio());
    if(ratio<1) radius *= ratio;
    c = geometry::Circle_2<K>(r.center(),radius);
    return 1.; // TODO
  }

  template<typename K> result_type operator()(
                                               const geometry::Circle_2<K> &c,
                                               geometry::Rectangle_2<K> &r
                                             ) const
  {
    typedef typename K::Vector_2 Vector_2;
    Vector_2 v((2*m_dief()-1),(2*m_dief()-1));
    v = v*(geometry::radius(c)/sqrt(v.squared_length()));
    r = geometry::Rectangle_2<K>(c.center(),v,1);
    return 1.; // TODO
  }

  template<typename K> result_type operator()(
                                               const geometry::Rectangle_2<K> &r,
                                               std::pair<geometry::Rectangle_2<K>,geometry::Circle_2<K> > &res
                                             ) const
  {
    typedef typename K::Vector_2 Vector_2;
                // todo: verifier que ca fait pas n'importe quoi...
    int i = m_die4();
    double f = m_dief();
    double g = m_dief();
    double h = m_dief();
    Vector_2 n = r.normal(i)*std::min(1.,std::abs(r.ratio()));
    if(i%2) n = n*(1./r.ratio());
    Vector_2 v(-n.x()+n.y(),-n.y()-n.x());
    res.first  = r.scaled_edge(i,f).scaled_edge(i+1,g);
    res.second = geometry::Circle_2<K>(r[i+3]+v*h,h*std::sqrt(n.squared_length()));
    return 1.; // TODO
  }
#endif // defined(GEOMETRY_CIRCLE_2_H) && defined(GEOMETRY_RECTANGLE_2_H)
*/

#endif // GEOMETRY_KERNELS_HPP
