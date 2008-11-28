#include "use_ihm.h"
#if USE_IHM

#include "layers/VectorLayer.hpp"
#include "layers/ImageLayer.hpp"
#include "gui/PanelManager.h"

#include "RJMCMC_BuildingsDetectionFrame.hpp"

enum
{
	ID_BUTTON_GO = wxID_HIGHEST + 1
};

using namespace std;

BEGIN_EVENT_TABLE(RJMCMC_BuildingsDetectionFrame,wxFrame)
	EVT_BUTTON(ID_BUTTON_GO,RJMCMC_BuildingsDetectionFrame::OnGoButton)
END_EVENT_TABLE();

RJMCMC_BuildingsDetectionFrame::RJMCMC_BuildingsDetectionFrame(const wxString& title, const wxPoint& pos,const wxSize& size) : wxFrame( (wxFrame *)NULL, -1 , title , pos , size, wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL )
{
	wxInitAllImageHandlers();

	// Le sizer contenant "tout"
	wxBoxSizer *main_sizer = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer *test_sizer= new wxBoxSizer( wxVERTICAL );
	wxPanel *test = new wxPanel(this,wxID_ANY,wxDefaultPosition,wxSize(350,1024));
	main_sizer->Add(test_sizer, 0, wxEXPAND|wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5);

	PanelViewer::Register(this);
	m_panel = PanelManager::Instance()->createObject("PanelViewer");

	m_panel->GetLayerControl()->Show(true);
	main_sizer->Add(m_panel, 1, wxEXPAND|wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5);
	this->SetStatusBar( m_panel->GetStatusBar() );
	this->SetToolBar( m_panel->GetToolBar() );

	//wxBoxSizer *button_sizer= new wxBoxSizer( wxVERTICAL);
	m_buttonGo = new wxButton(test,ID_BUTTON_GO,_("Go"),wxDefaultPosition,wxSize(90,40));
	test_sizer->Add(m_buttonGo, 0, wxEXPAND|wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5);

	std::vector< std::pair<std::string, std::string> > pileText;
	BuildingsDetectorParametersSingleton::Instance()->GetAsText(pileText);
	for (unsigned int i=0; i<pileText.size(); ++i)
	{
		m_pileText.push_back(make_pair(new wxStaticText(test, -1, wxString(pileText[i].first.c_str(),*wxConvCurrent)),new wxTextCtrl(test, -1, wxString(pileText[i].second.c_str(),*wxConvCurrent))));
		test_sizer->Add(m_pileText.back().first, 0, wxEXPAND|wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5);
		test_sizer->Add(m_pileText.back().second, 0, wxEXPAND|wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5);
	}

	//main_sizer->Add(button_sizer, 0, wxEXPAND|wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5);

	//panel_sizer->Layout();
	//button_sizer->Layout();
	//main_sizer->Layout();
	this->SetSizerAndFit(main_sizer);
	test_sizer->Layout();
	this->SetSize(1000,1300);
}

void RJMCMC_BuildingsDetectionFrame::OnGoButton(wxCommandEvent& event)
{
	m_buttonGo->Disable();
	std::vector< std::pair<std::string, std::string> > pileText;
	for (unsigned int i=0; i<m_pileText.size(); ++i)
	{
		m_pileText[i].second->Disable();
		pileText.push_back(
				make_pair(
						std::string( m_pileText[i].first->GetLabel().fn_str() ),
						std::string( m_pileText[i].second->GetLineText(0).fn_str() )
						));
	}
	BuildingsDetectorParametersSingleton::Instance()->SetFromText(pileText);

	//m_panel->AddLayer(ImageLayer<signed short>::CreateImageLayer(BuildingsDetectorParametersSingleton::Instance()->InputDataFilePath()));
	m_panel->AddLayer(ImageLayer<unsigned short,1>::CreateImageLayer(BuildingsDetectorParametersSingleton::Instance()->InputDataFilePath()));

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
	for (unsigned int i=0; i<m_pileText.size(); ++i)
	{
		m_pileText[i].second->Enable();
	}
}

class MyApp : public wxApp
{
public:
	RJMCMC_BuildingsDetectionFrame *m_frame;

private:
	bool OnInit()
	{
		m_frame = new RJMCMC_BuildingsDetectionFrame( _("RJMCMC Detection") , wxDefaultPosition , wxDefaultSize );
		m_frame->Show();
		return true;
	}
};

IMPLEMENT_APP(MyApp);

#endif //#if USE_IHM

