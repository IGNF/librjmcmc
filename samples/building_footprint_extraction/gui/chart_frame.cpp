#include <wx/sizer.h>
#include "chart_frame.hpp"

#include "wx/chartpanel.h"
#include "wx/xy/vectordataset.h"
#include "wx/xy/xylinerenderer.h"
#include "wx/xy/xyplot.h"

#include <iostream>

chart_frame::chart_frame(wxWindow *parent, wxWindowID id, const wxString& charttitle, long style, const wxPoint& pos, const wxSize& size) :
	wxDialog(parent, id, charttitle, pos, size, style)
{
	wxBoxSizer *test_sizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* inner_sizer = new wxBoxSizer(wxVERTICAL);

	wxChartPanel* wnd = new wxChartPanel(this);
	XYPlot *plot = new XYPlot();

	wxColour color[] = { wxColour(0,0,255), wxColour(255,0,0) };
	char *title[] = { "Energy", "Temperature" };
	AXIS_LOCATION location[] = { AXIS_LEFT, AXIS_RIGHT };

	// add dynamic datasets and axes with line renderers

	// create bottom number axis
	NumberAxis *iterations_axis = new NumberAxis(AXIS_BOTTOM);
	iterations_axis->SetTitle(wxT("Iterations"));
	iterations_axis->IntegerValues();
	plot->AddAxis(iterations_axis);

	// link axes and datasets
	for(unsigned int i=0; i<2; ++i) {
		m_dataset[i] = new VectorDataset;
                std::cout << i << "\n";
		XYLineStepRenderer *renderer = new XYLineStepRenderer();
		renderer->SetSerieColour(0, &color[i]);
		m_dataset[i]->SetRenderer(renderer);
		plot->AddDataset(m_dataset[i]);
		NumberAxis *axis = new NumberAxis(location[i]);
                axis->SetTitle(wxString(title[i], *wxConvCurrent));
		axis->SetLabelTextColour(color[i]);
		axis->SetTitleColour(color[i]);
		plot->AddAxis(axis);
		plot->LinkDataVerticalAxis(i, i);
		plot->LinkDataHorizontalAxis(i, 0);
		if(i==1) axis->SetFixedMin(0);
	}

	// and finally create chart
	Chart *chart = new Chart(plot);

	wnd->SetChart(chart);
	wnd->SetScrollbars(20, 20, 50, 50);

	wnd->SetSizer(inner_sizer);
	wnd->SetAutoLayout(true);
	wnd->Layout();

	test_sizer->Add(wnd, 1, wxEXPAND | wxALL, 5);
	SetSizer(test_sizer);
}

void chart_frame::begin(unsigned int dump, unsigned int save) {
        m_dataset[0]->Clear();
        m_dataset[1]->Clear();
}

void chart_frame::iterate(unsigned int i, double t, const configuration& config, const sampler& sample) {
	m_dataset[0]->Add(config.unary_energy() + config.binary_energy());
	m_dataset[1]->Add(t);
}
