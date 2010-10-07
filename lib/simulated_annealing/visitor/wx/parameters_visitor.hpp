/*
 * ParametersFrame.h
 *
 *  Created on: 4 déc. 2008
 *      Author: olivier
 */

#ifndef __WX_PARAMETERS_VISITOR_HPP__
#define __WX_PARAMETERS_VISITOR_HPP__

#include <map>
#include <string>

#include <wx/frame.h>

namespace simulated_annealing {
    namespace wx {

        class parameters_visitor: public wxFrame
        {
        public:
            parameters_visitor(
                    wxWindow *parent = (wxWindow *) NULL,
                    wxWindowID id = wxID_ANY,
                    const wxString& title = _("librjmcmc parameters"),
                    long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxSize(600,400));

            virtual ~parameters_visitor() {}

            void init(int dump, int save) {}

            template<typename Configuration, typename Sampler>
            void begin(const Configuration& config, const Sampler& sampler, double t)
            {
                wxMutexGuiEnter();
                Refresh();
                Disable();
                wxMutexGuiLeave();
            }

            template<typename Configuration, typename Sampler>
            void end(const Configuration& config, const Sampler& sampler, double t)
            {
                wxMutexGuiEnter();
                Enable();
                Refresh();
                Show();
                wxMutexGuiLeave();
            }

            template<typename Configuration, typename Sampler>
            inline void visit(const Configuration& config, const Sampler& sampler, double) {}

            void on_file_parameter(wxCommandEvent& event);
            void on_bool_parameter(wxCommandEvent& event);

            void OnCloseWindow(wxCloseEvent&) { Hide(); }

        private:
            std::map<long,std::string> m_name_for_id;

            DECLARE_EVENT_TABLE();
        };

    } // namespace wx
} // namespace simulated_annealing

#endif // __WX_PARAMETERS_VISITOR_HPP__
