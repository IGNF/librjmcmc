#ifndef WX_VISITOR_HPP
#define WX_VISITOR_HPP

#include "visitor/wx/wx_log_visitor.hpp"

#include "visitor/wx/configuration_frame.hpp"

#include "visitor/wx/chart_frame.hpp"

#include "param/wx_parameter_traits.hpp"
#include "visitor/wx/wx_parameter_frame.hpp"

template<typename Configuration, typename Sampler>
class wx_visitor
{
public:
    void init(int dump, int save)
    {
        m_wx_log_visitor.init(dump,save);
        m_confg_frame->init(dump,save);
        m_chart_frame->init(dump,save);
        m_param_frame->init(dump,save);
    }

    void begin(const Configuration& config, const Sampler& sample, double t)
    {
        m_wx_log_visitor.begin(config,sample,t);
        m_confg_frame->begin(config,sample,t);
        m_chart_frame->begin(config,sample,t);
        m_param_frame->begin(config,sample,t);
    }

    bool iterate(const Configuration& config, const Sampler& sample, double t, unsigned int i)
    {
        bool b = true;
        b = m_wx_log_visitor.iterate(config,sample,t,i) && b;
        b = m_confg_frame->iterate(config,sample,t,i) && b;
        b = m_chart_frame->iterate(config,sample,t,i) && b;
        b = m_param_frame->iterate(config,sample,t,i) && b;
        return b;
    }

    void end(const Configuration& config, const Sampler& sample, double t)
    {
        m_wx_log_visitor.end(config,sample,t);
        m_confg_frame->end(config,sample,t);
        m_chart_frame->end(config,sample,t);
        m_param_frame->end(config,sample,t);
    }

    void toggle_param_visibility() { m_param_frame->Show(!m_param_frame->IsShown()); }
    void toggle_chart_visibility() { m_chart_frame->Show(!m_chart_frame->IsShown()); }

    template<typename View> void controler(Controler<View> *c) {
        m_confg_frame->controler(c);
    }

    wx_visitor(const std::string& title)
    {
        m_confg_frame = new configuration_frame((wxFrame *)NULL, wxID_ANY, wxString(title.c_str(), *wxConvCurrent) );
        m_param_frame = new parameters_frame(m_confg_frame);
        m_chart_frame = new chart_frame(m_confg_frame);

        m_confg_frame->Show();
        m_param_frame->Show();
        m_chart_frame->Show();
    }

    ~wx_visitor() {
        m_confg_frame->Destroy();
        m_param_frame->Destroy();
        m_chart_frame->Destroy();
    }
private:
    configuration_frame    *m_confg_frame;
    parameters_frame       *m_param_frame;
    chart_frame            *m_chart_frame;
    wx_log_visitor<Sampler> m_wx_log_visitor;
};

#endif // WX_VISITOR_HPP
