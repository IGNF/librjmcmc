#ifndef CHART_FRAME_H_
#define CHART_FRAME_H_

#include <vector>
#include <string>

#include <wx/frame.h>
#include "core/building_footprint_rectangle.hpp"

class VectorDataset;

class chart_frame: public wxFrame
{
public:
	chart_frame(wxWindow *parent = (wxWindow *) NULL, wxWindowID id = wxID_ANY, const wxString& title = _("MPP chart"), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	virtual ~chart_frame() {}
	void begin(unsigned int dump, unsigned int save, double t, const configuration& config);
	void iterate(unsigned int i, double t, const configuration& config, const sampler& sample);
private:
	VectorDataset *m_dataset[2];
};

#endif /* CHART_FRAME_H_ */
