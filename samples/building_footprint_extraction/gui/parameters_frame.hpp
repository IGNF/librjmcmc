/*
 * ParametersFrame.h
 *
 *  Created on: 4 déc. 2008
 *      Author: olivier
 */

#ifndef PARAMETERSFRAME_H_
#define PARAMETERSFRAME_H_

#include <map>
#include <string>
#include <boost/variant.hpp>

// TODO : nettoyage des headers
#include <wx/wx.h>

struct sizer_adder;

class parameters_frame: public wxFrame
{
public:
	parameters_frame(wxWindow *parent = (wxWindow *) NULL, wxWindowID id = wxID_ANY, const wxString& title = _("MPP parameters"), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	virtual ~parameters_frame() {}

//	typedef boost::variant<wxTextCtrl*,wxCheckBox*> control_type;

//	inline control_type control(const std::string& s) { return *m_control.find(s); }
	void Refresh();
	
	friend struct sizer_adder;
private:
	void on_browse_input_button(wxCommandEvent& event);
	void on_check_save(wxCommandEvent& event);

//	std::map<std::string,control_type> m_control;
	wxSizer  *m_sizer;
};

#endif /* PARAMETERSFRAME_H_ */
