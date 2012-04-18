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

#ifdef WIN32
	#pragma warning(disable : 4251)
	#pragma warning(disable : 4275)
#endif

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/filedlg.h>
#include <wx/scrolwin.h>
#include <wx/button.h>
#include <wx/icon.h>
#include <wx/frame.h>

#include "param/parameters_inc.hpp"
#include "wx_parameter.hpp"
#include "parameters_visitor.hpp"

#include "gui/resources/IGN.xpm"
#include <map>
#include <string>

#include <GilViewer/gui/layer_control.hpp>
#include <GilViewer/layers/image_layer.hpp>
#include <GilViewer/gui/panel_viewer.hpp>

const char *c_str(const wxString& s)
{
#if wxMAJOR_VERSION >=2 && wxMINOR_VERSION >= 9
	return s.To8BitData().data();
#else
	return s.To8BitData();
#endif
}
namespace simulated_annealing {
    namespace wx {

        class parameters_frame : public wxFrame
        {
            typedef std::vector<wxControlWithItems *> notified_control_type;
        public:
            parameters_frame(param *p, wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
                : wxFrame(parent,id,title,pos,size,style), m_param(p)
            {
                SetIcon(wxICON(IGN));
            }
            void build(layer_control *lc);

            void on_file_parameter(wxCommandEvent& event);
            void on_bool_parameter(wxCommandEvent& event);
            void on_save_parameter(wxCommandEvent& event);
            void on_load_parameter(wxCommandEvent& event);

            void on_close_window(wxCloseEvent&) { Hide(); }

            void name(long id, const std::string& name) { m_name[id] = name; }

        private:
            param *m_param;
            std::map<long,std::string> m_name;
            DECLARE_EVENT_TABLE();
        };


        void parameters_frame::on_load_parameter(wxCommandEvent& event)
        {
            wxFileDialog *fileDialog = new wxFileDialog(this, wxString("Loading Parameter file", *wxConvCurrent), wxT(""), wxT(""),  wxT("INI (*.ini)|*.ini;*.INI"), wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR );
            if (fileDialog->ShowModal() == wxID_OK)
            {
                update_values(m_param);
                m_param->parse(c_str(fileDialog->GetPath()));
                update_controls(m_param);
                Refresh();
            }
        }

        void parameters_frame::on_save_parameter(wxCommandEvent& event)
        {
            wxFileDialog *fileDialog = new wxFileDialog(this, wxString("Saving Parameter file", *wxConvCurrent), wxT(""), wxT(""),  wxT("INI (*.ini)|*.ini;*.INI"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT|wxFD_CHANGE_DIR );
            if (fileDialog->ShowModal() == wxID_OK)
            {
                update_values(m_param);
                m_param->save(c_str(fileDialog->GetPath()));
            }
        }

        void parameters_frame::on_file_parameter(wxCommandEvent& event)
        {
            wxString wildcard;
            wildcard << _("Image files ");
            wildcard << wxT("(*.tif;*.tiff;*.png;*.jpg;*.jpeg)|*.tif;*.tiff;*.TIF;*.TIFF;*.png;*.PNG;*.jpg;*.jpeg;*.JPG;*.JPEG|");
            wildcard << wxT("TIFF (*.tif;*.tiff;*.TIF;*.TIFF)|*.tif;*.tiff;*.TIF;*.TIFF|");
            wildcard << wxT("PNG (*.png;*.PNG)|*.png;*.PNG|");
            wildcard << wxT("JPEG (*.jpg;*.jpeg;*.JPG;*.JPEG)|*.jpg;*.jpeg;*.JPG;*.JPEG|");
            std::string name = m_name[event.GetId()];
            wxFileDialog *fileDialog = new wxFileDialog(this, wxString(name.c_str(), *wxConvCurrent), wxT(""), wxT(""), wildcard, wxFD_OPEN|wxFD_CHANGE_DIR );
            if (fileDialog->ShowModal() == wxID_OK)
            {
                boost::filesystem::path file( c_str(fileDialog->GetPath()) );
                m_param->set(name,file);
            }
        }

        void parameters_frame::on_bool_parameter(wxCommandEvent& event)
        {
            m_param->set(m_name[event.GetId()],event.IsChecked());
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
                oss.precision(std::numeric_limits<double>::digits10+1);
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

        void parameters_frame::build(layer_control *lc)
        {
            wxBoxSizer* inner_sizer = new wxBoxSizer(wxVERTICAL);

            wxScrolledWindow* scroll = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxScrolledWindowStyle|wxTAB_TRAVERSAL, wxT("scroll"));
            scroll->SetScrollbars(20,20,50,50);

            wxFlexGridSizer* sizer = new wxFlexGridSizer(2);
            sizer->AddGrowableCol(1);
            sizer->AddSpacer(10);
            sizer->AddSpacer(10);

            wxString desc_save("Save", *wxConvCurrent);
            wxString desc_load("Load", *wxConvCurrent);
            wxButton *save_button = new wxButton(scroll,wxID_ANY,desc_save);
            wxButton *load_button = new wxButton(scroll,wxID_ANY,desc_load);
            sizer->Add(save_button, 0,wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
            sizer->Add(load_button, 0,wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
            save_button->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED,
                                 wxCommandEventHandler(parameters_frame::on_save_parameter), NULL, this);
            load_button->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED,
                                 wxCommandEventHandler(parameters_frame::on_load_parameter), NULL, this);

      //      load_button->Disable(); // NOT operational yet... makes the app crash

            for(param::iterator it=m_param->begin(); it!=m_param->end(); ++it)
            {
                sizer_adder<param::parameter_t> adder(this,lc,sizer,scroll,*it);
                boost::apply_visitor(adder,it->value());
            }

            scroll->SetSizer(sizer);
            scroll->SetAutoLayout(true);
            scroll->Layout();

            inner_sizer->Add(scroll, 1, wxEXPAND | wxALL, 5);
            SetSizer(inner_sizer);
            SetAutoLayout(true);
            Refresh();

        }


        parameters_visitor::parameters_visitor(panel_viewer *panel, param *p, wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
            : m_frame(new parameters_frame(p, parent, id, title, pos, size, style))
            , m_panel(panel)
        {
            m_frame->build(m_panel->layercontrol());
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
