#ifndef __WX_CHART_VISITOR_HPP__
#define __WX_CHART_VISITOR_HPP__

class VectorDataset;

#include <boost/shared_ptr.hpp>
#include <wx/window.h>
#include <wx/toplevel.h>

namespace simulated_annealing {
    namespace wx {

        class chart_frame;

        class chart_visitor
        {
        public:
            chart_visitor(
                    wxWindow *parent = (wxWindow *) NULL,
                    wxWindowID id = wxID_ANY,
                    const wxString& title = _("librjmcmc charts"),
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);

            void init(int dump, int)
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
            void Show(bool b = true);
            bool IsShown() const;

        private:
            boost::shared_ptr<chart_frame> m_frame;
            VectorDataset *m_dataset[2];
            unsigned int m_dump, m_iter;

            void clear();
            void add(double e, double t);
        };

    } // namespace wx
} // namespace simulated_annealing

#endif // __WX_CHART_VISITOR_HPP__
