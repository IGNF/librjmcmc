#ifndef CHART_FRAME_HPP
#define CHART_FRAME_HPP

#include <wx/frame.h>

class VectorDataset;
class chart_frame: public wxFrame
{
public:
    chart_frame(
            wxWindow *parent = (wxWindow *) NULL,
            wxWindowID id = wxID_ANY,
            const wxString& title = _("librjmcmc charts"),
            long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize);

    void init(int dump, int save)
    {
        m_dump = dump;
        m_iter = 0;
    }

    template<typename Configuration, typename Sampler>
    void begin(const Configuration& config, const Sampler& sampler, double t)
    {
        clear();
        add(config.energy(),t);
    }
    template<typename Configuration, typename Sampler>
    void end(const Configuration& config, const Sampler& sampler, double t)
    {
        add(config.energy(),t);
    }

    template<typename Configuration, typename Sampler>
    void visit(const Configuration& config, const Sampler& sampler, double t) {
        if(++m_iter==m_dump)
        {
            m_iter = 0;
            add(config.energy(),t);
        }
    }

    void OnCloseWindow(wxCloseEvent&) { Hide(); }

private:
    VectorDataset *m_dataset[2];
    unsigned int m_dump, m_iter;

    void clear();
    void add(double e, double t);

    DECLARE_EVENT_TABLE();
};

#endif /* CHART_FRAME_HPP */
