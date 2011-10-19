/*
 * ParametersFrame.h
 *
 *  Created on: 4 déc. 2008
 *      Author: olivier
 */

#ifndef __WX_PARAMETERS_VISITOR_HPP__
#define __WX_PARAMETERS_VISITOR_HPP__

#include <boost/shared_ptr.hpp>
#include <wx/window.h>
#include <wx/toplevel.h>

namespace simulated_annealing {
    namespace wx {

        class parameters_frame;

        class parameters_visitor
        {
        public:
            parameters_visitor(
                    wxWindow *parent = (wxWindow *) NULL,
                    wxWindowID id = wxID_ANY,
                    const wxString& title = _("librjmcmc parameters"),
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxSize(600,400),
                    long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);

            virtual ~parameters_visitor() {}

            void init(int dump, int save) {}

            template<typename Configuration, typename Sampler>
            void begin(const Configuration& config, const Sampler& sampler, double t)
            {
                Enable(false);
            }

            template<typename Configuration, typename Sampler>
            void end(const Configuration& config, const Sampler& sampler, double t)
            {
                Enable(true);
            }

            template<typename Configuration, typename Sampler>
            inline void visit(const Configuration& config, const Sampler& sampler, double) {}

            void Enable(bool b);
            void Show(bool b = true);
            bool IsShown() const;

        private:
            boost::shared_ptr<parameters_frame> m_frame;
        };

    } // namespace wx
} // namespace simulated_annealing

#endif // __WX_PARAMETERS_VISITOR_HPP__
