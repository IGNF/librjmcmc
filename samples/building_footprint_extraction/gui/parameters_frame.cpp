/*
 * ParametersFrame.cpp
 *
 *  Created on: 4 déc. 2008
 *      Author: olivier
 */

#include <wx/sizer.h>
#include "parameters_frame.hpp"

#include "core/building_footprint_extraction_parameters.hpp"
typedef building_footprint_extraction_parameters param;

#define ID_CHECK_SAVE	1


struct param_frame_writer {
	param::iterator m_it;
	param_frame_writer(param::iterator it) : m_it(it) {}
	typedef void result_type;
	template<typename T> void operator()(T& t) const {
		std::ostringstream oss;
		oss << t;
		m_it->control<wxTextCtrl>()->SetValue(wxString(oss.str().c_str(), *wxConvCurrent));
	}
	void operator() (bool& b) const {
		m_it->control<wxCheckBox>()->SetValue(b);
	}
};

void parameters_frame::Refresh() {
	param *p = param::Instance();
	for(param::iterator it=p->begin(); it!=p->end(); ++it)
		boost::apply_visitor(param_frame_writer(it),it->value());
	Update();
}

struct sizer_adder {
	typedef void result_type;
	param::iterator m_it;
	wxSizer *m_sizer;
	wxWindow *m_wnd;
	parameters_frame *m_frame;
	
	sizer_adder(wxSizer  *sizer, wxWindow* wnd, parameters_frame *frame)
		: m_sizer(sizer), m_wnd(wnd), m_frame(frame) {}

	void set(param::iterator it) { m_it=it; }

	template<typename T> void operator()(const T& t) const {
		std::ostringstream oss;
		oss << t;
		wxTextCtrl   *ctrl = new wxTextCtrl(m_wnd, wxID_ANY, wxString(oss.str().c_str(), *wxConvCurrent));
		m_it->control(ctrl);
		m_sizer->Add(ctrl, 1, wxEXPAND);
	}


	void operator()(bool b) const {
		wxCheckBox *checkbox = new wxCheckBox(m_wnd, ID_CHECK_SAVE, wxEmptyString );
		m_it->control(checkbox);
		checkbox->SetValue(b);
		m_sizer->Add(checkbox, wxEXPAND);
		// todo : generalize this "dosave"-specific connection
		checkbox->Connect(ID_CHECK_SAVE, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(parameters_frame::on_check_save), NULL, m_frame);
	}

	void operator()(const boost::filesystem::path& p) const {
		wxTextCtrl   *ctrl = new wxTextCtrl(m_wnd, wxID_ANY, wxString(p.string().c_str(), *wxConvCurrent));
		m_it->control(ctrl);
		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		wxButton *browse_input_button = new wxButton(m_wnd,wxID_OPEN,wxT("..."),wxDefaultPosition,wxSize(25,25));
		sizer->Add(browse_input_button,  wxALL);
		sizer->Add(ctrl, wxEXPAND);
		// todo : generalize this "input"-specific connection
		browse_input_button->Connect(wxID_OPEN, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(parameters_frame::on_browse_input_button), NULL, m_frame);
		m_sizer->Add(sizer, wxEXPAND);
	}
};

parameters_frame::parameters_frame(wxWindow *parent, wxWindowID id, const wxString& title, long style, const wxPoint& pos, const wxSize& size) :
	wxFrame(parent, id, title, pos, size, style)
{
	param *p = param::Instance();
	wxScrolledWindow* wnd = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxT("scroll"));
	wnd->SetScrollbars(20, 20, 50, 50);
	
	wxFlexGridSizer* sizer = new wxFlexGridSizer(2);
	sizer->AddSpacer(10);
	sizer->AddSpacer(10);
	sizer_adder adder(sizer,wnd,this);
	for(param::iterator it=p->begin(); it!=p->end(); ++it)
	{
		wxStaticText *text = new wxStaticText(wnd, wxID_ANY, wxString(it->description().c_str(), *wxConvCurrent));
		sizer->Add(text, wxALL | wxALIGN_CENTER_VERTICAL);
		adder.set(it);
		boost::apply_visitor(adder,it->value());
	}
	sizer->Layout();

	wnd->SetSizer(sizer);
	wnd->SetAutoLayout(true);
	sizer->Fit(wnd);
	SetSizer(sizer);
    	wnd->Layout();
	Refresh();
}

void parameters_frame::on_browse_input_button(wxCommandEvent& event)
{
	wxString wildcard;
	wildcard << _("Image files ");
	wildcard << wxT("(*.tif;*.tiff;*.png;*.jpg;*.jpeg)|*.tif;*.tiff;*.TIF;*.TIFF;*.png;*.PNG;*.jpg;*.jpeg;*.JPG;*.JPEG|");
	wildcard << wxT("TIFF (*.tif;*.tiff;*.TIF;*.TIFF)|*.tif;*.tiff;*.TIF;*.TIFF|");
	wildcard << wxT("PNG (*.png;*.PNG)|*.png;*.PNG|");
	wildcard << wxT("JPEG (*.jpg;*.jpeg;*.JPG;*.JPEG)|*.jpg;*.jpeg;*.JPG;*.JPEG|");
	wxString str;
	wxFileDialog *fileDialog = new wxFileDialog(this, _("Choose DEM"), wxT(""), wxT(""), wildcard, wxFD_OPEN|wxFD_CHANGE_DIR );
	if (fileDialog->ShowModal() == wxID_OK)
	{
		param::iterator it = param::Instance()->param("input");
		boost::filesystem::path file( fileDialog->GetPath().To8BitData() );
		it->set(file);
		it->control<wxTextCtrl>()->SetValue( wxString(file.string().c_str(), *wxConvCurrent) );
	}
}

void parameters_frame::on_check_save(wxCommandEvent& event)
{
	param::Instance()->set("dosave",event.IsChecked());
}
