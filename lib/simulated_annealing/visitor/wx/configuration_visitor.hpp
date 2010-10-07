#ifndef __WX_CCONFIGURATION_VISITOR_HPP__
#define __WX_CCONFIGURATION_VISITOR_HPP__

#include <wx/wx.h>
#include <wx/statusbr.h>

#include <GilViewer/gui/layer_control.hpp>
#include <GilViewer/gui/basic_viewer_frame.hpp>
#include <GilViewer/convenient/macros_gilviewer.hpp>
#include <GilViewer/layers/image_types.hpp>

#include "simulated_annealing/visitor/controler.hpp"
#include "rjmcmc/variant.hpp"
#include "geometry/wx/paint.h"

// to be specialized by each object type (-> files lib/geometry/*_paint.h)
template<typename T> void paint(layer::ptrLayerType&, const std::string& s, const T&);

struct layer_painter {
public:
    layer_painter( layer::ptrLayerType& layer) : m_layer(layer) {}

    void energy(double e) { m_energy=e; }

    typedef void result_type;

    template<typename T> void operator()(const T& t) const {
        std::ostringstream oss;
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

        class configuration_visitor: public basic_viewer_frame
        {
        public:
            configuration_visitor(
                    wxWindow* parent,
                    wxWindowID id,
                    const wxString& title,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxDEFAULT_FRAME_STYLE,
                    const wxString& name = _("frame"));

            ~configuration_visitor() {}

            void OnGoButton   (wxCommandEvent&);
            void OnStopButton (wxCommandEvent&);
            void OnChartButton(wxCommandEvent&);
            void OnParamButton(wxCommandEvent&);

            void init(int dump, int save);

            template<typename Configuration, typename Sampler>
            void begin(const Configuration& config, const Sampler& sample, double t)
            {
                m_buttonGo  ->Disable();
                m_buttonStop->Enable();
            }
            template<typename Configuration, typename Sampler>
            void end(const Configuration& config, const Sampler& sample, double t)
            {
                m_buttonStop->Disable();
                m_buttonGo  ->Enable();
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
                    wxMutexGuiEnter();
                    m_vlayer->clear();
                    m_vlayer << config;
                    Refresh();
                    wxMutexGuiLeave();
                }
            }

            virtual wxAboutDialogInfo about_info() const;

            void controler(Controler *c) { m_controler=c; }

            panel_viewer* panelviewer() const;
            void add_layer(const std::string& file);
            wxRect get_bbox() const;
            void set_bbox(const wxRect& r);

            DECLARE_GILVIEWER_METHODS_FOR_EVENTS_TABLE() ;
            DECLARE_EVENT_TABLE() ;

        private:
            Controler    *m_controler;
            wxButton     *m_buttonGo;
            wxButton     *m_buttonStop;
            panel_viewer *m_panel;

            int m_dump;
            int m_save;
            int m_iter;
            layer::ptrLayerType m_vlayer;
        };

    } // namespace wx
} // namespace simulated_annealing

#endif // __WX_CCONFIGURATION_VISITOR_HPP__
