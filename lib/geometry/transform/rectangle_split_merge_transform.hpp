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

#ifndef RECTANGLE_SPLIT_MERGE_KERNEL_HPP
#define RECTANGLE_SPLIT_MERGE_KERNEL_HPP

#include "geometry/Rectangle_2.hpp"
namespace geometry {
    struct rectangle_split_merge_transform
    {
        enum { dimension = 10 };
        rectangle_split_merge_transform(double d=5.) : m_d(d) {}

        template<int I, typename Iterator>
        inline double abs_jacobian(Iterator in) const {  // todo : check
            double u = in[2];
            double v = in[3];
            double j = 2.*m_d*m_d*(u*u+v*v);
            return I ? (1./j) : j ;
        }

        template<int I, typename IteratorIn,typename IteratorOut>
        inline double apply  (IteratorIn in, IteratorOut out) const {
            if(I) return backward(in,out);
            else  return forward (in,out);
        }

        template<typename IteratorIn,typename IteratorOut>
        inline double forward  (IteratorIn in, IteratorOut out) const {
            typedef typename std::iterator_traits<IteratorIn>::value_type FT;
            FT x = *in++;
            FT y = *in++;
            FT u = *in++;
            FT v = *in++;
            FT r = *in++;
            FT p = *in++;
            FT q = *in++;
            FT s = *in++;
            FT t = *in++;
            FT l = *in++;

            FT du = m_d*(s-0.5);
            FT dv = m_d*(t-0.5);
            FT dl = l-0.5;

            FT r_p = r-p;
            FT r_q = r-q;

            *out++ = x-r_p*v;
            *out++ = y+r_p*u;
            *out++ = u;
            *out++ = v;
            *out++ = p;

            *out++ = x+r_q*v+du-q*dv+dl*u;
            *out++ = y-r_q*u+dv+q*du+dl*v;
            *out++ = u+du;
            *out++ = v+dv;
            *out++ = q;
            /* maxima : factor(determinant(jacobian(
            [x-(r-p)*v, y+(r-p)*u, u, v, p, x+(r-q)*v+du-q*dv+dl*u, y-(r-q)*u+dv+q*du+dl*v, u+du, v+dv, q ] ,
            [x,y,u,v,r,p,q,du,dv,dl] ))) = 2*(u*u+v*v); */
            return 2.*m_d*m_d*(u*u+v*v);
        }

        template<typename IteratorIn,typename IteratorOut>
        inline double backward(IteratorIn in, IteratorOut out) const {
            typedef typename std::iterator_traits<IteratorIn>::value_type FT;
            FT x0 = *in++; // x -(r-p)*v
            FT y0 = *in++; // y +(r-p)*u
            FT u  = *in++;
            FT v  = *in++;
            FT p  = *in++;
            FT x1 = *in++; // x+(r-q)*v+du-q*dv+dl*u
            FT y1 = *in++; // y-(r-q)*u+dv+q*du+dl*v
            FT u1 = *in++;
            FT v1 = *in++;
            FT q  = *in++;

            FT du = u1 - u;
            FT dv = v1 - v;
            FT n2 = u*u+v*v;
            if(n2==0) return 0;
            FT dx = (x1-du+q*dv-x0); // == (2*r-p-q)*v+dl*u
            FT dy = (y1-dv-q*du-y0); // ==-(2*r-p-q)*u+dl*v
            FT dl = (dx*u+dy*v)/n2;
            FT sr = (dx*v-dy*u)/n2; // 2*r-p-q;
            FT  r = (sr + p + q)*0.5;
            FT r_p = r-p;
            FT  x = x0+r_p*v;
            FT  y = y0-r_p*u;

            *out++ = x;
            *out++ = y;
            *out++ = u;
            *out++ = v;
            *out++ = r;
            *out++ = p;
            *out++ = q;
            *out++ = 0.5+du/m_d;
            *out++ = 0.5+dv/m_d;
            *out++ = 0.5+dl;
            return 0.5/(m_d*m_d*(u*u+v*v));

            /*
            maxima:
    r(x0,y0,u0,v0,r0,x1,y1,u1,v1,r1):=((r0+r1+((x1-(u1-u0)+r1*(v1-v0)-x0)*v0-(y1-(v1-v0)-r1*(u1-u0)-y0)*u0)/(u0*u0+v0*v0))/2);
    factor(determinant(jacobian([
        x0-(r(x0,y0,u,v,p,x1,y1,u1,v1,q)-p)*v,
        y0+(r(x0,y0,u,v,p,x1,y1,u1,v1,q)-p)*u, u, v,
        r(x0,y0,u,v,p,x1,y1,u1,v1,q),
        p, q, u1-u, v1-v, ((x1-(u1-u)+q*(v1-v)-x0)*u+(y1-(v1-v)-q*(u1-u)-y0)*v)/(u*u+v*v)] ,
        [x0,y0,u,v,p,x1,y1,u1,v1,q]))) = 1/(2*(u*u+v*v)) ;

        ratsimp(sublis([x0=x-(r-p)*v, y0=y+(r-p)*u, u0=u, v0=v, r0=p, x1=x+(r-q)*v+d*(s-0.5)-q*d*(t-0.5)+(l-0.5)*u, y1=y-(r-q)*u+d*(t-0.5)+q*d*(s-0.5)+(l-0.5)*v, u1=u+d*(s-0.5), v1=v+d*(t-0.5), r1=q ], [x0+(r(x0,y0,u0,v0,r0,x1,y1,u1,v1,r1)-r0)*v0,y0-(r(x0,y0,u0,v0,r0,x1,y1,u1,v1,r1)-r0)*u0, u0, v0, r(x0,y0,u0,v0,r0,x1,y1,u1,v1,r1),r0, r1, 0.5+(u1-u0)/d, 0.5+(v1-v0)/d, 0.5+((x1-(u1-u0)+r1*(v1-v0)-x0)*u0+(y1-(v1-v0)-r1*(u1-u0)-y0)*v0)/(u0*u0+v0*v0)]));
        */

        }
    private:
        double m_d;
    };


    struct rectangle_split_merge_transform_v2
    {
        enum { dimension = 10 };
        rectangle_split_merge_transform_v2(double d=400.) : m_d(d), m_d4(d*d*d*d) {}

        template<int I, typename Iterator>
        inline double abs_jacobian(Iterator in) const {
            if(I) return 1./((in[4]+in[9])*m_d4);
            else  return in[4]*m_d4;
        }

        template<int I, typename IteratorIn,typename IteratorOut>
        inline double apply  (IteratorIn in, IteratorOut out) const {
            if(I) return backward(in,out);
            else  return forward (in,out);
        }

        template<typename IteratorIn,typename IteratorOut>
        inline double forward  (IteratorIn in, IteratorOut out) const {
            typedef typename std::iterator_traits<IteratorIn>::value_type FT;
            FT x = *in++;
            FT y = *in++;
            FT u = *in++;
            FT v = *in++;
            FT r = *in++;
            FT p = *in++;
            FT q = *in++;
            FT s = *in++;
            FT t = *in++;
            FT g = *in++;

            // split rectangle at fraction f
            // Vector_2 m(-r*n.y(),r*n.x());
            // case 0 : Point_2 p(c-f*m); return std::make_pair(Rectangle_2(p+m, n,f*r),Rectangle_2(p,n,(1-f)*r)); }
            // perturbate the center and normal of the second rectangle

            FT f  = r*g;
            FT dx = m_d*(p-0.5);
            FT dy = m_d*(q-0.5);
            FT du = m_d*(s-0.5);
            FT dv = m_d*(t-0.5);

            *out++ = x+(r-f)*v;
            *out++ = y-(r-f)*u;
            *out++ = u;
            *out++ = v;
            *out++ = f;

            *out++ = x-f*v+dx;
            *out++ = y+f*u+dy;
            *out++ = u+du;
            *out++ = v+dv;
            *out++ = r-f;
            // maxima : factor(determinant(jacobian([x+(r-f)*v, y-(r-f)*u, u, v, f, x-f*v+dx, y+f*u+dy, u+du, v+dv, r-f ] ,[x,y,u,v,r,du,dv,dx,dy,f] )));
            // -1

            return r*m_d4;
        }

        template<typename IteratorIn,typename IteratorOut>
        inline double backward(IteratorIn in, IteratorOut out) const {
            typedef typename std::iterator_traits<IteratorIn>::value_type FT;
            FT x0 = *in++; // x+(r-f)*v
            FT y0 = *in++; // y-(r-f)*u
            FT u  = *in++;
            FT v  = *in++;
            FT f  = *in++;
            FT x1 = *in++; // x-f*v+dx
            FT y1 = *in++; // y+f*u+dy
            FT u1 = *in++; // u+du
            FT v1 = *in++; // v+dv
            FT q  = *in++; // r-f

            FT x  = x0-q*v;
            FT y  = y0+q*u;
            FT r  = f+q;
            FT dx = x1-x+f*v;
            FT dy = y1-y-f*u;
            FT du = u1-u;
            FT dv = v1-v;

            *out++ = x;
            *out++ = y;
            *out++ = u;
            *out++ = v;
            *out++ = r;
            *out++ = 0.5+dx/m_d;
            *out++ = 0.5+dy/m_d;
            *out++ = 0.5+du/m_d;
            *out++ = 0.5+dv/m_d;
            *out++ = f/r;

            return 1./(m_d4*r);

        }
    private:
        double m_d, m_d4;
    };
}

#endif // RECTANGLE_SPLIT_MERGE_KERNEL_HPP
