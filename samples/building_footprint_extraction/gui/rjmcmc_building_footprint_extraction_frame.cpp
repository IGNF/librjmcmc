#include <fstream>
#include <sstream>

#include <wx/artprov.h>

#include <GilViewer/layers/ImageLayer.hpp>
#include <GilViewer/layers/VectorLayer.hpp>
#include <GilViewer/gui/ApplicationSettings.hpp>
#include <GilViewer/gui/PanelManager.h>
#include <GilViewer/gui/define_id.hpp>

#include "core/building_footprint_extraction_parameters.hpp"
#include "parameters_frame.hpp"
#include "rjmcmc_building_footprint_extraction_frame.hpp"
#include "rjmcmc_building_footprint_extraction_thread.hpp"

enum
{
	ID_BUTTON_GO,
	ID_BUTTON_STOP
};

using namespace std;

BEGIN_EVENT_TABLE(rjmcmc_building_footprint_extraction_frame,BasicViewerFrame)
	EVT_BUTTON(ID_BUTTON_GO,rjmcmc_building_footprint_extraction_frame::OnGoButton)
	EVT_BUTTON(ID_BUTTON_STOP,rjmcmc_building_footprint_extraction_frame::OnStopButton)
	ADD_GILVIEWER_EVENTS_TO_TABLE(rjmcmc_building_footprint_extraction_frame)
END_EVENT_TABLE();

IMPLEMENTS_GILVIEWER_METHODS_FOR_EVENTS_TABLE(rjmcmc_building_footprint_extraction_frame,m_panel)

rjmcmc_building_footprint_extraction_frame::rjmcmc_building_footprint_extraction_frame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos , const wxSize& size , long style , const wxString& name ):
	BasicViewerFrame(parent, id, title, pos, size, style, name)
{

#if defined(__WXMSW__)
	// Sous windows, on va chercher l'image dans les resources
	wxIcon icon("logo_matis_small");
	SetIcon (icon);
#else
	//SetIcon(wxICON(LOGO_MATIS_small));
#endif

	PanelViewer::Register(this);
	m_panel = PanelManager::Instance()->createObject("PanelViewer");

	m_statusBar->SetStatusText(_("GilViewer - Adrien Chauve & Olivier Tournaire"));

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
	m_parameters_frame = new parameters_frame();
	m_parameters_frame->SetModal(false);
	m_parameters_frame->Show();
}

rjmcmc_building_footprint_extraction_frame::~rjmcmc_building_footprint_extraction_frame()
{
	m_parameters_frame->Destroy();
}

void rjmcmc_building_footprint_extraction_frame::OnGoButton(wxCommandEvent& event)
{
	m_buttonGo->Disable();
	m_buttonStop->Enable();
	m_parameters_frame->Disable();
	std::map< std::string, std::string > parameters;
	for (unsigned int i=0; i<m_parameters_frame->m_parameters.size(); ++i)
	{
		parameters[std::string( m_parameters_frame->m_parameters[i].first->GetLabel().fn_str() )]
			=  std::string( m_parameters_frame->m_parameters[i].second->GetLineText(0).fn_str() );
	}
	
	// Back up stuffs erased in parse_string_map ...
	bool back_up_do_save = building_footprint_extraction_parameters::Instance()->m_do_save;
	
	building_footprint_extraction_parameters::Instance()->parse_string_map(parameters);
	
	// Restore backuped stuffs
	building_footprint_extraction_parameters::Instance()->m_do_save = back_up_do_save;
	
	// Test if input file exists
	std::ifstream test( building_footprint_extraction_parameters::Instance()->m_input_data_file_path.c_str() , std::ios::in );
	if ( !test.good() )
	{
	  std::ostringstream oss;
	  oss << "File " << building_footprint_extraction_parameters::Instance()->m_input_data_file_path << " does not exist !";
	  wxString message( oss.str().c_str() , *wxConvCurrent );
	  ::wxMessageBox( message , _("Error !") , wxICON_ERROR );
	  // Restore interface ...
	  OnThreadEnd();
	  return;
	}

	try
	{
		m_panel->AddLayer( ImageLayer::CreateImageLayer(building_footprint_extraction_parameters::Instance()->m_input_data_file_path) );
		Layer::ptrLayerType layer = VectorLayer::CreateVectorLayer(std::string("Buildings") , SHPT_POLYGON );
		m_panel->AddLayer(layer);
		layer->PolygonsRingsColour(wxColour(255,255,0));
		layer->PolygonsInsideStyle( wxTRANSPARENT );
		layer->PolygonsRingsWidth(3);
		layer->TextsVisibility(false);

		m_thread = new rjmcmc_building_footprint_extraction_thread(layer, this);
		m_thread->Create();
		m_thread->Run();
	}
	catch( const exception &e )
	{
		wxString message( e.what() , *wxConvCurrent );
		::wxMessageBox( message , _("Exception!") , wxICON_ERROR );
	}
}

void rjmcmc_building_footprint_extraction_frame::OnStopButton(wxCommandEvent& event)
{
	m_thread->Delete();
}

void rjmcmc_building_footprint_extraction_frame::OnThreadEnd()
{
	m_buttonStop->Disable();
	m_buttonGo->Enable();
	m_parameters_frame->Enable();
	m_parameters_frame->Show();
}

wxAboutDialogInfo rjmcmc_building_footprint_extraction_frame::getAboutInfo() const
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
