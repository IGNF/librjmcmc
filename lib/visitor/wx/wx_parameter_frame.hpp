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

	void init(int dump, int save) {}

	template<typename Configuration, typename Sampler>
        void begin(const Configuration& config, const Sampler& sampler, double t)
	{
		wxMutexGuiEnter();
		Refresh();
		Disable();
		wxMutexGuiLeave();
	}
	template<typename Configuration, typename Sampler>
        void end(const Configuration& config, const Sampler& sampler, double t)
	{
		wxMutexGuiEnter();
		Enable();
		Refresh();
		Show();
		wxMutexGuiLeave();
	}

	template<typename Configuration, typename Sampler>
	bool iterate(const Configuration& config, const Sampler& sampler, double, unsigned int i) {
		return true;
	}

	void on_file_parameter(wxCommandEvent& event);
	void on_bool_parameter(wxCommandEvent& event);

	void OnCloseWindow(wxCloseEvent&) { Hide(); }

private:
	std::map<long,std::string> m_name_for_id;

	DECLARE_EVENT_TABLE();
};

#endif /* PARAMETERSFRAME_H_ */
