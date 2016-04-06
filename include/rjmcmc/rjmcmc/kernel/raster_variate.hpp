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

#ifndef RJMCMC_RASTER_VARIATE_HPP
#define RJMCMC_RASTER_VARIATE_HPP

#include <boost/random/uniform_real.hpp>
#include <vector>
#include <algorithm>

namespace rjmcmc {


    // this variate generates a point in [0,1]^N with a piecewise uniform density where the pieces are given by a uniform rectangular voxel grid.
    // int *size gives the number of elements of the grid in each dimension
    // T *pdf gives the unnormalized pdf values of each voxel as a N dimensional array of size (size)

    template<int N>
    class raster_variate
    {
        typedef boost::uniform_real<> rand_type;
        mutable rand_type m_rand;

    public:
        typedef double value_type;
        enum { dimension = N };
        template<typename Engine, typename OutputIterator>
        inline double operator()(Engine& e, OutputIterator it) const {
            double x = m_rand(e);
            int offset = int(std::upper_bound(m_cdf.begin()+1,m_cdf.end(),x)-(m_cdf.begin()+1));
            double pdf = (m_cdf[offset+1]-m_cdf[offset])*m_totsize;
            for(int i=0; i<N; ++i)
            {
                int ix = offset % m_size[i];
                *it++ = (ix+m_rand(e))/m_size[i];
                offset /= m_size[i];
            }
            return pdf;
        }
        template<typename InputIterator>
        inline double pdf(InputIterator it) const {
            int offset = 0;
            int stride = 1;
            for(int i=0; i<N; ++i)
            {
                double x = *it++;
                if(x<0. || x>=1.) return 0.;
                int ix = int(x*m_size[i]);
                offset += stride*ix;
                stride *= m_size[i];
            }
            return (m_cdf[offset+1]-m_cdf[offset])*m_totsize;
        }
        template<typename T>
        raster_variate(T* pdf, int *size) {
            m_size.resize(N);
            m_totsize = 1;
            for(int i=0; i<N; ++i) { m_totsize *= size[i]; m_size[i] = size[i]; }
            m_sum = 0;
            m_cdf.resize(m_totsize+1);
            m_cdf[0] = 0.;
            for(int i=0; i<m_totsize; ++i) m_sum = m_cdf[i+1] = m_sum + pdf[i]; // assert(pdf[i]>=0)
            for(int i=0; i<m_totsize; ++i) m_cdf[i+1]/=m_sum;
        }
    private:
        std::vector<double> m_cdf;
        std::vector<int> m_size;
        int m_totsize;
        double m_sum;
    };


}; // namespace rjmcmc

#endif // RJMCMC_RASTER_VARIATE_HPP
