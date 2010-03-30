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

#include "param/wx_parameter_traits.hpp"
#include "param/wx_parameter_frame.hpp"

BEGIN_EVENT_TABLE(parameters_frame, wxFrame)
    EVT_CLOSE(parameters_frame::OnCloseWindow)
END_EVENT_TABLE()

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
		sizer->Add(text, 0,wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	}

	template<typename T>
	void operator()(parameter<wx_parameter_traits,T>& p) {
		AddText(m_sizer,p);
		std::ostringstream oss;
		oss << p.value();
		wxString s(oss.str().c_str(), *wxConvCurrent);
		wxTextCtrl   *ctrl = new wxTextCtrl(m_wnd, wxID_ANY, s);
		p.control(ctrl);
		m_sizer->Add(ctrl,0,wxEXPAND);
	}

	void operator()(parameter<wx_parameter_traits,bool>& p) {
		AddText(m_sizer,p);
		long id = wxNewId();
		wxCheckBox *checkbox = new wxCheckBox(m_wnd, id, wxEmptyString);
		p.control(checkbox);
		checkbox->SetValue(p.value());
		m_sizer->Add(checkbox);
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
		wxButton *browse_button = new wxButton(m_wnd,id,wxT("..."),wxDefaultPosition,wxSize(25,25));
		browse_button->Connect(id, wxEVT_COMMAND_BUTTON_CLICKED,
	wxCommandEventHandler(parameters_frame::on_file_parameter), NULL, m_frame);
		m_name[id]=p.name();
		sizer->Add(browse_button);
		AddText(sizer,p);
		m_sizer->Add(sizer,0,wxALL);
		m_sizer->Add(ctrl ,0,wxEXPAND);
	}
};

parameters_frame::parameters_frame(wxWindow *parent, wxWindowID id, const wxString& title, long style, const wxPoint& pos, const wxSize& size) :
	wxFrame(parent, id, title, pos, size, style)
{
	param *p = param::Instance();
	wxFlexGridSizer* sizer = new wxFlexGridSizer(2);
	sizer->AddGrowableCol(1);
	sizer->AddSpacer(10);
	sizer->AddSpacer(10);
	sizer_adder adder(this,sizer,this,m_name_for_id);
	for(param::iterator it=p->begin(); it!=p->end(); ++it)
		boost::apply_visitor(adder,*it);
	SetSizer(sizer);
	SetAutoLayout(true);
	Refresh();
}

void parameters_frame::on_file_parameter(wxCommandEvent& event)
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
		param::Instance()->set(name,file);
	}
}

void parameters_frame::on_bool_parameter(wxCommandEvent& event)
{
	param::Instance()->set(m_name_for_id[event.GetId()],event.IsChecked());
}
