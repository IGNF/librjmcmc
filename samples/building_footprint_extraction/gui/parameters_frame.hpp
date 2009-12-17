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

class parameters_frame: public wxDialog
{
public:
	parameters_frame(wxWindow *parent = (wxWindow *) NULL, wxWindowID id = wxID_ANY, const wxString& title = _("MPP parameters"), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	virtual ~parameters_frame() {}

	std::vector<std::pair<wxStaticText*, wxTextCtrl*> > m_parameters;
	typedef std::vector<std::pair<wxStaticText*, wxTextCtrl*> >::const_iterator parameters_const_iterator;
	
private:
	void on_browse_input_button(wxCommandEvent& event);
	void on_check_save(wxCommandEvent& event);
};

#endif /* PARAMETERSFRAME_H_ */
