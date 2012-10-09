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

#ifndef BINARY_KERNEL_HPP
#define BINARY_KERNEL_HPP

#include "rjmcmc/random.hpp"

namespace rjmcmc {

    // a kernel encodes a reversible jump
    // for complex problems, a reversible jump typically only modifies a portion of the whole configuration.
    // eg for mpp problems, a reversible jump typically only adds, removes or modifies a few objects.
    // a view denotes the part of the configuration modified by the jump, other aspects of the configuration remaining constant.
    //

    class null_view
    {
    public:
        enum { dimension = 0 };
        template<typename Configuration, typename Modification, typename OutputIterator>
        inline double operator()(Configuration& c, Modification& modif, OutputIterator it) const {
            return 1.;
        }
        template<typename Configuration, typename Modification, typename InputIterator>
        inline double inverse_pdf(Configuration& c, Modification& modif, InputIterator it) const {
            return 1.;
        }
    };


    template<unsigned int N>
    class variate
    {
        typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_real<> > die_type;
        mutable die_type m_die;
    public:
        enum { dimension = N };
        template<typename Configuration, typename Modification, typename OutputIterator>
        inline double operator()(Configuration&, Modification&, OutputIterator it) const {
            for(unsigned int i=0; i<N; ++i) *it++ = m_die();
            return 1.;
        }
        template<typename Configuration, typename Modification, typename InputIterator>
        inline double inverse_pdf(Configuration&, Modification&, InputIterator it) const {
            for(unsigned int i=0; i<N; ++i, ++it) if(*it<0 || *it>1) return 0;
            return 1.;
        }
        template<typename InputIterator>
        inline double pdf(InputIterator it) const {
            for(unsigned int i=0; i<N; ++i, ++it) if(*it<0 || *it>1) return 0;
            return 1.;
        }
        variate() : m_die(rjmcmc::random(), boost::uniform_real<>(0,1)) {}
    };

    template<typename View0, typename View1, typename Variate0, typename Variate1, typename Transform> class kernel
    {
    double m_p, m_p01, m_p10;
    View0 m_view0;
    View1 m_view1;
    Variate0 m_variate0;
    Variate1 m_variate1;
    Transform m_transform;
    mutable unsigned int m_kernel_id;
    public:
    enum { size = 2 };
    inline unsigned int kernel_id() const { return m_kernel_id; }
    inline const char* name(unsigned int i) const { return "kernel"; }//(i)?m_kernel1.name():m_kernel0.name(); }

    kernel(const View0& v0, const View1& v1, const Variate0& x0, const Variate1& x1, const Transform& t, double p01=0.5, double p10=0.5) :
            m_view0(v0), m_view1(v1), m_variate0(x0), m_variate1(x1), m_transform(t), m_p(p01+p10), m_p01(p01), m_p10(p10)
    {
    }
    kernel(double p01=0.5, double p10=0.5) :
            m_view0(), m_view1(), m_variate0(), m_variate1(), m_transform(), m_p(p01+p10), m_p01(p01), m_p10(p10)
    {
    }
    inline double probability() const { return m_p; }

    // p is uniform between 0 and probability()
    template<typename Configuration, typename Modification>
    double operator()(double p, Configuration& c, Modification& modif) const
    {
        modif.clear();
        double val0[Transform::dimension];
        double val1[Transform::dimension];
        if(p<m_p01) { // branch probability : m_p01
            m_kernel_id = 0;
            double *var0 = val0 + View0::dimension;
            double *var1 = val1 + View1::dimension;
            double J01  = m_view0   (c,modif,val0);             // returns the discrete probability that samples the portion of the configuration that is being modified (stored in the modif input)
            double phi01= m_variate0(c,modif,var0);             // returns the continuous probability that samples the completion variates
            m_transform.apply(val0,val1);                       // computes val1 from val0
            double phi10= m_variate1.inverse_pdf(c,modif,var1); // returns the continuous probability of the inverse variate sampling, arguments are constant
            double J10  = m_view1   .inverse_pdf(c,modif,val1); // returns the discrete probability of the inverse view sampling, arguments are constant except val1 that is encoded in modif
            return m_transform.abs_jacobian(val0)*(m_p10*J10*phi10)/(m_p01*J01*phi01);
        } else { // branch probability : m_p10
            m_kernel_id = 1;
            double *var1 = val1 + View1::dimension;
            double *var0 = val0 + View0::dimension;
            double J10  = m_view1   (c,modif,val1);      // returns the discrete probability that samples the portion of the configuration that is being modified (stored in the modif input)
            double phi10= m_variate1(c,modif,var1);      // returns the continuous probability that samples the completion variates
            m_transform.inverse(val1,val0);                       // computes val0 from val1
            double J01  = m_view0   .inverse_pdf(c,modif,var0);  // returns the continuous probability of the inverse variate sampling, arguments are constant
            double phi01= m_variate0.inverse_pdf(c,modif,val0);  // returns the discrete probability of the inverse view sampling, arguments are constant except val0 that is encoded in modif
            return m_transform.abs_jacobian(val1)*(m_p01*J01*phi01)/(m_p10*J10*phi10);
        }
    }
};


}; // namespace rjmcmc

#endif // BINARY_KERNEL_HPP
