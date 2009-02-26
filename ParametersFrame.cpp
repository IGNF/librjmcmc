/*
 * ParametersFrame.cpp
 *
 *  Created on: 4 déc. 2008
 *      Author: olivier
 */

#include "ParametersFrame.h"
#include "BuildingsDetectorParameters.hpp"

#include <wx/sizer.h>

ParametersFrame::ParametersFrame(wxWindow *parent, wxWindowID id, const wxString& title, long style, const wxPoint& pos, const wxSize& size) :
	wxDialog(parent, id, title, pos, size, style)
{
	wxBoxSizer *test_sizer= new wxBoxSizer( wxVERTICAL );
	wxBoxSizer* inner_sizer = new wxBoxSizer(wxVERTICAL);

	wxScrolledWindow* wnd = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxT("scroll"));
	wnd->SetScrollbars(20,20,50,50);

	std::vector< std::pair<std::string, std::string> > pileText2;
	BuildingsDetectorParametersSingleton::Instance()->GetAsText(pileText2);
	for (unsigned int i=0; i<pileText2.size(); ++i)
	{
		wxBoxSizer *sizerH = new wxBoxSizer( wxHORIZONTAL );
		m_pileText.push_back(std::make_pair(new wxStaticText(this, -1, wxString(pileText2[i].first.c_str(),*wxConvCurrent)),new wxTextCtrl(this, -1, wxString(pileText2[i].second.c_str(),*wxConvCurrent))));
		sizerH->Add(m_pileText.back().first, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
		sizerH->Add(m_pileText.back().second, 1, wxALL, 5);
		inner_sizer->Add(sizerH,0,wxEXPAND);
		sizerH->Layout();
	}

	wnd->SetSizer(inner_sizer);
    wnd->SetAutoLayout(true);
    wnd->Layout();

    test_sizer->Add(wnd, 1, wxEXPAND | wxALL, 5);
    SetSizer(test_sizer);
    //SetAutoLayout(true);
    //Layout();

	//SetSizerAndFit(test_sizer);
	//test_sizer->Layout();
}

ParametersFrame::~ParametersFrame()
{
	// TODO Auto-generated destructor stub
}
