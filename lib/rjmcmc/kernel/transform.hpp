#ifndef __TRANSFORM_HPP__
#define __TRANSFORM_HPP__


namespace rjmcmc {

    // transform<N,T> concept :
    //	void apply  (const T[N] in, T[N] out) const;
    //	void inverse(const T[N] in, T[N] out) const;
    //	T abs_jacobian(const T[N]) const;

    template<unsigned int N, typename T>
    class linear_transform
    {
    public:

        inline T determinant() const { return m_determinant; }
        inline T abs_jacobian(const T[N]) const { return m_abs_determinant; }
        inline void apply  (const T in[N], T out[N]) const { apply(m_mat,in,out); }
        inline void inverse(const T in[N], T out[N]) const { apply(m_inv,in,out); }

        linear_transform() {}

        linear_transform(T *v) {
            std::copy(v,v+(N*N), m_mat);
            m_determinant = determinant(m_mat);
            T det_inv = (m_determinant ? T(1)/m_determinant : T(0));
            for(unsigned int j=0; j<N; ++j)
            {
                for(unsigned int i=0; i<N; ++i)
                {
                    m_inv[i+j*N] = cofactor(j,i,m_mat)*det_inv;
                }
            }
            m_abs_determinant = std::abs(m_determinant);
        }

        T cofactor(unsigned int i, unsigned int j) const {
            return cofactor(i,j,m_mat);
        }

    private:
        T m_mat[N*N];
        T m_inv[N*N];
        T m_determinant;
        T m_abs_determinant;

        static void apply(const T *m, const T *in, T *out)
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

        template<unsigned int S> static inline T cofactor(int i0, int j0, T m[S*S])
        {
            if(S==1) return *m;
            T sub[(S-1)*(S-1)];
            T *s = sub;
            for(unsigned int j=0; j<S; ++j)
            {
                if(j==j0) continue;
                for(unsigned int i=0; i<S; ++i)
                {
                    if(i==i0) continue;
                    *s++ = m[i+j*S];
                }
            }
            T sign = ((i0+j0)&1)?-1:1;
            return sign*determinant(sub);
        }

        template<unsigned int S> static inline T determinant(T m[S*S])
        {
            T det = 0;
            for(unsigned int k=0; k<N; ++k)
                det += m[k]*cofactor(k,0,m);
            return det;
        }
    };

    template<unsigned int N, typename T>
    class affine_transform
    {
        // TODO
    };

    template<unsigned int N, typename T>
    class diagonal_linear_transform
    {
    public:

        inline T determinant() const { return m_determinant; }
        inline T abs_jacobian(const T[N]) const { return m_abs_determinant; }

        inline void apply  (const T in[N], T out[N]) const {
            T *m = m_mat;
            for(T *oit = out;oit<out+N;++oit) *oit = (*in++)*(*m++);
        }
        inline void inverse(const T in[N], T out[N]) const {
            T *m = m_inv;
            for(T *oit = out;oit<out+N;++oit) *oit = (*in++)*(*m++);
        }

        diagonal_linear_transform() {}

        diagonal_linear_transform(T v[N]) {
            m_determinant = 1.;
            for(unsigned int i=0; i<N; ++i)
            {
                m_determinant *= v[i];
                m_mat[i] = v[i];
                m_inv[i] = T(1)/v[i];
            }
            m_abs_determinant = std::fabs(m_determinant);
        }

    private:
        T m_mat[N];
        T m_inv[N];
        T m_determinant;
        T m_abs_determinant;
    };

    template<unsigned int N, typename T>
    class diagonal_affine_transform
    {
    public:

        inline T determinant() const { return m_determinant; }
        inline T abs_jacobian(const T[N]) const { return m_abs_determinant; }

        inline void apply  (const T in[N], T out[N]) const {
            const T *m = m_mat, *d = m_delta;
            for(T *oit = out;oit<out+N;++oit) *oit = (*in++)*(*m++) + (*d++);
        }
        inline void inverse(const T in[N], T out[N]) const {
            const T *m = m_inv, *d = m_delta;
            for(T *oit = out;oit<out+N;++oit) *oit = ((*in++) - (*d++))*(*m++);
        }

        diagonal_affine_transform() {}

        diagonal_affine_transform(T v[N], T d[N]) {
            m_determinant = 1.;
            for(unsigned int i=0; i<N; ++i)
            {
                m_determinant *= v[i];
                m_mat[i] = v[i];
                m_inv[i] = T(1)/v[i];
                m_delta[i] = d[i];
            }
            m_abs_determinant = std::fabs(m_determinant);
        }

    private:
        T m_delta[N];
        T m_mat  [N];
        T m_inv  [N];
        T m_determinant;
        T m_abs_determinant;

    };

}


#endif /* __TRANSFORM_HPP__ */
