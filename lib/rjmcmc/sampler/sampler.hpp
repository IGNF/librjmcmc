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

#ifndef RJMCMC_SAMPLER_HPP
#define RJMCMC_SAMPLER_HPP

#include "rjmcmc/tuple.hpp"
#include "rjmcmc/random.hpp"
#include "rjmcmc/random_apply.hpp"
#include "rjmcmc/kernel/kernel_traits.hpp"
#include "rjmcmc/kernel/kernel.hpp"
#include <iomanip>

namespace rjmcmc {


    class sampler_base
    {
    public:
        inline double acceptance_probability() const { return m_acceptance_probability; }
        inline double temperature() const { return m_temperature; }
        inline double delta() const { return m_delta; }
        inline double green_ratio() const { return m_green_ratio; }
        inline bool accepted() const { return m_accepted; }
        inline double kernel_ratio() const { return m_kernel_ratio; }
        inline double ref_pdf_ratio() const { return m_ref_pdf_ratio; }

    protected:
        double  m_acceptance_probability;
        double  m_temperature;
        double  m_delta;
        double  m_green_ratio;
        double  m_kernel_ratio;
        double  m_ref_pdf_ratio;
        bool    m_accepted;
    };

    namespace detail
    {
        // statistics accessors
        template<typename K, unsigned int I, unsigned int N> struct get_name {
            inline const char *operator()(unsigned int i, const K& k) const {
                enum { ks = tuple_element<I,K>::type::size };
                if(i<ks) return get<I>(k).name(i);
                return get_name<K,I+1,N>()(i-ks,k);
            }
        };
        template<typename K, unsigned int N> struct get_name<K,N,N> {
            inline const char *operator()(unsigned int i, const K& k) const { return ""; }
        };

        template<typename K, unsigned int I, unsigned int N> struct get_kernel_id {
            inline unsigned int operator()(unsigned int i, const K& k) const {
                enum { ks = tuple_element<I,K>::type::size };
                if(i==I) return get<I>(k).kernel_id();
                return ks+get_kernel_id<K,I+1,N>()(i,k);
            }
        };
        template<typename K, unsigned int N> struct get_kernel_id<K,N,N> {
            inline unsigned int operator()(unsigned int i, const K& k) const { return 0; }
        };

        template<typename Configuration, typename Modification>
        struct kernel_functor
        {
            Configuration& m_c;
            Modification& m_m;
            typedef double result_type;
            kernel_functor(Configuration &c, Modification &m) :m_c(c), m_m(m) {}
            template<typename T> inline result_type operator()(double x, const T& t) {
                return t(x,m_c,m_m);
            }
        };
    }

    template<typename Density, typename Acceptance, RJMCMC_TUPLE_TYPENAMES >
    class sampler : public sampler_base
    {
        typedef tuple<RJMCMC_TUPLE_TYPES> Kernels;
    public:
        /// variadic constructor : d is the reference process, a is the acceptance strategy, RJMCMC_TUPLE_ARGS is the comma-separated list of kernels
        sampler(const Density& d, const Acceptance& a, RJMCMC_TUPLE_ARGS) :
                m_die(random(), boost::uniform_real<>(0,1)),
                m_density(d),
                m_acceptance(a),
                m_kernel(RJMCMC_TUPLE_PARAMS)
        {}


        enum { size = tuple_size<Kernels>::value };
        enum { m_kernel_size = kernel_traits<Kernels>::size };

        /// This is the main sampling function, performing an RJMCMC step on the configuration c in place
        template<typename Configuration>
        void operator()(Configuration &c, double temp)
        {
            typedef typename Configuration::modification Modification;

            //1 & 2
            Modification modif;
            m_temperature = temp;
            detail::kernel_functor<Configuration,Modification> kf(c,modif);
            m_kernel_ratio = random_apply(m_kernel_id,m_die(),m_kernel,kf);

            //3
            m_ref_pdf_ratio = m_density.pdf_ratio(c,modif);
            m_green_ratio = m_kernel_ratio*m_ref_pdf_ratio;

            //4
            if(m_green_ratio<=0) {
                m_delta   =0;
                m_accepted=false;
                return;
            }
            m_delta       = c.delta_energy(modif);
            m_acceptance_probability  = m_acceptance(m_delta,m_temperature,m_green_ratio);

            //5
            m_accepted    = ( m_die() < m_acceptance_probability );
            if (m_accepted) c.apply(modif);
        }

    public:
        ///  Getting the density of the reference process
        inline const Density& density() const { return m_density; }

        ///  statistics accessor : getting the name of the ith kernel
        inline const char * kernel_name(unsigned int i) const { return detail::get_name<Kernels,0,size>()(i,m_kernel); }

        ///  statistics accessor : getting the id of the latest proposed kernel
        inline unsigned int kernel_id  () const { return detail::get_kernel_id<Kernels,0,size>()(m_kernel_id,m_kernel); }

        ///  statistics accessor : getting the number of kernels
        inline unsigned int kernel_size() const { return m_kernel_size; }

    private:
        // data
        boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_real<> > m_die;
        Density    m_density;
        Acceptance m_acceptance;
        Kernels    m_kernel;

        // statistics
        unsigned int m_kernel_id;
    };

    /*
    // convenience creation functions
    template<typename Density, typename Acceptance, RJMCMC_TUPLE_TYPENAMES >
    sampler<Density,Acceptance,RJMCMC_TUPLE_TYPES> make_sampler(const Density& d, const Acceptance& a, RJMCMC_TUPLE_ARGS)
    {
        return sampler<Density,Acceptance,RJMCMC_TUPLE_TYPES>(d,a,RJMCMC_TUPLE_PARAMS);
    }

    template<typename Density, typename Acceptance, RJMCMC_TUPLE_TYPENAMES >
    sampler<Density,Acceptance,RJMCMC_TUPLE_TYPES> * new_sampler(const Density& d, const Acceptance& a, RJMCMC_TUPLE_ARGS)
    {
        return new sampler<Density,Acceptance,RJMCMC_TUPLE_TYPES>(d,a,RJMCMC_TUPLE_PARAMS);
    }
    */
}; // namespace rjmcmc

#endif // RJMCMC_SAMPLER_HPP
