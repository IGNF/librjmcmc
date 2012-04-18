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

#ifndef DIRECT_SAMPLER_HPP
#define DIRECT_SAMPLER_HPP

#include "rjmcmc/random_variant_init.hpp"

namespace marked_point_process {

    template<typename Density, typename ObjectSampler>
    class direct_sampler
    {
    public:
        direct_sampler( const Density & density,
                        const ObjectSampler& object_sampler) :
        m_density(density), m_object_sampler(object_sampler)
        {}

        template<typename Configuration> void operator()(Configuration &c, double temperature=0) const
        {
            typedef typename Configuration::value_type T;
            T res;
            c.clear();
            int n = m_density();
            for(int i=0; i<n; ++i) {
                rjmcmc::random_variant_init(res);
                rjmcmc::apply_visitor(m_object_sampler,res);
                c.insert(res);
            }
        }

        // new/old
        template<typename Configuration, typename Modification>
        double pdf_ratio(const Configuration &c, const Modification &m) const
        {
            double ratio = m_density.pdf_ratio(c.size(),c.size()+m.delta_size());
            for(typename Modification::birth_const_iterator b = m.birth_begin(); b!=m.birth_end(); ++b)
                ratio*=rjmcmc::apply_visitor(m_object_sampler.pdf(),*b);
            for(typename Modification::death_const_iterator d = m.death_begin(); d!=m.death_end(); ++d)
                ratio/=rjmcmc::apply_visitor(m_object_sampler.pdf(),c[*d]);
            return ratio;
        }

        template<typename Configuration>
        double pdf(const Configuration &c) const
        {
            double res = m_density.pdf(c.size());
            for(typename Configuration::const_iterator it = c.begin(); it!=c.end(); ++it)
                res*=rjmcmc::apply_visitor(m_object_sampler.pdf(),c[it]);
            return res;
        }

        inline const char * kernel_name(unsigned int i) const { return "direct"; }
        inline int kernel_id() const { return 0; }
        inline bool accepted() const { return true; }
        enum { kernel_size = 1 };
    private:
        Density  m_density;
        ObjectSampler m_object_sampler;
    };
}

#endif // DIRECT_SAMPLER_HPP
