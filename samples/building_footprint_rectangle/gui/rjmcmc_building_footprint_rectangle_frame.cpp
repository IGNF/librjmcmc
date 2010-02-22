#include <fstream>
#include <sstream>

#include <wx/artprov.h>

#include <GilViewer/layers/VectorLayerGhost.h>
#include <GilViewer/layers/ImageLayer.hpp>
#include <GilViewer/layers/VectorLayer.hpp>
#include <GilViewer/gui/ApplicationSettings.hpp>
#include <GilViewer/gui/PanelManager.h>
#include <GilViewer/gui/define_id.hpp>

#include "param/wx_parameter_traits.hpp"
#include "core/building_footprint_rectangle_parameters_inc.hpp"

#include "parameters_frame.hpp"
#include "rjmcmc_building_footprint_rectangle_frame.hpp"
#include "rjmcmc_building_footprint_rectangle_thread.hpp"
#include "chart_frame.hpp"


enum
{
	ID_BUTTON_GO,
	ID_BUTTON_STOP
};

using namespace std;

BEGIN_EVENT_TABLE(rjmcmc_building_footprint_rectangle_frame,BasicViewerFrame)
        EVT_BUTTON(ID_BUTTON_GO,rjmcmc_building_footprint_rectangle_frame::OnGoButton)
        EVT_BUTTON(ID_BUTTON_STOP,rjmcmc_building_footprint_rectangle_frame::OnStopButton)
        ADD_GILVIEWER_EVENTS_TO_TABLE(rjmcmc_building_footprint_rectangle_frame)
END_EVENT_TABLE();

IMPLEMENTS_GILVIEWER_METHODS_FOR_EVENTS_TABLE(rjmcmc_building_footprint_rectangle_frame,m_panel)

rjmcmc_building_footprint_rectangle_frame::rjmcmc_building_footprint_rectangle_frame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos , const wxSize& size , long style , const wxString& name ):
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

	m_statusBar->SetStatusText(_("libRJMCMC"));

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
	applicationToolBarInfo.Caption( _("libRJMCMC toolbar") );
	applicationToolBarInfo.ToolbarPane();
	applicationToolBarInfo.Top();
	applicationToolBarInfo.CloseButton(false);
	applicationToolBarInfo.CaptionVisible(false);
	wxToolBar *libRJMCMCtoolbar = new wxToolBar(this,wxID_ANY);
	m_buttonGo = new wxBitmapButton(libRJMCMCtoolbar, ID_BUTTON_GO, wxArtProvider::GetBitmap(wxART_TICK_MARK, wxART_BUTTON, wxSize(22,22)) );
	m_buttonStop = new wxBitmapButton(libRJMCMCtoolbar, ID_BUTTON_STOP, wxArtProvider::GetBitmap(wxART_CROSS_MARK, wxART_BUTTON, wxSize(22,22)) );
	m_buttonStop->Disable();
	libRJMCMCtoolbar->AddControl(m_buttonGo);
	libRJMCMCtoolbar->AddControl(m_buttonStop);
	m_dockManager.AddPane( libRJMCMCtoolbar , applicationToolBarInfo );

	m_dockManager.Update();
	m_parameters_frame = new parameters_frame(this);
	m_parameters_frame->Show();

	m_chart_frame = new chart_frame(this);
	m_chart_frame->Show();
}

rjmcmc_building_footprint_rectangle_frame::~rjmcmc_building_footprint_rectangle_frame()
{
	m_thread->Delete();
	m_parameters_frame->Destroy();
	m_parameters_frame = NULL;
	m_chart_frame->Destroy();
	m_chart_frame = NULL;
}

void rjmcmc_building_footprint_rectangle_frame::OnGoButton(wxCommandEvent& event)
{
	param::Instance()->update_values();

	LayerControl::const_iterator it  = m_panel->GetLayerControl()->begin();
	LayerControl::const_iterator end = m_panel->GetLayerControl()->end();
	Layer::ptrLayerType ilayer;
	for(;it!=end && !ilayer;++it) {
		if(dynamic_cast<ImageLayer *>(&**it)) ilayer=*it; //use dem tag??
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
		  OnThreadEnd();
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
                Layer::ptrLayerType vlayer = VectorLayer::CreateVectorLayer(std::string("Buildings"), 1/*IMAGE_COORDINATES*/, false);
                Layer::ptrLayerType clayer = ilayer->crop(p0.x,p0.y,p1.x,p1.y);
                if(!clayer) {
			  std::ostringstream oss;
                          oss << "Cropping outside the bounds of " << ilayer->Filename() << " !";
			  wxString message( oss.str().c_str() , *wxConvCurrent );
			  ::wxMessageBox( message , _("Error !") , wxICON_ERROR );
			  OnThreadEnd();
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
                vlayer->set_style(*wxRED, *wxBLUE, wxTRANSPARENT, wxSOLID, 3);
                vlayer->TextsVisibility(false);
		
                boost::filesystem::path file(clayer->Filename());
		param::Instance()->set("dem",file);
		param::Instance()->set("xmin",0);
                param::Instance()->set("ymin",0);
		param::Instance()->set("xmax",p1.x-p0.x);
		param::Instance()->set("ymax",p1.y-p0.y);
		
		m_parameters_frame->Refresh();

                m_thread = new rjmcmc_building_footprint_rectangle_thread(clayer,vlayer,this);

		m_thread->Create();
		m_thread->Run();
		m_buttonGo->Disable();
		m_buttonStop->Enable();
		m_parameters_frame->Disable();
	}
	catch( const exception &e )
	{
		wxString message( e.what() , *wxConvCurrent );
		::wxMessageBox( message , _("Exception!") , wxICON_ERROR );
	}
}

void rjmcmc_building_footprint_rectangle_frame::OnStopButton(wxCommandEvent& event)
{
	m_thread->Delete();
}

void rjmcmc_building_footprint_rectangle_frame::OnThreadEnd()
{
	m_buttonStop->Disable();
	m_buttonGo->Enable();
	if(m_parameters_frame) {
		m_parameters_frame->Enable();
		m_parameters_frame->Show();
	}
}

wxAboutDialogInfo rjmcmc_building_footprint_rectangle_frame::getAboutInfo() const
{
	wxAboutDialogInfo info;
	info.AddDeveloper(_("Olivier Tournaire"));
	info.AddDeveloper(_("Didier Boldo"));
	info.AddDeveloper(_("Mathieu Brédif"));
	info.SetName(_("libRJMCMC Buildings detection sample"));
	info.SetVersion(_("0.1.0"));
	info.SetWebSite(_("http://??????"), _("Home page") );
	info.SetDescription(_("Marked point process for buildings detection from DEM"));
	info.SetCopyright(_T("olivier.tournaire@ign.fr\nmathieu.bredif@ign.fr"));
	return info;
}
