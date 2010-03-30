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

#include "param/wx_parameter_traits.hpp"
#include "core/building_footprint_extraction_parameters_inc.hpp"

#include "rjmcmc_building_footprint_extraction_frame.hpp"

enum
{
    ID_BUTTON_GO,
    ID_BUTTON_STOP,
    ID_BUTTON_CHART,
    ID_BUTTON_PARAM
};

using namespace std;

BEGIN_EVENT_TABLE(rjmcmc_building_footprint_extraction_frame,BasicViewerFrame)
    EVT_BUTTON(ID_BUTTON_GO,rjmcmc_building_footprint_extraction_frame::OnGoButton)
    EVT_BUTTON(ID_BUTTON_STOP,rjmcmc_building_footprint_extraction_frame::OnStopButton)
    EVT_BUTTON(ID_BUTTON_CHART,rjmcmc_building_footprint_extraction_frame::OnChartButton)
    EVT_BUTTON(ID_BUTTON_PARAM,rjmcmc_building_footprint_extraction_frame::OnParamButton)
    ADD_GILVIEWER_EVENTS_TO_TABLE(rjmcmc_building_footprint_extraction_frame)
END_EVENT_TABLE();

IMPLEMENTS_GILVIEWER_METHODS_FOR_EVENTS_TABLE(rjmcmc_building_footprint_extraction_frame,m_panel)

        rjmcmc_building_footprint_extraction_frame::rjmcmc_building_footprint_extraction_frame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos , const wxSize& size , long style , const wxString& name ):
    BasicViewerFrame(parent, id, title, pos, size, style, name)
{

#if defined(__WXMSW__)
    wxIcon icon("logo_matis_small");
    SetIcon (icon);
#else
    //SetIcon(wxICON(LOGO_MATIS_small));
#endif

    PanelViewer::Register(this);
    m_panel = PanelManager::Instance()->createObject("PanelViewer");

    m_statusBar->SetStatusText(_("librjmcmc"));

    wxAuiPaneInfo toolbarInfo;
    toolbarInfo.Caption( _("Main toolbar") );
    toolbarInfo.ToolbarPane();
    toolbarInfo.Top();
    toolbarInfo.CloseButton(false);
    toolbarInfo.CaptionVisible(false);
    m_dockManager.AddPane( m_panel->GetMainToolBar(this), toolbarInfo );

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
    m_dockManager.AddPane( m_panel->GetModeAndGeometryToolBar(this), modeAndGeometryToolbarInfo );

    wxAuiPaneInfo applicationToolBarInfo;
    applicationToolBarInfo.Caption( _("librjmcmc toolbar") );
    applicationToolBarInfo.ToolbarPane();
    applicationToolBarInfo.Top();
    applicationToolBarInfo.CloseButton(false);
    applicationToolBarInfo.CaptionVisible(false);
    wxToolBar *librjmcmctoolbar = new wxToolBar(this,wxID_ANY);
    m_buttonGo = new wxBitmapButton(librjmcmctoolbar, ID_BUTTON_GO, wxArtProvider::GetBitmap(wxART_TICK_MARK, wxART_BUTTON, wxSize(22,22)) );
    m_buttonStop = new wxBitmapButton(librjmcmctoolbar, ID_BUTTON_STOP, wxArtProvider::GetBitmap(wxART_CROSS_MARK, wxART_BUTTON, wxSize(22,22)) );
    wxButton *button_param = new wxBitmapButton(librjmcmctoolbar, ID_BUTTON_PARAM, wxArtProvider::GetBitmap(wxART_REPORT_VIEW, wxART_BUTTON, wxSize(22,22)) );
    wxButton *button_chart = new wxBitmapButton(librjmcmctoolbar, ID_BUTTON_CHART, wxArtProvider::GetBitmap(wxART_HELP_BOOK, wxART_BUTTON, wxSize(22,22)) );
    m_buttonStop->Disable();
    librjmcmctoolbar->AddControl(m_buttonGo);
    librjmcmctoolbar->AddControl(m_buttonStop);
    librjmcmctoolbar->AddControl(button_param);
    librjmcmctoolbar->AddControl(button_chart);

    m_dockManager.AddPane( librjmcmctoolbar , applicationToolBarInfo );
    m_dockManager.Update();
}

rjmcmc_building_footprint_extraction_frame::~rjmcmc_building_footprint_extraction_frame()
{
}

void rjmcmc_building_footprint_extraction_frame::OnStopButton(wxCommandEvent&)
{
    m_controler->stop();
}

void rjmcmc_building_footprint_extraction_frame::OnParamButton(wxCommandEvent&)
{
    m_controler->toggle_param_visibility();
}

void rjmcmc_building_footprint_extraction_frame::OnChartButton(wxCommandEvent&)
{
    m_controler->toggle_chart_visibility();
}

void rjmcmc_building_footprint_extraction_frame::OnGoButton(wxCommandEvent&)
{
    param::Instance()->update_values();

    LayerControl::const_iterator it  = m_panel->GetLayerControl()->begin();
    LayerControl::const_iterator end = m_panel->GetLayerControl()->end();
    Layer::ptrLayerType ilayer;
    for(;it!=end && !ilayer;++it) {
         if(boost::dynamic_pointer_cast<ImageLayer>(*it)) ilayer=*it; //use dem tag??
    }
    if(!ilayer) {
        boost::filesystem::path file;
        param::Instance()->get("dem",file);
        ilayer = ImageLayer::CreateImageLayer(file.string());
        if ( !ilayer )
        {
          std::ostringstream oss;
          oss << "File " << file << " does not exist !";
          wxString message( oss.str().c_str() , *wxConvCurrent );
          ::wxMessageBox( message , _("Error !") , wxICON_ERROR );
          return;
        }
    }

    wxPoint p0,p1;
    boost::shared_ptr<VectorLayerGhost> ghost = m_panel->GetVectorLayerGhost();
    if(ghost->m_drawRectangleSelection) {
        wxRect rect = ghost->GetRectangle();
        p0 = ilayer->ToLocal(rect.GetTopLeft    ());
        p1 = ilayer->ToLocal(rect.GetBottomRight());
    } else {
        p0.x = param::Instance()->get<int>("xmin");
        p0.y = param::Instance()->get<int>("ymin");
        p1.x = param::Instance()->get<int>("xmax");
        p1.y = param::Instance()->get<int>("ymax");
        if(p0.x>p1.x) std::swap(p0.x,p1.x);
        if(p0.y>p1.y) std::swap(p0.y,p1.y);
    }
    try
    {
       boost::shared_ptr<Layer> vlayer = boost::shared_ptr<Layer>(new simple_vector_layer("Buildings"));
        Layer::ptrLayerType clayer = ilayer->crop(p0.x,p0.y,p1.x,p1.y);
        if(!clayer) {
            std::ostringstream oss;
            oss << "Cropping outside the bounds of " << ilayer->Filename() << " !";
            wxString message( oss.str().c_str() , *wxConvCurrent );
            ::wxMessageBox( message , _("Error !") , wxICON_ERROR );
            return;
        }
        m_panel->AddLayer(clayer);
        m_panel->AddLayer(vlayer);
        vlayer->TranslationX(p0.x+ilayer->TranslationX());
        vlayer->TranslationY(p0.y+ilayer->TranslationY());
        vlayer->ZoomFactor  (     ilayer->ZoomFactor  ());
        clayer->TranslationX(p0.x+ilayer->TranslationX());
        clayer->TranslationY(p0.y+ilayer->TranslationY());
        clayer->ZoomFactor  (     ilayer->ZoomFactor  ());
	
	vlayer->set_line_color(*wxRED);
        vlayer->set_line_style(wxSOLID);
        vlayer->set_line_width(3);
        vlayer->set_polygon_border_color(*wxBLUE);
        vlayer->set_polygon_border_style(wxSOLID);
        vlayer->set_polygon_border_width(3);
        vlayer->set_polygon_inner_color(*wxRED);
        vlayer->set_polygon_inner_style(wxTRANSPARENT);
        vlayer->text_visibility(false);
        
        boost::filesystem::path file(clayer->Filename());
        param::Instance()->set("dem",file);
        param::Instance()->set("xmin",0);
        param::Instance()->set("ymin",0);
        param::Instance()->set("xmax",p1.x-p0.x);
        param::Instance()->set("ymax",p1.y-p0.y);
        
        m_vlayer = vlayer;
        m_controler->go((void *)&clayer); // todo
    }
    catch( const exception &e )
    {
        wxString message( e.what() , *wxConvCurrent );
        ::wxMessageBox( message , _("Exception!") , wxICON_ERROR );
    }
}

wxAboutDialogInfo rjmcmc_building_footprint_extraction_frame::getAboutInfo() const
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
