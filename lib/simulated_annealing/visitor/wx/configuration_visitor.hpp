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

#ifndef WX_CCONFIGURATION_VISITOR_HPP
#define WX_CCONFIGURATION_VISITOR_HPP

#include <wx/wx.h>
#include <wx/aboutdlg.h>
#include "rjmcmc/variant.hpp"
#include "paint/all.hpp"
#include <GilViewer/gui/panel_viewer.hpp>

struct layer_painter {
public:
    layer_painter( layer::ptrLayerType& layer) : m_layer(layer) {}

    void energy(double e) { m_energy=e; }

    typedef void result_type;

    template<typename T> void operator()(const T& t) const {
        std::ostringstream oss;
        oss.precision(std::numeric_limits<double>::digits10+1);
        oss << m_energy;
        paint(m_layer,oss.str(),t);
    }

private:
    layer::ptrLayerType& m_layer;
    double m_energy;
};


template<typename Config>
layer::ptrLayerType& operator<<(layer::ptrLayerType& layer, const Config& config)
{
    typename Config::const_iterator it = config.begin(), end = config.end();
    layer_painter painter(layer);
    for (; it != end; ++it)
    {
        painter.energy(config.energy(it));
        rjmcmc::apply_visitor(painter,config[it]);
    }
    return layer;
}

namespace simulated_annealing {
    namespace wx {

        class configuration_visitor
        {
        public:
            configuration_visitor(panel_viewer *panel);

            void init(int dump, int save);

            template<typename Configuration, typename Sampler>
            void begin(const Configuration& config, const Sampler&, double)
            {
                paint(config);
            }

            template<typename Configuration, typename Sampler>
            void end(const Configuration& config, const Sampler&, double)
            {
                paint(config);
            }

            template<typename Configuration, typename Sampler>
            void visit(const Configuration& config, const Sampler&, double)
            {
                ++m_iter;
                if ( m_save && (m_iter % m_save == 0) )
                {
                    std::ostringstream out;
                    out.width(8);
                    out.fill('0');
                    out << m_iter;
                    m_vlayer->save( out.str() + ".shp" );
                }
                if ( m_dump && (m_iter % m_dump == 0) )
                {
                    paint(config);
                }
            }

            virtual wxAboutDialogInfo about_info() const;

            panel_viewer* panelviewer() const;

            wxRect get_bbox() const;
            void set_bbox(const wxRect& r);

        private:
            panel_viewer *m_panel;

            int m_dump;
            int m_save;
            int m_iter;
            layer::ptrLayerType m_vlayer;


            template<typename Configuration>
            void paint(const Configuration& config)
            {
                wxMutexGuiEnter();
                m_vlayer->clear();
                m_vlayer << config;
                wxMutexGuiLeave();
                m_panel->Refresh();
            }
        };

    } // namespace wx
} // namespace simulated_annealing

#endif // WX_CCONFIGURATION_VISITOR_HPP
