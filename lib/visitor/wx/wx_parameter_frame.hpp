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

#include <wx/frame.h>

class parameters_frame: public wxFrame
{
public:
	parameters_frame(
		wxWindow *parent = (wxWindow *) NULL,
		wxWindowID id = wxID_ANY,
		const wxString& title = _("librjmcmc parameters"),
		long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize);

	virtual ~parameters_frame() {}

	template<typename Config>
	void begin(unsigned int dump, unsigned int save, double t, const Config& config) {
		wxMutexGuiEnter();
		Refresh();
		Disable();
		wxMutexGuiLeave();
	}

	template<typename Config, typename Sampler>
	bool iterate(unsigned int i, double t, const Config& config, const Sampler&)
	{
		return true;
	}

	template<typename Config>
	void end(const Config&)
	{
		wxMutexGuiEnter();
		Enable();
		Refresh();
		Show();
		wxMutexGuiLeave();
	}

	void on_file_parameter(wxCommandEvent& event);
	void on_bool_parameter(wxCommandEvent& event);

	void OnCloseWindow(wxCloseEvent&) { Hide(); }

private:
	std::map<long,std::string> m_name_for_id;

	DECLARE_EVENT_TABLE();
};

#endif /* PARAMETERSFRAME_H_ */
