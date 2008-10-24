///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "noname.h"

///////////////////////////////////////////////////////////////////////////

MyPanel1::MyPanel1( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* m_mainSizer;
	m_mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* m_saSizer;
	m_saSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Simulated annealing") ), wxVERTICAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticInitialTemperature = new wxStaticText( this, wxID_ANY, wxT("Initial temperature"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticInitialTemperature->Wrap( -1 );
	bSizer2->Add( m_staticInitialTemperature, 1, wxALL, 10 );
	
	m_txtInitialTemperature = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_txtInitialTemperature, 1, wxALL, 5 );
	
	m_staticDecreaseCoefficient = new wxStaticText( this, wxID_ANY, wxT("Decrease coefficient"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticDecreaseCoefficient->Wrap( -1 );
	bSizer2->Add( m_staticDecreaseCoefficient, 1, wxALL, 10 );
	
	m_txtDecreaseCoefficient = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_txtDecreaseCoefficient, 1, wxALL, 5 );
	
	m_staticNbIterations = new wxStaticText( this, wxID_ANY, wxT("Number of iterations"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticNbIterations->Wrap( -1 );
	bSizer2->Add( m_staticNbIterations, 1, wxALL, 10 );
	
	m_txtNbIterations = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_txtNbIterations, 1, wxALL, 5 );
	
	m_saSizer->Add( bSizer2, 1, wxEXPAND, 5 );
	
	m_mainSizer->Add( m_saSizer, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* m_probasSizer;
	m_probasSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("label") ), wxVERTICAL );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticBirth = new wxStaticText( this, wxID_ANY, wxT("Birth"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticBirth->Wrap( -1 );
	bSizer3->Add( m_staticBirth, 1, wxALL, 10 );
	
	m_textCtrl7 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_textCtrl7, 1, wxALL, 5 );
	
	m_staticDeath = new wxStaticText( this, wxID_ANY, wxT("Death"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticDeath->Wrap( -1 );
	bSizer3->Add( m_staticDeath, 1, wxALL, 10 );
	
	m_textCtrl8 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_textCtrl8, 1, wxALL, 5 );
	
	m_staticModifications = new wxStaticText( this, wxID_ANY, wxT("Modifications"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticModifications->Wrap( -1 );
	bSizer3->Add( m_staticModifications, 1, wxALL, 10 );
	
	m_textCtrl9 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl9->Enable( false );
	
	bSizer3->Add( m_textCtrl9, 1, wxALL, 5 );
	
	m_probasSizer->Add( bSizer3, 1, wxEXPAND, 5 );
	
	m_mainSizer->Add( m_probasSizer, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* m_sizerData;
	m_sizerData = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Data") ), wxVERTICAL );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticFilePath = new wxStaticText( this, wxID_ANY, wxT("Image file path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticFilePath->Wrap( -1 );
	bSizer7->Add( m_staticFilePath, 1, wxALL, 10 );
	
	m_filePickerImage = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select an image file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE );
	bSizer7->Add( m_filePickerImage, 1, wxALL, 5 );
	
	
	bSizer7->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText10 = new wxStaticText( this, wxID_ANY, wxT("Image width"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	bSizer7->Add( m_staticText10, 1, wxALL, 10 );
	
	m_textCtrl12 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_textCtrl12, 1, wxALL, 5 );
	
	bSizer6->Add( bSizer7, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText9 = new wxStaticText( this, wxID_ANY, wxT("Variance Gaussian filter"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	bSizer9->Add( m_staticText9, 1, wxALL, 10 );
	
	m_textCtrl11 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_textCtrl11, 1, wxALL, 5 );
	
	
	bSizer9->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText12 = new wxStaticText( this, wxID_ANY, wxT("Image height"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	bSizer9->Add( m_staticText12, 1, wxALL, 10 );
	
	m_textCtrl14 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_textCtrl14, 1, wxALL, 5 );
	
	bSizer8->Add( bSizer9, 1, wxEXPAND, 5 );
	
	bSizer6->Add( bSizer8, 1, wxEXPAND, 5 );
	
	m_sizerData->Add( bSizer6, 1, wxEXPAND, 5 );
	
	m_mainSizer->Add( m_sizerData, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* m_sizerEnergy;
	m_sizerEnergy = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Energy") ), wxVERTICAL );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticIndividualPriorEnergy = new wxStaticText( this, wxID_ANY, wxT("Individual prior energy"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticIndividualPriorEnergy->Wrap( -1 );
	bSizer4->Add( m_staticIndividualPriorEnergy, 1, wxALL, 10 );
	
	m_txtIndividualPriorEnergy = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_txtIndividualPriorEnergy, 1, wxALL, 5 );
	
	
	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_sizerEnergy->Add( bSizer4, 1, wxEXPAND, 5 );
	
	m_mainSizer->Add( m_sizerEnergy, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* m_sizerRectangles;
	m_sizerRectangles = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Rectangles") ), wxVERTICAL );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText13 = new wxStaticText( this, wxID_ANY, wxT("Minimum size"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	bSizer11->Add( m_staticText13, 1, wxALL, 10 );
	
	m_textCtrl15 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_textCtrl15, 1, wxALL, 10 );
	
	
	bSizer11->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText14 = new wxStaticText( this, wxID_ANY, wxT("Maximum ratio"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText14->Wrap( -1 );
	bSizer11->Add( m_staticText14, 1, wxALL, 10 );
	
	m_textCtrl16 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_textCtrl16, 1, wxALL, 10 );
	
	m_sizerRectangles->Add( bSizer11, 1, wxEXPAND, 5 );
	
	m_mainSizer->Add( m_sizerRectangles, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* m_sizerPointsSurfaces;
	m_sizerPointsSurfaces = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Points / Surfaces") ), wxVERTICAL );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText15 = new wxStaticText( this, wxID_ANY, wxT("Intersection surface ponderation"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	bSizer12->Add( m_staticText15, 1, wxALL, 10 );
	
	m_textCtrl18 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( m_textCtrl18, 1, wxALL, 5 );
	
	
	bSizer12->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_sizerPointsSurfaces->Add( bSizer12, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText16 = new wxStaticText( this, wxID_ANY, wxT("Points distance ponderation"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	bSizer13->Add( m_staticText16, 1, wxALL, 10 );
	
	m_textCtrl19 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_textCtrl19, 1, wxALL, 5 );
	
	
	bSizer13->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_sizerPointsSurfaces->Add( bSizer13, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText17 = new wxStaticText( this, wxID_ANY, wxT("Points maximum distance"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	bSizer14->Add( m_staticText17, 1, wxALL, 10 );
	
	m_textCtrl20 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer14->Add( m_textCtrl20, 1, wxALL, 5 );
	
	
	bSizer14->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_sizerPointsSurfaces->Add( bSizer14, 1, wxEXPAND, 5 );
	
	m_mainSizer->Add( m_sizerPointsSurfaces, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( m_mainSizer );
	this->Layout();
}

MyPanel1::~MyPanel1()
{
}
