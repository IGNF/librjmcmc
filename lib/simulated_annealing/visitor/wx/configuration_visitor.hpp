#ifndef __WX_CCONFIGURATION_VISITOR_HPP__
#define __WX_CCONFIGURATION_VISITOR_HPP__

#include <wx/wx.h>
#include <wx/aboutdlg.h>
#include "rjmcmc/variant.hpp"
#include "geometry/wx/paint.h"
#include <GilViewer/gui/panel_viewer.hpp>

// to be specialized by each object type (-> files lib/geometry/*_paint.h)

template<typename T> void paint(layer::ptrLayerType&, const std::string& s, const T&);

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
                m_panel->Refresh();
                wxMutexGuiLeave();
            }
        };

    } // namespace wx
} // namespace simulated_annealing

#endif // __WX_CCONFIGURATION_VISITOR_HPP__
