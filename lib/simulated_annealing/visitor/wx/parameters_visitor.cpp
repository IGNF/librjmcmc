/*
 * ParametersFrame.cpp
 *
 *  Created on: 4 déc. 2008
 *      Author: olivier
 */

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/filedlg.h>
#include <wx/scrolwin.h>
#include <wx/button.h>
#include <wx/icon.h>
#include <wx/frame.h>

#include "param/wx_parameter.hpp"
#include "parameters_visitor.hpp"
typedef parameters< wx_parameter > param;

#include "gui/resources/IGN.xpm"
#include <map>
#include <string>

#include <GilViewer/gui/layer_control.hpp>
#include <GilViewer/layers/image_layer.hpp>
#include <GilViewer/gui/panel_viewer.hpp>

namespace simulated_annealing {
    namespace wx {

        class parameters_frame : public wxFrame
        {
            typedef std::vector<wxControlWithItems *> notified_control_type;
        public:
            parameters_frame(wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
                : wxFrame(parent,id,title,pos,size,style)
            {
            }

            void on_file_parameter(wxCommandEvent& event);
            void on_bool_parameter(wxCommandEvent& event);

            void on_close_window(wxCloseEvent&) { Hide(); }

            void name(long id, const std::string& name) { m_name[id] = name; }

        private:
            std::map<long,std::string> m_name;
            DECLARE_EVENT_TABLE();
        };


        void parameters_frame::on_file_parameter(wxCommandEvent& event)
        {
            wxString wildcard;
            wildcard << _("Image files ");
            wildcard << wxT("(*.tif;*.tiff;*.png;*.jpg;*.jpeg)|*.tif;*.tiff;*.TIF;*.TIFF;*.png;*.PNG;*.jpg;*.jpeg;*.JPG;*.JPEG|");
            wildcard << wxT("TIFF (*.tif;*.tiff;*.TIF;*.TIFF)|*.tif;*.tiff;*.TIF;*.TIFF|");
            wildcard << wxT("PNG (*.png;*.PNG)|*.png;*.PNG|");
            wildcard << wxT("JPEG (*.jpg;*.jpeg;*.JPG;*.JPEG)|*.jpg;*.jpeg;*.JPG;*.JPEG|");
            wxString str;
            std::string name = m_name[event.GetId()];
            wxFileDialog *fileDialog = new wxFileDialog(this, wxString(name.c_str(), *wxConvCurrent), wxT(""), wxT(""), wildcard, wxFD_OPEN|wxFD_CHANGE_DIR );
            if (fileDialog->ShowModal() == wxID_OK)
            {
                boost::filesystem::path file( fileDialog->GetPath().To8BitData() );
                param::instance()->set(name,file);
            }
        }

        void parameters_frame::on_bool_parameter(wxCommandEvent& event)
        {
            param::instance()->set(m_name[event.GetId()],event.IsChecked());
        }

        BEGIN_EVENT_TABLE(parameters_frame, wxFrame)
                EVT_CLOSE(parameters_frame::on_close_window)
                END_EVENT_TABLE();

        template<typename P>
        struct sizer_adder
        {
            typedef void result_type;
            wxSizer *m_sizer;
            wxWindow *m_wnd;
            layer_control * m_lc;
            parameters_frame *m_frame;
            P& m_p;

            sizer_adder(parameters_frame *frame, layer_control* lc, wxSizer *sizer, wxWindow* wnd, P& p)
                : m_frame(frame), m_lc(lc), m_sizer(sizer), m_wnd(wnd), m_p(p) {}

            void AddText(wxSizer *sizer)
            {
                wxString s(m_p.description().c_str(), *wxConvCurrent);
                wxStaticText *text = new wxStaticText(m_wnd, wxID_ANY, s);
                sizer->Add(text, 0,wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
            }

            template<typename T>
            void operator()(const T& t)
            {
                AddText(m_sizer);
                std::ostringstream oss;
                oss << t;
                wxString s(oss.str().c_str(), *wxConvCurrent);
                wxTextCtrl   *ctrl = new wxTextCtrl(m_wnd, wxID_ANY, s);
                m_p.control(ctrl);
                m_sizer->Add(ctrl, 0, wxEXPAND|wxALL, 3);
            }

            void operator()(bool b)
            {
                AddText(m_sizer);
                long id = wxNewId();
                wxCheckBox *checkbox = new wxCheckBox(m_wnd, id, wxEmptyString);
                m_p.control(checkbox);
                checkbox->SetValue(b);
                m_sizer->Add(checkbox, 0, wxEXPAND|wxALL, 3);
                checkbox->Connect(id, wxEVT_COMMAND_CHECKBOX_CLICKED,
                                  wxCommandEventHandler(parameters_frame::on_bool_parameter), NULL, m_frame);
                m_frame->name(id,m_p.name());
            }

            void operator()(const boost::filesystem::path& p)
            {
                /*                wxString s(p.string().c_str(), *wxConvCurrent);
                wxTextCtrl   *ctrl = new wxTextCtrl(m_wnd, wxID_ANY, s);
                m_p.control(ctrl);
                long id = wxNewId();
                wxString desc((m_p.description()+" ...").c_str(), *wxConvCurrent);
                wxButton *browse_button = new wxButton(m_wnd,id,desc);
                browse_button->Connect(id, wxEVT_COMMAND_BUTTON_CLICKED,
                                       wxCommandEventHandler(parameters_frame::on_file_parameter), NULL, m_frame);
                m_frame->name(id,m_p.name());
                m_sizer->Add(browse_button, 0,wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
                m_sizer->Add(ctrl, 0, wxEXPAND|wxALL, 3);
*/

                wxChoice* ctrl = new wxChoice(m_wnd, wxID_ANY);
                ctrl->Append(wxString(p.string().c_str(), *wxConvCurrent));
                m_lc->update_control<image_layer>(ctrl);
                m_p.control(ctrl);
                long id = wxNewId();
                wxString desc((m_p.description()+" ...").c_str(), *wxConvCurrent);
                wxButton *browse_button = new wxButton(m_wnd,id,desc);
                browse_button->Connect(id, wxEVT_COMMAND_BUTTON_CLICKED,
                                       wxCommandEventHandler(parameters_frame::on_file_parameter), NULL, m_frame);
                m_frame->name(id,m_p.name());
                m_sizer->Add(browse_button, 0,wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
                m_sizer->Add(ctrl, 0, wxEXPAND|wxALL, 3);
            }
        };

        parameters_visitor::parameters_visitor(panel_viewer *panel, wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
            : m_frame(new parameters_frame(parent, id, title, pos, size, style))
            , m_panel(panel)
        {

            m_frame->SetIcon(wxICON(IGN));
            param *p = param::instance();

            wxBoxSizer* inner_sizer = new wxBoxSizer(wxVERTICAL);

            wxScrolledWindow* scroll = new wxScrolledWindow(m_frame.get(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxScrolledWindowStyle|wxTAB_TRAVERSAL, wxT("scroll"));
            scroll->SetScrollbars(20,20,50,50);

            wxFlexGridSizer* sizer = new wxFlexGridSizer(2);
            sizer->AddGrowableCol(1);
            sizer->AddSpacer(10);
            sizer->AddSpacer(10);

            for(param::iterator it=p->begin(); it!=p->end(); ++it)
            {
                sizer_adder<param::parameter_t> adder(m_frame.get(),m_panel->layercontrol(),sizer,scroll,*it);
                boost::apply_visitor(adder,it->value());
            }

            scroll->SetSizer(sizer);
            scroll->SetAutoLayout(true);
            scroll->Layout();

            inner_sizer->Add(scroll, 1, wxEXPAND | wxALL, 5);
            m_frame->SetSizer(inner_sizer);
            m_frame->SetAutoLayout(true);
            m_frame->Refresh();
            m_frame->Show();
        }

        void parameters_visitor::Enable(bool b)
        {
            wxMutexGuiEnter();
            m_frame->Enable(b);
            m_frame->Refresh();
            wxMutexGuiLeave();

        }
        void parameters_visitor::Show(bool b) { m_frame->Show(b); }
        bool parameters_visitor::IsShown() const { return m_frame->IsShown(); }
    } // namespace wx
} // namespace simulated_annealing
