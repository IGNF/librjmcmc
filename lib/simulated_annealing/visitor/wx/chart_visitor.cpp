#include "chart_visitor.hpp"

#include <wx/sizer.h>
#include "wx/chartpanel.h"
#include "wx/xy/vectordataset.h"
#include "wx/xy/xylinerenderer.h"
#include "wx/xy/xyplot.h"

#include "resources/IGN.xpm"

namespace simulated_annealing {
    namespace wx {

        BEGIN_EVENT_TABLE(chart_visitor, wxFrame)
                EVT_CLOSE(chart_visitor::OnCloseWindow)
                END_EVENT_TABLE();

        chart_visitor::chart_visitor(wxWindow *parent, wxWindowID id, const wxString& charttitle, long style, const wxPoint& pos, const wxSize& size) :
                wxFrame(parent, id, charttitle, pos, size, style)
        {
            SetIcon(wxICON(IGN));

            wxBoxSizer *test_sizer = new wxBoxSizer(wxVERTICAL);
            wxBoxSizer* inner_sizer = new wxBoxSizer(wxVERTICAL);

            wxChartPanel* wnd = new wxChartPanel(this);
            XYPlot *plot = new XYPlot();

            wxColour color[] = { wxColour(0,0,255), wxColour(255,0,0) };
            char * title[] = { (char *)"Energy", (char *)"Temperature" };
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
            Show();
        }


        void chart_visitor::clear()
        {
            wxMutexGuiEnter();
            for(unsigned int i=0; i<2; ++i) {
		m_dataset[i]->SetX0(0.);
		m_dataset[i]->SetScaleX(m_dump);
		m_dataset[i]->Clear();
            }
            wxMutexGuiLeave();
        }

        void chart_visitor::add(double e, double t)
        {
            wxMutexGuiEnter();
            m_dataset[0]->Add(e);
            m_dataset[1]->Add(t);
            wxMutexGuiLeave();
        }


    } // namespace wx
} // namespace simulated_annealing


