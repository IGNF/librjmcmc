/***********************************************************************
This file is part of the librjmcmc project source files.

Copyright : Institut Geographique National (2008-2012)
Contributors : Mathieu Brédif, Olivier Tournaire, Didier Boldo
email : librjmcmc@ign.fr

This software is a generic C++ library for stochastic optimization.

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software. You can use,
modify and/or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty and the software's author, the holder of the
economic rights, and the successive licensors have only limited liability.

In this respect, the user's attention is drawn to the risks associated
with loading, using, modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean that it is complicated to manipulate, and that also
therefore means that it is reserved for developers and experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and, more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

***********************************************************************/

#include "chart_visitor.hpp"

#include <wx/sizer.h>
#include "wx/chartpanel.h"
#include "wx/xy/vectordataset.h"
#include "wx/xy/xylinerenderer.h"
#include "wx/xy/xyplot.h"

#include "gui/resources/IGN.xpm"

namespace simulated_annealing {
    namespace wx {

        class chart_frame : public wxFrame
        {
        public:
            chart_frame(wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
                : wxFrame(parent,id,title,pos,size,style)
            {
            }
            void on_close_window(wxCloseEvent&) { Hide(); }
        private:
            DECLARE_EVENT_TABLE();
        };


        BEGIN_EVENT_TABLE(chart_frame, wxFrame)
                EVT_CLOSE(chart_frame::on_close_window)
                END_EVENT_TABLE();

        chart_visitor::chart_visitor(wxWindow *parent, wxWindowID id, const wxString& charttitle, const wxPoint& pos, const wxSize& size, long style) :
                m_frame(new chart_frame(parent, id, charttitle, pos, size, style))
        {
            m_frame->SetIcon(wxICON(IGN));

            wxBoxSizer *test_sizer = new wxBoxSizer(wxVERTICAL);
            wxBoxSizer* inner_sizer = new wxBoxSizer(wxVERTICAL);

            wxChartPanel* wnd = new wxChartPanel(m_frame.get());
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
            m_frame->SetSizer(test_sizer);
            m_frame->Show();
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

        void chart_visitor::Show(bool b) { m_frame->Show(b); }
        bool chart_visitor::IsShown() const { return m_frame->IsShown(); }

    } // namespace wx
} // namespace simulated_annealing


