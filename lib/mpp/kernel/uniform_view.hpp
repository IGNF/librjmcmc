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
encouraged to load and test thesoftware's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and, more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

***********************************************************************/

#ifndef MPP_UNIFORM_VIEW_HPP
#define MPP_UNIFORM_VIEW_HPP

#include "rjmcmc/kernel/kernel.hpp"
#include "rjmcmc/kernel/transform.hpp"
#include "geometry/coordinates/coordinates.hpp"
#include <algorithm> // copy_n


#include "boost/random/uniform_smallint.hpp"

namespace marked_point_process {
    

    // single object type configuration for now...
    template<typename T, unsigned int N=1>
    class uniform_view
    {
    public:
        typedef T object_type;
        enum { dimension =  coordinates_iterator<T>::dimension };

        template<typename Engine, typename Configuration, typename Modification, typename OutputIterator>
        inline double operator()(Engine& e, Configuration const& c, Modification& m, OutputIterator out) const
        {
            m.death().clear();
            typedef typename coordinates_iterator<T>::type iterator;
            unsigned int n = c.size();
            if(n<N) return 0.;
            unsigned int denom=1;
            int d[N];
            for(unsigned int i=0 ; i<N ; ++i,--n)
            {
                boost::uniform_smallint<> die(0,n-1);
                d[i]=die(e);
                for(unsigned int j=0;j<i;++j) if(d[j]<=d[i]) ++d[i]; // skip already selected indices

                typename Configuration::const_iterator it = c.begin();
                std::advance(it, d[i]);
                m.death().push_back(it);
                const T& t = c.value(it);
                iterator coord_it  = coordinates_begin(t,e);
                for(unsigned int j=0; j<dimension; ++j) *out++ = *coord_it++;
                denom *= n;
            }
            return 1./denom;
        }
        template<typename Configuration, typename Modification, typename InputIterator>
        inline double inverse_pdf(Configuration const& c, Modification& m, InputIterator it) const
        {
            m.birth().clear();
            unsigned int beg   = c.size()-m.death().size()+1;
            unsigned int end   = beg+N;
            unsigned int denom = 1;
            object_from_coordinates<T> creator;
            for(unsigned int n=beg ; n<end ; ++n)
            {
                m.birth().push_back(creator(it));
                it    += dimension;
                denom *= n;
            }
            return 1./denom;
        }
    };

}; // namespace marked_point_process

#endif // UNIFORM_VIEW_HPP
