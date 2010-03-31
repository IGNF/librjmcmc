#ifndef CHART_FRAME_HPP
#define CHART_FRAME_HPP

#include <wx/frame.h>

class VectorDataset;
class chart_frame: public wxFrame
{
public:
	chart_frame(
		wxWindow *parent = (wxWindow *) NULL,
		wxWindowID id = wxID_ANY,
		const wxString& title = _("librjmcmc charts"),
		long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize);

	template<typename Config>
	void begin(unsigned int dump, unsigned int save, double t, const Config& config) {
		m_dump = dump;
		clear();
		add(config.energy(),t);
	}

	template<typename Config, typename Sampler>
	bool iterate(unsigned int i, double t, const Config& config, const Sampler&) {
		if(i%m_dump == 0) add(config.energy(),t);
		return true;
	}

	template<typename Config>
	void end(const Config&)
	{
	}

	void OnCloseWindow(wxCloseEvent&) { Hide(); }

private:
	VectorDataset *m_dataset[2];
	unsigned int m_dump;

	void clear();
	void add(double e, double t);

	DECLARE_EVENT_TABLE();
};

#endif /* CHART_FRAME_HPP */
