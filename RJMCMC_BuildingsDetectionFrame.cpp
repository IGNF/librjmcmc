#include "gui/ApplicationSettings.hpp"
#include "layers/VectorLayer.hpp"
#include "layers/ImageLayer.hpp"
#include "gui/PanelManager.h"
#include "gui/define_id.hpp"

#include "RJMCMC_BuildingsDetectionFrame.hpp"
#include "BuildingsDetectorParameters.hpp"
#include "ParametersFrame.h"

enum
{
	ID_BUTTON_GO = wxID_HIGHEST + 1
};

using namespace std;

BEGIN_EVENT_TABLE(RJMCMC_BuildingsDetectionFrame,wxFrame)
	EVT_BUTTON(ID_BUTTON_GO,RJMCMC_BuildingsDetectionFrame::OnGoButton)
	ADD_ITKVIEWER_EVENTS_TO_TABLE(RJMCMC_BuildingsDetectionFrame)
END_EVENT_TABLE();

IMPLEMENTS_ITKVIEWER_METHODS_FOR_EVENTS_TABLE(RJMCMC_BuildingsDetectionFrame,m_panel)

RJMCMC_BuildingsDetectionFrame::RJMCMC_BuildingsDetectionFrame(bool with_parameters, const wxString& title, const wxPoint& pos,const wxSize& size) : wxFrame( (wxFrame *)NULL, -1 , title , pos , size, wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL )
{
	wxInitAllImageHandlers();

	if (with_parameters)
	{
		parameters = new ParametersFrame();
		parameters->Show();
	}
	else 
		parameters = 00;

	// Le sizer contenant "tout"
	wxBoxSizer *main_sizer = new wxBoxSizer( wxHORIZONTAL );

	PanelViewer::Register(this);
	m_panel = PanelManager::Instance()->createObject("PanelViewer");

	m_panel->GetLayerControl()->Show(true);
	main_sizer->Add(m_panel, 1, wxEXPAND|wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5);
	this->SetStatusBar( m_panel->GetStatusBar() );
	this->SetToolBar( m_panel->GetToolBar() );

	m_buttonGo = new wxButton(m_panel->GetToolBar(),ID_BUTTON_GO,_("Go"));
	m_panel->GetToolBar()->AddControl( m_buttonGo );

	this->SetSizerAndFit(main_sizer);
	this->SetSize(800,600);
	main_sizer->Layout();
	
	if (!with_parameters)
	{
		wxCommandEvent ev;
		OnGoButton(ev);
	}
}

void RJMCMC_BuildingsDetectionFrame::OnGoButton(wxCommandEvent& event)
{
	m_buttonGo->Disable();
	if (parameters != 00)
	{
		std::vector< std::pair<std::string, std::string> > pileText1;
		for (unsigned int i=0; i<parameters->m_pileText.size(); ++i)
		{
			parameters->m_pileText[i].second->Disable();
			pileText1.push_back(
					make_pair(
							std::string( parameters->m_pileText[i].first->GetLabel().fn_str() ),
							std::string( parameters->m_pileText[i].second->GetLineText(0).fn_str() )
							));
		}
		BuildingsDetectorParametersSingleton::Instance()->SetFromText(pileText1);
	}
	m_panel->AddLayer(ImageLayer<signed short,1>::CreateImageLayer(BuildingsDetectorParametersSingleton::Instance()->InputDataFilePath()));
//	m_panel->AddLayer(ImageLayer<unsigned short,1>::CreateImageLayer(BuildingsDetectorParametersSingleton::Instance()->InputDataFilePath()));

//	Layer::ptrLayerType layer = VectorLayer::CreateVectorLayer(std::string("Points") , SHPT_POINT);
//	Layer::ptrLayerType layer = VectorLayer::CreateVectorLayer(std::string("Polylines") , SHPT_ARC);
	Layer::ptrLayerType layer = VectorLayer::CreateVectorLayer(std::string("Buildings") , SHPT_POLYGON);
	m_panel->AddLayer(layer);

	wxLog *logger=new wxLogWindow(this, _("LOG"));
	wxLog::SetActiveTarget(logger);

	m_thread = new RJMCMC_BuildingsDetectionThread(layer, m_panel, this );
	m_thread->Create();
	m_thread->Run();
}

void RJMCMC_BuildingsDetectionFrame::OnThreadEnd()
{
	m_buttonGo->Enable();
	for (unsigned int i=0; i<parameters->m_pileText.size(); ++i)
	{
		parameters->m_pileText[i].second->Enable();
	}
}

class MyApp : public wxApp
{
public:
	RJMCMC_BuildingsDetectionFrame *m_frame;

private:
	bool OnInit()
	{
		bool with_parameters = true;
		if (argc > 1)
		{
			char **my_argv = new char*[argc];
			for (int i=0;i<argc;++i)
			{
				wxString mystr(argv[i]);
				my_argv[i] = new char[mystr.size()+1];
				strcpy(my_argv[i], mystr.To8BitData());
			}
			BuildingsDetectorParametersSingleton::Instance()->ParseCmdLine(argc,my_argv);
			with_parameters = false;
		}
		m_frame = new RJMCMC_BuildingsDetectionFrame(with_parameters, _("RJMCMC Detection") , wxDefaultPosition , wxDefaultSize );
		m_frame->Show();
		return true;
	}
};

IMPLEMENT_APP(MyApp);
