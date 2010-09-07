#include <boost/filesystem.hpp>

#include <fstream>
#include <sstream>

#include <wx/artprov.h>

#include <GilViewer/layers/VectorLayerGhost.h>
#include <GilViewer/layers/ImageLayer.hpp>
#include <GilViewer/layers/simple_vector_layer.hpp>
#include <GilViewer/gui/LayerControl.hpp>
#include <GilViewer/gui/ApplicationSettings.hpp>
#include <GilViewer/gui/PanelManager.h>
#include <GilViewer/gui/define_id.hpp>
#include <GilViewer/io/gilviewer_file_io.hpp>
#include <GilViewer/io/gilviewer_io_factory.hpp>
#include <GilViewer/convenient/macros_gilviewer.hpp>

#include "param/wx_parameter.hpp"
#include "wx_parameter_frame.hpp"
typedef parameters< wx_parameter > param;

class rjmcmc_panel_viewer : public panel_viewer {
 public:
    virtual ~rjmcmc_panel_viewer() {}
    virtual void execute_mode_capture() { update_bbox(); }
    virtual void execute_mode_geometry_moving() { update_bbox(); }
	static void Register(wxFrame* parent);

protected:
    rjmcmc_panel_viewer(wxFrame *parent) : panel_viewer(parent) {}
    friend rjmcmc_panel_viewer* create_rjmcmc_panel_viewer(wxFrame* parent);

private:
	void update_bbox() {
        boost::shared_ptr<vector_layer_ghost> ghost = vectorlayerghost();
		if (!ghost->m_drawRectangleSelection) return;
		param *p = param::instance();
		wxPoint p0 = ghost->m_rectangleSelection.first;
		wxPoint p1 = ghost->m_rectangleSelection.second;
		if(p0.x>p1.x) std::swap(p0.x,p1.x);
		if(p0.y>p1.y) std::swap(p0.y,p1.y);
		p->set("xmin",p0.x);
		p->set("ymin",p0.y);
		p->set("xmax",p1.x);
		p->set("ymax",p1.y);
	}
};

rjmcmc_panel_viewer* create_rjmcmc_panel_viewer(wxFrame* parent)
{
        return new rjmcmc_panel_viewer(parent);
}

void rjmcmc_panel_viewer::Register(wxFrame* parent)
{
	panel_manager::instance()->Register("rjmcmc_panel_viewer", boost::bind(create_rjmcmc_panel_viewer, parent));
}


#include "configuration_frame.hpp"

enum
{
    ID_BUTTON_GO,
    ID_BUTTON_STOP,
    ID_BUTTON_CHART,
    ID_BUTTON_PARAM
};

using namespace std;

BEGIN_EVENT_TABLE(configuration_frame,basic_viewer_frame)
    EVT_BUTTON(ID_BUTTON_GO,configuration_frame::OnGoButton)
    EVT_BUTTON(ID_BUTTON_STOP,configuration_frame::OnStopButton)
    EVT_BUTTON(ID_BUTTON_CHART,configuration_frame::OnChartButton)
    EVT_BUTTON(ID_BUTTON_PARAM,configuration_frame::OnParamButton)
    ADD_GILVIEWER_EVENTS_TO_TABLE(configuration_frame)
END_EVENT_TABLE();

IMPLEMENTS_GILVIEWER_METHODS_FOR_EVENTS_TABLE(configuration_frame,m_panel)

        configuration_frame::configuration_frame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos , const wxSize& size , long style , const wxString& name ):
    basic_viewer_frame(parent, id, title, pos, size, style, name)
{
#if defined(__WXMSW__)
    wxIcon icon("logo_matis_small");
    SetIcon (icon);
#else
    //SetIcon(wxICON(LOGO_MATIS_small));
#endif

    panel_viewer::Register(this);
	rjmcmc_panel_viewer::Register(this);
    m_panel = panel_manager::instance()->create_object("rjmcmc_panel_viewer");

    m_statusBar->SetStatusText(_("librjmcmc"));

    wxAuiPaneInfo toolbarInfo;
    toolbarInfo.Caption( _("Main toolbar") );
    toolbarInfo.ToolbarPane();
    toolbarInfo.Top();
    toolbarInfo.CloseButton(false);
    toolbarInfo.CaptionVisible(false);
    m_dockManager.AddPane( m_panel->main_toolbar(this), toolbarInfo );

    wxAuiPaneInfo paneInfoDrawPane;
    paneInfoDrawPane.Caption( _("viewer panel") );
    paneInfoDrawPane.Center();
    paneInfoDrawPane.CloseButton(false);
    paneInfoDrawPane.CaptionVisible(false);
    m_dockManager.AddPane( m_panel, paneInfoDrawPane );

    wxAuiPaneInfo modeAndGeometryToolbarInfo;
    modeAndGeometryToolbarInfo.Caption( _("Mode and geometry Toolbar") );
    modeAndGeometryToolbarInfo.ToolbarPane();
    modeAndGeometryToolbarInfo.Top();
    modeAndGeometryToolbarInfo.CloseButton(false);
    modeAndGeometryToolbarInfo.CaptionVisible(false);
    m_dockManager.AddPane( m_panel->mode_and_geometry_toolbar(this), modeAndGeometryToolbarInfo );

    wxAuiPaneInfo applicationToolBarInfo;
    applicationToolBarInfo.Caption( _("librjmcmc toolbar") );
    applicationToolBarInfo.ToolbarPane();
    applicationToolBarInfo.Top();
    applicationToolBarInfo.CloseButton(false);
    applicationToolBarInfo.CaptionVisible(false);
    wxToolBar *librjmcmctoolbar = new wxToolBar(this,wxID_ANY);
    m_buttonGo = new wxBitmapButton(librjmcmctoolbar, ID_BUTTON_GO, wxArtProvider::GetBitmap(wxART_TICK_MARK, wxART_BUTTON, wxSize(22,22)) );
    m_buttonStop = new wxBitmapButton(librjmcmctoolbar, ID_BUTTON_STOP, wxArtProvider::GetBitmap(wxART_CROSS_MARK, wxART_BUTTON, wxSize(22,22)) );
    wxButton *button_param = new wxBitmapButton(librjmcmctoolbar, ID_BUTTON_PARAM, wxArtProvider::GetBitmap(wxART_HELP_SETTINGS, wxART_BUTTON, wxSize(22,22)) );
    wxButton *button_chart = new wxBitmapButton(librjmcmctoolbar, ID_BUTTON_CHART, wxArtProvider::GetBitmap(wxART_EXECUTABLE_FILE, wxART_BUTTON, wxSize(22,22)) );
    m_buttonStop->Disable();
    librjmcmctoolbar->AddControl(m_buttonGo);
    librjmcmctoolbar->AddControl(m_buttonStop);
    librjmcmctoolbar->AddControl(button_param);
    librjmcmctoolbar->AddControl(button_chart);

    m_dockManager.AddPane( librjmcmctoolbar , applicationToolBarInfo );
    m_dockManager.Update();
	Show();
}

void configuration_frame::OnGoButton(wxCommandEvent&)
{
    m_controler->go(); 
}

void configuration_frame::OnStopButton(wxCommandEvent&)
{
    m_controler->stop();
}

void configuration_frame::OnParamButton(wxCommandEvent&)
{
    m_controler->param_visibility(!m_controler->param_visibility());
}

void configuration_frame::OnChartButton(wxCommandEvent&)
{
    m_controler->chart_visibility(!m_controler->chart_visibility());
}

wxAboutDialogInfo configuration_frame::about_info() const
{
    wxAboutDialogInfo info;
    info.AddDeveloper(_("Olivier Tournaire"));
    info.AddDeveloper(_("Mathieu Brédif"));
    info.AddDeveloper(_("Didier Boldo"));
    info.SetName(_("librjmcmc buildings detection sample"));
    info.SetVersion(_("0.1.0"));
    info.SetWebSite(_("http://librjmcmc.ign.fr"), _("Home page") );
    info.SetDescription(_("Marked point process for buildings detection from DEM"));
    info.SetCopyright(_T("olivier.tournaire@ign.fr\nmathieu.bredif@ign.fr"));
    return info;
}


void configuration_frame::init(int dump, int save)
{
		m_dump = dump;
		m_save = save;

        m_vlayer = layer::ptrLayerType(new simple_vector_layer("Extracted elements"));
        m_panel->add_layer(m_vlayer);

                boost::shared_ptr<vector_layer_ghost> ghost = m_panel->vectorlayerghost();

        m_vlayer->translation_x(/*p0.x+*/ghost->translation_x());
        m_vlayer->translation_y(/*p0.y+*/ghost->translation_y());
        m_vlayer->zoom_factor  (     ghost->zoom_factor  ());
	
        m_vlayer->line_color(*wxRED);
        m_vlayer->line_style(wxSOLID);
        m_vlayer->line_width(3);
        m_vlayer->polygon_border_color(*wxBLUE);
        m_vlayer->polygon_border_style(wxSOLID);
        m_vlayer->polygon_border_width(3);
        m_vlayer->polygon_inner_color(*wxRED);
        m_vlayer->polygon_inner_style(wxTRANSPARENT);
        m_vlayer->text_visibility(false);      
}


	wxRect configuration_frame::get_bbox() const {
            boost::shared_ptr<vector_layer_ghost> ghost = m_panel->vectorlayerghost();
    	if(!ghost->m_drawRectangleSelection) return wxRect();
		return wxRect(ghost->m_rectangleSelection.first,ghost->m_rectangleSelection.second);
	}

	void configuration_frame::set_bbox(const wxRect& r) {
            boost::shared_ptr<vector_layer_ghost> ghost = m_panel->vectorlayerghost();
		ghost->m_rectangleSelection.first  = r.GetTopLeft    ();
		ghost->m_rectangleSelection.second = r.GetBottomRight();	
    	ghost->m_drawRectangleSelection = true;
		ghost->m_penRectangle = wxPen(*wxRED, 2, wxDOT);
		ghost->m_brushRectangle = wxBrush(*wxRED, wxTRANSPARENT);
	}


        panel_viewer* configuration_frame::panelviewer() const { return m_panel; }

	void configuration_frame::add_layer(const std::string& file) {
            boost::shared_ptr<vector_layer_ghost> ghost = m_panel->vectorlayerghost();

            std::string extension(boost::filesystem::extension(file));
            layer::ptrLayerType ilayer;
            try
            {
                boost::shared_ptr<gilviewer_file_io> file_io = gilviewer_io_factory::instance()->create_object(extension.substr(1,extension.size()-1));
                ilayer = file_io->load(file);
                m_panel->add_layer( ilayer );
            }
            catch (const std::exception &e)
            {
                GILVIEWER_LOG_EXCEPTION("Read error: " + file);
                wxString message( oss.str().c_str() , *wxConvCurrent );
                ::wxMessageBox( message , _("Error !") , wxICON_ERROR );
                return;
            }

        ilayer->translation_x(/*p0.x+*/ghost->translation_x());
        ilayer->translation_y(/*p0.y+*/ghost->translation_y());
        ilayer->zoom_factor  (     ghost->zoom_factor  ());
	}

