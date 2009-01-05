/*
 * ParametersFrame.h
 *
 *  Created on: 4 déc. 2008
 *      Author: olivier
 */

#ifndef PARAMETERSFRAME_H_
#define PARAMETERSFRAME_H_

#include <vector>
#include <string>

// TODO : nettoyage des headers
#include <wx/wx.h>
#include <wx/dialog.h>

class ParametersFrame: public wxDialog
{
public:
	ParametersFrame(wxWindow *parent = (wxWindow *) NULL, wxWindowID id = wxID_ANY, const wxString& title = _("MPP parameters"), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	virtual ~ParametersFrame();

	std::vector< std::pair<wxStaticText*, wxTextCtrl*> > m_pileText;
};

#endif /* PARAMETERSFRAME_H_ */
