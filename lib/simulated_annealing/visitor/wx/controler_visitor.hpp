#ifndef __CONTROLER_HPP__
#define __CONTROLER_HPP__

#include <wx/window.h>
#include <boost/shared_ptr.hpp>
class wxAuiManager;

namespace simulated_annealing {

    class Controler {
    public:
        virtual void go() = 0;
        virtual void stop() = 0;
        virtual void param_visibility(bool) = 0;
        virtual void chart_visibility(bool) = 0;
        virtual bool param_visibility() const = 0;
        virtual bool chart_visibility() const = 0;
    };

    namespace wx {
        class controler_toolbar;

        class controler_visitor
        {
        public:
            controler_visitor(Controler *controler,
                              wxWindow *parent = (wxWindow *) NULL,
                              wxWindowID id = wxID_ANY);

            void init(int, int) {}

            template<typename Configuration, typename Sampler>
            void begin(const Configuration& config, const Sampler& sampler, double t)
            {
                running(true);
            }

            template<typename Configuration, typename Sampler>
            void end(const Configuration& config, const Sampler& sampler, double t)
            {
                running(false);
            }

            template<typename Configuration, typename Sampler>
            inline void visit(const Configuration& config, const Sampler& sampler, double) {}

            void running(bool b);
            void add_pane(wxAuiManager *manager);
        private:
            boost::shared_ptr<controler_toolbar> m_toolbar;
        };
    };

};


// cpp

#include <wx/toolbar.h>
#include <wx/artprov.h>
#include <wx/aui/framemanager.h>

namespace simulated_annealing {

    namespace wx {
        enum
        {
            ID_BUTTON_GO,
            ID_BUTTON_STOP,
            ID_BUTTON_CHART,
            ID_BUTTON_PARAM
        };

        class controler_toolbar : public wxToolBar
        {
        public:
            controler_toolbar(
                    Controler *controler,
                    wxWindow *parent = (wxWindow *) NULL,
                    wxWindowID id = wxID_ANY)
                        : wxToolBar(parent,id)
                        , m_controler(controler)
            {
                m_button_go    = new wxBitmapButton(this, ID_BUTTON_GO   , wxArtProvider::GetBitmap(wxART_TICK_MARK,      wxART_BUTTON, wxSize(22,22)));
                m_button_stop  = new wxBitmapButton(this, ID_BUTTON_STOP , wxArtProvider::GetBitmap(wxART_CROSS_MARK,     wxART_BUTTON, wxSize(22,22)));
                m_button_param = new wxBitmapButton(this, ID_BUTTON_PARAM, wxArtProvider::GetBitmap(wxART_HELP_SETTINGS,  wxART_BUTTON, wxSize(22,22)));
                m_button_chart = new wxBitmapButton(this, ID_BUTTON_CHART, wxArtProvider::GetBitmap(wxART_EXECUTABLE_FILE,wxART_BUTTON, wxSize(22,22)));
                m_button_go   ->Update();
                m_button_stop ->Update();
                m_button_param->Update();
                m_button_chart->Update();
                m_button_go   ->Show();
                m_button_stop ->Show();
                m_button_param->Show();
                m_button_chart->Show();
                m_button_stop->Disable();
                AddControl(m_button_go);
                AddControl(m_button_stop);
                AddControl(m_button_param);
                AddControl(m_button_chart);

                m_info.Caption( _("librjmcmc toolbar") );
                m_info.ToolbarPane();
                m_info.Top();
                m_info.CloseButton(false);
                m_info.CaptionVisible(false);
                Realize();
                Update();
                Show();
            }

            void on_button_go   (wxCommandEvent&);
            void on_button_stop (wxCommandEvent&);
            void on_button_chart(wxCommandEvent&);
            void on_button_param(wxCommandEvent&);

            void running(bool b) {
                if(b) {
                    m_button_go  ->Disable();
                    m_button_stop->Enable();
                } else {
                    m_button_stop->Disable();
                    m_button_go  ->Enable();
                }
            }

            const wxAuiPaneInfo& info() const { return m_info; }

        private:
            wxButton  *m_button_go;
            wxButton  *m_button_stop;
            wxButton  *m_button_chart;
            wxButton  *m_button_param;

            Controler *m_controler;
            wxAuiPaneInfo m_info;
            DECLARE_EVENT_TABLE();
        };

        void controler_toolbar::on_button_go(wxCommandEvent&)
        {
            m_controler->go();
        }

        void controler_toolbar::on_button_stop(wxCommandEvent&)
        {
            m_controler->stop();
        }

        void controler_toolbar::on_button_chart(wxCommandEvent&)
        {
            m_controler->param_visibility(!m_controler->param_visibility());
        }

        void controler_toolbar::on_button_param(wxCommandEvent&)
        {
            m_controler->chart_visibility(!m_controler->chart_visibility());
        }

        BEGIN_EVENT_TABLE(controler_toolbar,wxToolBar)
                EVT_BUTTON(ID_BUTTON_GO   ,controler_toolbar::on_button_go   )
                EVT_BUTTON(ID_BUTTON_STOP ,controler_toolbar::on_button_stop )
                EVT_BUTTON(ID_BUTTON_CHART,controler_toolbar::on_button_chart)
                EVT_BUTTON(ID_BUTTON_PARAM,controler_toolbar::on_button_param)
                END_EVENT_TABLE();


        void controler_visitor::running(bool b)
        {
            m_toolbar->running(b);
        }

        void controler_visitor::add_pane(wxAuiManager *manager)
        {
            manager->AddPane( m_toolbar.get() , m_toolbar->info() );
        }

        controler_visitor::controler_visitor(Controler *controler, wxWindow *parent, wxWindowID id)
            : m_toolbar(new controler_toolbar(controler,parent,id))
        {
        }

    };
};
#endif // __CONTROLER_HPP__
