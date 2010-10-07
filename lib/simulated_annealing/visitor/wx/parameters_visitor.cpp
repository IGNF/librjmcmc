/*
 * ParametersFrame.cpp
 *
 *  Created on: 4 déc. 2008
 *      Author: olivier
 */

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/filedlg.h>
#include <wx/scrolwin.h>
#include <wx/button.h>
#include <wx/icon.h>

#include "param/wx_parameter.hpp"
#include "parameters_visitor.hpp"
typedef parameters< wx_parameter > param;

#include "resources/IGN.xpm"

namespace simulated_annealing {
    namespace wx {

        BEGIN_EVENT_TABLE(parameters_visitor, wxFrame)
                EVT_CLOSE(parameters_visitor::OnCloseWindow)
                END_EVENT_TABLE();

        template<typename P>
        struct sizer_adder
        {
            typedef void result_type;
            wxSizer *m_sizer;
            wxWindow *m_wnd;
            parameters_visitor *m_visitor;
            std::map<long,std::string>& m_name;
            P& m_p;

            sizer_adder(parameters_visitor *frame, wxSizer *sizer, wxWindow* wnd, std::map<long,std::string>& name, P& p)
                : m_visitor(frame), m_sizer(sizer), m_wnd(wnd), m_name(name), m_p(p) {}

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
                                  wxCommandEventHandler(parameters_visitor::on_bool_parameter), NULL, m_visitor);
                m_name[id]=m_p.name();
            }

            void operator()(const boost::filesystem::path& p)
            {
                wxString s(p.string().c_str(), *wxConvCurrent);
                wxTextCtrl   *ctrl = new wxTextCtrl(m_wnd, wxID_ANY, s);
                m_p.control(ctrl);
                long id = wxNewId();
                wxString desc((m_p.description()+" ...").c_str(), *wxConvCurrent);
                wxButton *browse_button = new wxButton(m_wnd,id,desc);
                browse_button->Connect(id, wxEVT_COMMAND_BUTTON_CLICKED,
                                       wxCommandEventHandler(parameters_visitor::on_file_parameter), NULL, m_visitor);
                m_name[id]=m_p.name();
                m_sizer->Add(browse_button, 0,wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
                m_sizer->Add(ctrl, 0, wxEXPAND|wxALL, 3);
            }
        };

        parameters_visitor::parameters_visitor(wxWindow *parent, wxWindowID id, const wxString& title, long style, const wxPoint& pos, const wxSize& size) :
                wxFrame(parent, id, title, pos, size, style)
        {
            SetIcon(wxICON(IGN));
            param *p = param::instance();

            wxBoxSizer* inner_sizer = new wxBoxSizer(wxVERTICAL);

            wxScrolledWindow* scroll = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxScrolledWindowStyle|wxTAB_TRAVERSAL, wxT("scroll"));
            scroll->SetScrollbars(20,20,50,50);

            wxFlexGridSizer* sizer = new wxFlexGridSizer(2);
            sizer->AddGrowableCol(1);
            sizer->AddSpacer(10);
            sizer->AddSpacer(10);

            for(param::iterator it=p->begin(); it!=p->end(); ++it)
            {
                sizer_adder<param::parameter_t> adder(this,sizer,scroll,m_name_for_id,*it);
                boost::apply_visitor(adder,it->value());
            }

            scroll->SetSizer(sizer);
            scroll->SetAutoLayout(true);
            scroll->Layout();

            inner_sizer->Add(scroll, 1, wxEXPAND | wxALL, 5);
            SetSizer(inner_sizer);
            SetAutoLayout(true);
            Refresh();
            Show();
        }

        void parameters_visitor::on_file_parameter(wxCommandEvent& event)
        {
            wxString wildcard;
            wildcard << _("Image files ");
            wildcard << wxT("(*.tif;*.tiff;*.png;*.jpg;*.jpeg)|*.tif;*.tiff;*.TIF;*.TIFF;*.png;*.PNG;*.jpg;*.jpeg;*.JPG;*.JPEG|");
            wildcard << wxT("TIFF (*.tif;*.tiff;*.TIF;*.TIFF)|*.tif;*.tiff;*.TIF;*.TIFF|");
            wildcard << wxT("PNG (*.png;*.PNG)|*.png;*.PNG|");
            wildcard << wxT("JPEG (*.jpg;*.jpeg;*.JPG;*.JPEG)|*.jpg;*.jpeg;*.JPG;*.JPEG|");
            wxString str;
            std::string name = m_name_for_id[event.GetId()];
            wxFileDialog *fileDialog = new wxFileDialog(this, wxString(name.c_str(), *wxConvCurrent), wxT(""), wxT(""), wildcard, wxFD_OPEN|wxFD_CHANGE_DIR );
            if (fileDialog->ShowModal() == wxID_OK)
            {
                boost::filesystem::path file( fileDialog->GetPath().To8BitData() );
                param::instance()->set(name,file);
            }
        }

        void parameters_visitor::on_bool_parameter(wxCommandEvent& event)
        {
            param::instance()->set(m_name_for_id[event.GetId()],event.IsChecked());
        }

    } // namespace wx
} // namespace simulated_annealing
