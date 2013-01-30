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

#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP


// transform<N,T> concept :
//
//	T apply  (const T[N] in, T[N] out) const;
// applies T and returns the absolute jacobian of T evaluated at "in"
//
//	T inverse(const T[N] in, T[N] out) const;
// applies T^-1 and returns the absolute jacobian of T^-1 evaluated at "in"



#include <algorithm>
#include <cmath>

namespace rjmcmc {


    namespace matrix {

        template<unsigned int N, typename T> T determinant(T m[N*N]);
        template<unsigned int N, typename T> T cofactor(int i0, int j0, T m[N*N])
        {
            if(N==1) return *m;
            T sub[(N-1)*(N-1)];
            T *s = sub;
            for(unsigned int j=0; j<N; ++j)
            {
                if(j==j0) continue;
                for(unsigned int i=0; i<N; ++i)
                {
                    if(i==i0) continue;
                    *s++ = m[i+j*N];
                }
            }
            T sign = ((i0+j0)&1)?-1:1;
            return sign*determinant(sub);
        }

        template<unsigned int N, typename T> T determinant(T m[N*N])
        {
            T det = 0;
            for(unsigned int k=0; k<N; ++k)
                det += m[k]*cofactor(k,0,m);
            return det;
        }

        template<unsigned int N, typename T> T inverse(T in[N*N], T out[N*N])
        {
            T det = determinant(in);
            // assert(det!=0);
            T det_inv = 1/det;
            for(unsigned int j=0; j<N; ++j)
                for(unsigned int i=0; i<N; ++i)
                    out[i+j*N] = cofactor(j,i,in)*det_inv;
            return det;
        }

        template<unsigned int N, typename T> void linear_apply(const T m[N*N], const T in[N], T out[N])
        {
            for(T *oit = out; oit<out+N; ++oit)
            {
                *oit = 0;
                for(const T *iit = in; iit<in+N; ++iit)
                {
                    *oit += *iit*(*m++);
                }
            }
        }

        template<unsigned int N, typename T, unsigned int S=1> void affine_apply(const T m[N*N], const T d[N], const T in[N], T out[N])
        {
            for(T *oit = out; oit<out+N; ++oit)
            {
                *oit = S*(*d++);
                for(const T *iit = in; iit<in+N; ++iit)
                {
                    *oit += *iit*(*m++);
                }
            }
        }
    }

    template<unsigned int N, typename T>
    class linear_transform
    {
    public:
        enum { dimension = N };

        inline T apply  (const T in[N], T out[N]) const { matrix::linear_apply(m_mat,in,out); return m_abs_jacobian[0]; }
        inline T inverse(const T in[N], T out[N]) const { matrix::linear_apply(m_inv,in,out); return m_abs_jacobian[1]; }

        linear_transform(T *v) {
            std::copy(v,v+(N*N), m_mat);
            T det = matrix::inverse(m_mat,m_inv);
            m_abs_jacobian[0] = std::abs(  det);
            m_abs_jacobian[1] = std::abs(1/det);
        }

    private:
        T m_mat[N*N];
        T m_inv[N*N];
        T m_abs_jacobian[2];

    };

    template<unsigned int N, typename T>
    class identity_transform
    {
    public:
        enum { dimension = N };

        template<typename IteratorIn,typename IteratorOut>
        inline T apply  (IteratorIn in, IteratorOut out) const {
            for(unsigned int i=0; i<N; ++i) *out++ = *in++;
            return 1;
        }
        template<typename IteratorIn,typename IteratorOut>
        inline T inverse(IteratorIn in, IteratorOut out) const {
            for(unsigned int i=0; i<N; ++i) *out++ = *in++;
            return 1;
        }

        identity_transform() {}
    };

    template<unsigned int N, typename T>
    class diagonal_linear_transform
    {
    public:
        enum { dimension = N };

        template<typename IteratorIn,typename IteratorOut>
        inline T apply  (IteratorIn in, IteratorOut out) const {
            for(T *m = m_mat;m!=m_mat+N;++m) *out++ = (*in++)*(*m);
            return m_abs_jacobian[0];
        }
        template<typename IteratorIn,typename IteratorOut>
        inline T inverse(IteratorIn in, IteratorOut out) const {
            for(T *m = m_inv;m!=m_inv+N;++m) *out++ = (*in++)*(*m);
            return m_abs_jacobian[1];
        }

        diagonal_linear_transform() {}

        template<typename Iterator>
        diagonal_linear_transform(Iterator it) {
            T det = 1.;
            for(unsigned int i=0; i<N; ++i)
            {
                T v = *it++;
                det *= v;
                m_mat[i] = v;
                m_inv[i] = T(1)/v;
            }
            m_abs_jacobian[0] = std::fabs(  det);
            m_abs_jacobian[1] = std::fabs(1/det);
        }

    private:
        T m_mat[N];
        T m_inv[N];
        T m_abs_jacobian[2];
    };

    template<unsigned int N, typename T>
    class diagonal_affine_transform
    {
    public:
        enum { dimension = N };

        template<typename IteratorIn,typename IteratorOut>
        inline T apply  (IteratorIn in, IteratorOut out) const {
            const T *m = m_mat, *d = m_delta;
            for(T *oit = out;oit<out+N;++oit) *oit = (*in++)*(*m++) + (*d++);
            return m_abs_jacobian[0];
        }
        template<typename IteratorIn,typename IteratorOut>
        inline T inverse(IteratorIn in, IteratorOut out) const {
            const T *m = m_inv, *d = m_delta;
            for(T *oit = out;oit<out+N;++oit) *oit = ((*in++) - (*d++))*(*m++);
            return m_abs_jacobian[1];
        }

        diagonal_affine_transform() {}

        template<typename IteratorV,typename IteratorD>
        diagonal_affine_transform(IteratorD d,IteratorV v) {
            T det = 1.;
            for(unsigned int i=0; i<N; ++i,++v,++d)
            {
                det *= *d;
                m_mat[i] = *d;
                m_inv[i] = T(1)/(*d);
                m_delta[i] = *v;
            }
            m_abs_jacobian[0] = std::fabs(  det);
            m_abs_jacobian[1] = std::fabs(1/det);
        }

    private:
        T m_delta[N];
        T m_mat  [N];
        T m_inv  [N];
        T m_abs_jacobian[2];

    };

    template<unsigned int N, typename T>
    class affine_transform
    {
        enum { dimension = N };

        inline T apply  (const T in[N], T out[N]) const { matrix::affine_apply<N,T, 1>(m_mat,m_delta,in,out); return m_abs_jacobian[0]; }
        inline T inverse(const T in[N], T out[N]) const { matrix::affine_apply<N,T,-1>(m_inv,m_delta,in,out); return m_abs_jacobian[1]; }

        affine_transform(T *v, T *d) {
            std::copy(v,v+(N*N), m_mat  );
            std::copy(d,d+ N   , m_delta);
            T det = matrix::inverse(m_mat,m_inv);
            m_abs_jacobian[0] = std::abs(  det);
            m_abs_jacobian[1] = std::abs(1/det);
        }

    private:
        T m_delta[N];
        T m_mat[N*N];
        T m_inv[N*N];
        T m_abs_jacobian[2];
    };

}


#endif /* TRANSFORM_HPP */
