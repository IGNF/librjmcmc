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

#ifndef SHP_VISITOR_HPP
#define SHP_VISITOR_HPP

#include <string>
#include <shapefil.h>

#ifdef  GEOMETRY_RECTANGLE_2_HPP
template<typename K>
SHPObject* shp_create_object(const geometry::Rectangle_2<K>& r)
{
    double x[5], y[5];
    for(unsigned int i=0; i<4; ++i)
    {
        typename geometry::Rectangle_2<K>::Point_2 p = r.point(i);
        x[i] = geometry::to_double(p.x());
        y[i] = geometry::to_double(p.y());
    }
    x[4]=x[0];
    y[4]=y[0];

    return SHPCreateSimpleObject(SHPT_POLYGON, 5, x, y, NULL);
}
#endif //  GEOMETRY_RECTANGLE_2_HPP

#ifdef  GEOMETRY_CIRCLE_2_HPP
template<typename K>
SHPObject* shp_create_object(const geometry::Circle_2<K>& c)
{
    enum { N=20 };
    double x[N+1], y[N+1];
    typename geometry::Circle_2<K>::Point_2 p = c.center();
    double x0 = geometry::to_double(p.x());
    double y0 = geometry::to_double(p.y());
    double r  = geometry::to_double(c.radius());
    for(unsigned int i=0; i<N; ++i)
    {
        double theta = (2*M_PI*i)/N;
        x[i] = x0+r*cos(theta);
        y[i] = y0+r*sin(theta);
    }
    x[N]=x[0];
    y[N]=y[0];

    return SHPCreateSimpleObject(SHPT_POLYGON, N+1, x, y, NULL);
}
#endif // GEOMETRY_CIRCLE_2_HPP

struct shp_writer
{

public:
    shp_writer(const std::string& file) { m_file = SHPCreate(file.c_str(), SHPT_POLYGON); }
    ~shp_writer() { SHPClose(m_file); }

    typedef void result_type;

    inline operator bool() { return m_file; }

    template<typename T> void operator()(const T& t) const {
        SHPObject* object = shp_create_object(t);
        SHPComputeExtents(object);
        SHPWriteObject( m_file, -1, object);
    }

private:
    SHPHandle m_file;
};

#include <iostream>
#include <iomanip>
#include <sstream>
#include "rjmcmc/variant.hpp"

namespace simulated_annealing {
    namespace shp {

        class shp_visitor
        {
        public:
            shp_visitor(const std::string& prefix) : m_iter(0), m_prefix(prefix) {}

            void init(int, int s)
            {
                m_save = s;
                m_iter = 0;
            }

            template<typename Configuration, typename Sampler>
            void begin(const Configuration& config, const Sampler&, double)
            {
                m_iter = 0;
                save(config);
            }

            template<typename Configuration, typename Sampler>
            void end(const Configuration& config, const Sampler&, double)
            {
                save(config);
            }

            template<typename Configuration, typename Sampler>
            void visit(const Configuration& config, const Sampler&, double) {
                if((++m_iter)%m_save==0)
                    save(config);
            }
        private:
            unsigned int m_save, m_iter;
            std::string m_prefix;

            template<typename Configuration>
            void save(const Configuration& config) const
            {
                std::ostringstream oss;
                oss << m_prefix << std::setw(15) << std::setfill('0') << m_iter << ".shp";
                shp_writer writer(oss.str());
                if(!writer)
                {
                    std::cout << "\tUnable to create SHP " << oss.str() << std::endl;
                    return;
                }
                typename Configuration::const_iterator it = config.begin(), end = config.end();
                for (; it != end; ++it)
                {
                    rjmcmc::apply_visitor(writer,config[it]);
                }
            }
        };

    } // namespace shp
} // namespace simulated_annealing

#endif // SHP_VISITOR_HPP

