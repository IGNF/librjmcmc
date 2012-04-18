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

#ifndef REJECTION_SAMPLER_HPP_
#define REJECTION_SAMPLER_HPP_

namespace rjmcmc
{

    template<typename Sampler, typename Pred>
    class rejection_sampler
    {
    public:
        rejection_sampler( const Sampler & sampler, Pred pred = Pred() ) :
                m_sampler(sampler), m_pred(pred)
        {}

        template<typename Configuration> void operator()(Configuration &c, double temperature=0)
        {
            // caveat: if m_sampler is (rj)mcmc, it may take a while to come back to valid configurations...
            // TODO: backup initial configuration ???
            do { m_sampler(c,temperature); } while (!m_pred(c));
        }

        inline const char * kernel_name(unsigned int i) const { return m_sampler.kernel_name(i); }
        inline unsigned int kernel_id() const { return m_sampler.kernel_id(); }
        inline bool accepted() const { return m_sampler.accepted(); }
        enum { kernel_size = Sampler::kernel_size };
    private:
        Sampler m_sampler;
        Pred    m_pred;
    };

    struct null_binary_energy_predicate
    {
        template<typename Configuration> bool operator()(const Configuration &c) const
        {
            return c.binary_energy()==0;
        }

    };

}

#endif // REJECTION_SAMPLER_HPP_
