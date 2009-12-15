/*
 * ParametersFrame.cpp
 *
 *  Created on: 4 déc. 2008
 *      Author: olivier
 */

#include <wx/sizer.h>
#include "core/building_footprint_extraction_parameters.hpp"
#include "parameters_frame.hpp"

parameters_frame::parameters_frame(wxWindow *parent, wxWindowID id, const wxString& title, long style, const wxPoint& pos, const wxSize& size) :
	wxDialog(parent, id, title, pos, size, style)
{
	wxBoxSizer *test_sizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* inner_sizer = new wxBoxSizer(wxVERTICAL);

	wxScrolledWindow* wnd = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxT("scroll"));
	wnd->SetScrollbars(20, 20, 50, 50);

	std::map<std::string, std::string> options;
	building_footprint_extraction_parameters::Instance()->store_string_map(options);
	std::map<std::string, std::string>::const_iterator it;
	for (it = options.begin(); it != options.end(); ++it)
	{
		wxBoxSizer *sizerH = new wxBoxSizer(wxHORIZONTAL);
		m_parameters.push_back(std::make_pair(new wxStaticText(wnd, -1, wxString(it->first.c_str(), *wxConvCurrent)), new wxTextCtrl(wnd, -1, wxString(it->second.c_str(), *wxConvCurrent))));
		sizerH->Add(m_parameters.back().first, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
		sizerH->Add(m_parameters.back().second, 1, wxALL, 5);
		inner_sizer->Add(sizerH, 0, wxEXPAND);
		sizerH->Layout();
	}

	wnd->SetSizer(inner_sizer);
	wnd->SetAutoLayout(true);
	wnd->Layout();

	test_sizer->Add(wnd, 1, wxEXPAND | wxALL, 5);
	SetSizer(test_sizer);

	//	SetSizer(inner_sizer);
	//    SetAutoLayout(true);
	//    Layout();

	//SetSizerAndFit(test_sizer);
	//test_sizer->Layout();
}

parameters_frame::~parameters_frame()
{
	// TODO Auto-generated destructor stub
}
