#ifndef CHART_FRAME_H_
#define CHART_FRAME_H_

#include <vector>
#include <string>

// TODO : nettoyage des headers
#include <wx/wx.h>
#include <wx/dialog.h>
#include "core/building_footprint_extraction.hpp"

class VectorDataset;

class chart_frame: public wxDialog
{
public:
	chart_frame(wxWindow *parent = (wxWindow *) NULL, wxWindowID id = wxID_ANY, const wxString& title = _("MPP chart"), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	virtual ~chart_frame() {}
	void begin(unsigned int dump, unsigned int save);
	void iterate(unsigned int i, double t, const configuration& config, const sampler& sample);
private:
	VectorDataset *m_temperature, *m_energy;
};

#endif /* CHART_FRAME_H_ */
