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
                m_frame(new chart_frame(parent, id, charttitle, pos, size, style)), m_proposed(NULL), m_accepted(NULL)
        {
            m_frame->SetIcon(wxICON(IGN));

            wxBoxSizer *test_sizer = new wxBoxSizer(wxVERTICAL);
            wxBoxSizer* inner_sizer = new wxBoxSizer(wxVERTICAL);

            m_panel = new wxChartPanel(m_frame.get());
            m_panel->SetScrollbars(20, 20, 50, 50);

            m_panel->SetSizer(inner_sizer);
            m_panel->SetAutoLayout(true);
            m_panel->Layout();

            test_sizer->Add(m_panel, 1, wxEXPAND | wxALL, 5);
            m_frame->SetSizer(test_sizer);
            m_frame->Show();
        }


        void chart_visitor::init(int dump, int)
        {
            m_dump = 20*dump;
            m_iter = 0;
        }
        void chart_visitor::clear(const std::vector<std::string>& kernel_name )
        {
#ifdef OVERALL_ACCEPTANCE
                unsigned int kernel_size = 1;
#else
                unsigned int kernel_size = kernel_name.size();
#endif
            if(m_accepted) delete m_accepted;
            if(m_proposed) delete m_proposed;
            m_accepted = new unsigned int[kernel_size];
            m_proposed = new unsigned int[kernel_size];
            for (unsigned int i=0; i<kernel_size; ++i) m_accepted[i] = m_proposed[i] = 0;


            // create bottom number axis
            NumberAxis *iterations_axis = new NumberAxis(AXIS_BOTTOM);
            iterations_axis->SetTitle(wxT("Iterations"));
            iterations_axis->IntegerValues();

            m_dataset.resize(kernel_size+2);

            XYPlot *plot = new XYPlot();
            plot->AddAxis(iterations_axis);

            wxColour color[2] = { wxColour(0,0,255), wxColour(255,0,0) };
            std::string title[2] = { "Energy", "Temperature" };

            // create axes
            for(unsigned int i=0; i<2; ++i) {
                wxColour& col =color[i];
                wxString s(title[i].c_str(), *wxConvCurrent);
                NumberAxis *axis = new NumberAxis(AXIS_LEFT);
                //axis->SetTitle(s);
                axis->SetLabelTextColour(col);
                axis->SetTitleColour(col);
                if(i) axis->SetFixedMin(0);
                plot->AddAxis(axis);
                m_dataset[i] = new VectorDataset(s);
                m_dataset[i]->SetX0(0.);
                m_dataset[i]->SetScaleX(m_dump);
                m_dataset[i]->Clear();
                XYLineStepRenderer *renderer = new XYLineStepRenderer();
                renderer->SetSerieColour(0, color+i);
                m_dataset[i]->SetRenderer(renderer);
                plot->AddDataset(m_dataset[i]);
                plot->LinkDataVerticalAxis(i, i);
                plot->LinkDataHorizontalAxis(i, 0);
            }

            NumberAxis *acceptance = new NumberAxis(AXIS_RIGHT);
            acceptance->SetTitle(wxString("Acceptance", *wxConvCurrent));
            acceptance->SetFixedMin(0);
            acceptance->SetFixedMax(100);
            plot->AddAxis(acceptance);

            // create and link datasets
            for(unsigned int i=0; i<kernel_size; ++i) {
                int r = rand()%255;
                int g = rand()%255;
                int b = rand()%255;
                wxColour col(r,g,b);
#ifdef OVERALL_ACCEPTANCE
                wxString s("Overall", *wxConvCurrent);
#else
                wxString s(kernel_name[i].c_str(), *wxConvCurrent);
#endif
                m_dataset[i+2] = new VectorDataset(s);
                m_dataset[i+2]->SetX0(0.);
                m_dataset[i+2]->SetScaleX(m_dump);
                m_dataset[i+2]->Clear();
                XYLineStepRenderer *renderer = new XYLineStepRenderer();
                renderer->SetSerieColour(0, &col);
                m_dataset[i+2]->SetRenderer(renderer);
                plot->AddDataset(m_dataset[i+2]);
                plot->LinkDataVerticalAxis(i+2, 2);
                plot->LinkDataHorizontalAxis(i+2, 0);
            }

            plot->SetLegend(new Legend(wxCENTER,wxRIGHT));
            // and finally create chart
            Chart *chart = new Chart(plot);
            m_panel->SetAntialias(true);
            wxMutexGuiEnter();
            m_panel->SetChart(chart);
            wxMutexGuiLeave();
        }

        void chart_visitor::add(int i, double val)
        {
            if(m_dataset[i]) m_dataset[i]->Add(val);
        }

        void chart_visitor::Show(bool b) { m_frame->Show(b); }
        bool chart_visitor::IsShown() const { return m_frame->IsShown(); }
        void chart_visitor::begin_update() {
            wxMutexGuiEnter();
            m_panel->GetChart()->GetPlot()->BeginUpdate();
        }
        void chart_visitor::end_update() {
            m_panel->GetChart()->GetPlot()->EndUpdate();
            wxMutexGuiLeave();
        }
    } // namespace wx
} // namespace simulated_annealing


