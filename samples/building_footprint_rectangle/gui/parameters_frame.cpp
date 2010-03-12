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
#include "parameters_frame.hpp"

#include "param/wx_parameter_traits.hpp"

struct sizer_adder {
	typedef void result_type;
	wxSizer *m_sizer;
	wxWindow *m_wnd;
	parameters_frame *m_frame;
	std::map<long,std::string>& m_name;
	
	sizer_adder(parameters_frame *frame, wxSizer  *sizer, wxWindow* wnd, std::map<long,std::string>& name)
		: m_frame(frame), m_sizer(sizer), m_wnd(wnd), m_name(name) {}

	template<typename T>
	void AddText(wxSizer *sizer, const parameter<wx_parameter_traits,T>& p) {
		wxString s(p.description().c_str(), *wxConvCurrent);
		wxStaticText *text = new wxStaticText(m_wnd, wxID_ANY, s);
                sizer->Add(text, 1, wxEXPAND|wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5);
	}

	template<typename T>
	void operator()(parameter<wx_parameter_traits,T>& p) {
		AddText(m_sizer,p);
		std::ostringstream oss;
		oss << p.value();
		wxString s(oss.str().c_str(), *wxConvCurrent);
		wxTextCtrl   *ctrl = new wxTextCtrl(m_wnd, wxID_ANY, s);
		p.control(ctrl);
                m_sizer->Add(ctrl,1, wxEXPAND|wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5);
	}

	void operator()(parameter<wx_parameter_traits,bool>& p) {
		AddText(m_sizer,p);
		long id = wxNewId();
		wxCheckBox *checkbox = new wxCheckBox(m_wnd, id, wxEmptyString);
		p.control(checkbox);
		checkbox->SetValue(p.value());
                m_sizer->Add(checkbox, 1, wxEXPAND|wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5);
		checkbox->Connect(id, wxEVT_COMMAND_CHECKBOX_CLICKED,
	wxCommandEventHandler(parameters_frame::on_bool_parameter), NULL, m_frame);
		m_name[id]=p.name();
	}

	void operator()(parameter<wx_parameter_traits,boost::filesystem::path>& p) {
		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		wxString s(p.value().string().c_str(), *wxConvCurrent);
		wxTextCtrl   *ctrl = new wxTextCtrl(m_wnd, wxID_ANY, s);
		p.control(ctrl);
		long id = wxNewId();
                wxButton *browse_button = new wxButton(m_wnd,id,wxT("Browse"),wxDefaultPosition,wxSize(100,25));
		browse_button->Connect(id, wxEVT_COMMAND_BUTTON_CLICKED,
	wxCommandEventHandler(parameters_frame::on_file_parameter), NULL, m_frame);
                m_name[id]=p.name();
		AddText(sizer,p);
                m_sizer->Add(sizer,1, wxEXPAND|wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5);

                wxBoxSizer* sizer_browse = new wxBoxSizer(wxHORIZONTAL);
                sizer_browse->Add(ctrl ,1, wxEXPAND|wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5);
                sizer_browse->Add(browse_button,0, wxFIXED_MINSIZE|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5);
                m_sizer->Add(sizer_browse,1, wxEXPAND|wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5);
	}
};

parameters_frame::parameters_frame(wxWindow *parent, wxWindowID id, const wxString& title, long style, const wxPoint& pos, const wxSize& size) :
	wxFrame(parent, id, title, pos, size, style)
{
	param *p = param::Instance();
        wxPanel* panel= new wxPanel(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL);
        wxBoxSizer* s = new wxBoxSizer(wxVERTICAL);
        s->Add(panel,5,wxEXPAND,wxALL);
	wxFlexGridSizer* sizer = new wxFlexGridSizer(2);
        panel->SetSizer(sizer);
	sizer->AddGrowableCol(1);
	sizer->AddSpacer(10);
	sizer->AddSpacer(10);
        sizer_adder adder(this,sizer,panel,m_name_for_id);
	for(param::iterator it=p->begin(); it!=p->end(); ++it)
                boost::apply_visitor(adder,*it);
        //AddChild(panel);
        SetSizer(s);
	SetAutoLayout(true);
	Refresh();
}

void parameters_frame::on_file_parameter(wxCommandEvent& event)
{
    // TODO: get wildcard from gilviewer static methods (to be implemented)
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
		param::Instance()->set(name,file);
	}
}

void parameters_frame::on_bool_parameter(wxCommandEvent& event)
{
	param::Instance()->set(m_name_for_id[event.GetId()],event.IsChecked());
}
