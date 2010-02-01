#include <wx/sizer.h>
#include "chart_frame.hpp"

#include "wx/chartpanel.h"
#include "wx/xy/vectordataset.h"
#include "wx/xy/xylinerenderer.h"
#include "wx/xy/xyplot.h"

chart_frame::chart_frame(wxWindow *parent, wxWindowID id, const wxString& title, long style, const wxPoint& pos, const wxSize& size) :
	wxDialog(parent, id, title, pos, size, style)
{
	wxBoxSizer *test_sizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* inner_sizer = new wxBoxSizer(wxVERTICAL);

	wxChartPanel* wnd = new wxChartPanel(this);
	XYPlot *plot = new XYPlot();

	wxColour color[] = { wxColour(0,0,255), wxColour(255,0,0) };


	// add dynamic datasets and axes with line renderers

	m_energy = new VectorDataset;
	XYLineStepRenderer *energy_renderer = new XYLineStepRenderer();
	energy_renderer->SetSerieColour(0, &color[0]);
	m_energy->SetRenderer(energy_renderer);
	plot->AddDataset(m_energy);
	NumberAxis *energy_axis = new NumberAxis(AXIS_LEFT);
	energy_axis->SetTitle(wxT("Energy"));
	energy_axis->SetLabelTextColour(color[0]);
	plot->AddAxis(energy_axis);
	plot->LinkDataVerticalAxis(0, 0);

	m_temperature = new VectorDataset;
	XYLineStepRenderer *temperature_renderer = new XYLineStepRenderer();
	temperature_renderer->SetSerieColour(0, &color[1]);
	m_temperature->SetRenderer(temperature_renderer);
	plot->AddDataset(m_temperature);
	NumberAxis *temperature_axis = new NumberAxis(AXIS_RIGHT);
	temperature_axis->SetTitle(wxT("Temperature"));
	temperature_axis->SetLabelTextColour(color[1]);
	plot->AddAxis(temperature_axis);
	plot->LinkDataVerticalAxis(1, 1);
	//temperature_axis->SetFixedBounds(0,100);

	// create bottom number axis
	NumberAxis *iterations_axis = new NumberAxis(AXIS_BOTTOM);
	iterations_axis->SetTitle(wxT("Iterations"));
	iterations_axis->IntegerValues();
	// add axes to plot
	plot->AddAxis(iterations_axis);
	// link axes and datasets
	for(unsigned int i=0; i<plot->GetDatasetCount(); ++i) {
		plot->LinkDataHorizontalAxis(i, 0);
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
	m_temperature->Clear();
	m_energy->Clear();
}

void chart_frame::iterate(unsigned int i, double t, const configuration& config, const sampler& sample) {
	m_temperature->Add(t);
	m_energy->Add(config.unary_energy() + config.binary_energy());
}
