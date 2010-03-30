#ifndef CHART_FRAME_H_
#define CHART_FRAME_H_

#include <vector>
#include <string>

#include <wx/frame.h>
#include "wx/xy/vectordataset.h"

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
		wxMutexGuiEnter();
		for(unsigned int i=0; i<2; ++i) {
			m_dataset[i]->SetX0(0.);
			m_dataset[i]->SetScaleX(m_dump);
			m_dataset[i]->Clear();
		}
		m_dataset[0]->Add(config.energy());
		m_dataset[1]->Add(t);
		wxMutexGuiLeave();
	}

	template<typename Config, typename Sampler>
	bool iterate(unsigned int i, double t, const Config& config, const Sampler&) {
		if(i%m_dump) return true;
		wxMutexGuiEnter();
		m_dataset[0]->Add(config.energy());
		m_dataset[1]->Add(t);
		wxMutexGuiLeave();
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

	DECLARE_EVENT_TABLE();
};

#endif /* CHART_FRAME_H_ */
