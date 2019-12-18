///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug  4 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "noname.h"

///////////////////////////////////////////////////////////////////////////

AssetsFrame::AssetsFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* Sizer0;
	Sizer0 = new wxBoxSizer( wxVERTICAL );
	
	Tabs = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	TabPanelGeometry = new wxPanel( Tabs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* Sizer1;
	Sizer1 = new wxFlexGridSizer( 1, 2, 0, 0 );
	Sizer1->SetFlexibleDirection( wxBOTH );
	Sizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	LstGeometry = new wxListBox( TabPanelGeometry, wxID_ANY, wxDefaultPosition, wxSize( 200,270 ), 0, NULL, wxLB_ALWAYS_SB ); 
	Sizer1->Add( LstGeometry, 0, wxALL, 5 );
	
	wxFlexGridSizer* Sizer2;
	Sizer2 = new wxFlexGridSizer( 9, 2, 0, 0 );
	Sizer2->AddGrowableCol( 1 );
	Sizer2->SetFlexibleDirection( wxBOTH );
	Sizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	LblName = new wxStaticText( TabPanelGeometry, wxID_ANY, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	LblName->Wrap( -1 );
	Sizer2->Add( LblName, 0, wxALL, 5 );
	
	TxtName = new wxTextCtrl( TabPanelGeometry, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 160,-1 ), wxTE_PROCESS_ENTER );
	Sizer2->Add( TxtName, 0, wxALL, 5 );
	
	LblMaterial = new wxStaticText( TabPanelGeometry, wxID_ANY, wxT("Material:"), wxDefaultPosition, wxDefaultSize, 0 );
	LblMaterial->Wrap( -1 );
	Sizer2->Add( LblMaterial, 0, wxALL, 5 );
	
	wxBoxSizer* Sizer3;
	Sizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	TxtMaterial = new wxTextCtrl( TabPanelGeometry, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 160,-1 ), wxTE_READONLY );
	Sizer3->Add( TxtMaterial, 0, wxALL, 5 );
	
	BtnBrowseMaterial = new wxButton( TabPanelGeometry, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	Sizer3->Add( BtnBrowseMaterial, 0, wxALL, 5 );
	
	Sizer2->Add( Sizer3, 1, wxEXPAND, 5 );
	
	LblPhysics = new wxStaticText( TabPanelGeometry, wxID_ANY, wxT("Physics cage:"), wxDefaultPosition, wxDefaultSize, 0 );
	LblPhysics->Wrap( -1 );
	Sizer2->Add( LblPhysics, 0, wxALL, 5 );
	
	wxString DrpPhysicsChoices[] = { wxT("No Physics/Collision"), wxT("Box Physics"), wxT("Sphere Physics"), wxT("Capsule Physics"), wxT("Custom Wrap Physics"), wxT("Static Level Collision") };
	int DrpPhysicsNChoices = sizeof( DrpPhysicsChoices ) / sizeof( wxString );
	DrpPhysics = new wxChoice( TabPanelGeometry, wxID_ANY, wxDefaultPosition, wxSize( 160,-1 ), DrpPhysicsNChoices, DrpPhysicsChoices, 0 );
	DrpPhysics->SetSelection( 0 );
	Sizer2->Add( DrpPhysics, 0, wxALL, 5 );
	
	LblMass = new wxStaticText( TabPanelGeometry, wxID_ANY, wxT("Mass:"), wxDefaultPosition, wxDefaultSize, 0 );
	LblMass->Wrap( -1 );
	Sizer2->Add( LblMass, 0, wxALL, 5 );
	
	SpnMass = new wxSpinCtrl( TabPanelGeometry, wxID_ANY, wxT("2"), wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 0, 100, 2 );
	Sizer2->Add( SpnMass, 0, wxALL, 5 );
	
	LblPosition = new wxStaticText( TabPanelGeometry, wxID_ANY, wxT("Position:"), wxDefaultPosition, wxDefaultSize, 0 );
	LblPosition->Wrap( -1 );
	Sizer2->Add( LblPosition, 0, wxALL, 5 );
	
	wxBoxSizer* Sizer4;
	Sizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	SpnPositionX = new wxSpinCtrl( TabPanelGeometry, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 0, 1000000, 0 );
	Sizer4->Add( SpnPositionX, 0, wxALL, 5 );
	
	SpnPositionY = new wxSpinCtrl( TabPanelGeometry, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 0, 1000000, 0 );
	Sizer4->Add( SpnPositionY, 0, wxALL, 5 );
	
	SpnPositionZ = new wxSpinCtrl( TabPanelGeometry, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 0, 1000000, 0 );
	Sizer4->Add( SpnPositionZ, 0, wxALL, 5 );
	
	Sizer2->Add( Sizer4, 1, wxEXPAND, 5 );
	
	LblRotation = new wxStaticText( TabPanelGeometry, wxID_ANY, wxT("Rotation:"), wxDefaultPosition, wxDefaultSize, 0 );
	LblRotation->Wrap( -1 );
	Sizer2->Add( LblRotation, 0, wxALL, 5 );
	
	wxBoxSizer* Sizer5;
	Sizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	SpnRotationX = new wxSpinCtrl( TabPanelGeometry, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 0, 360, 0 );
	Sizer5->Add( SpnRotationX, 0, wxALL, 5 );
	
	SpnRotationY = new wxSpinCtrl( TabPanelGeometry, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 0, 360, 0 );
	Sizer5->Add( SpnRotationY, 0, wxALL, 5 );
	
	SpnRotationZ = new wxSpinCtrl( TabPanelGeometry, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 0, 360, 0 );
	Sizer5->Add( SpnRotationZ, 0, wxALL, 5 );
	
	Sizer2->Add( Sizer5, 1, wxEXPAND, 5 );
	
	LblScale = new wxStaticText( TabPanelGeometry, wxID_ANY, wxT("Scale:"), wxDefaultPosition, wxDefaultSize, 0 );
	LblScale->Wrap( -1 );
	Sizer2->Add( LblScale, 0, wxALL, 5 );
	
	wxBoxSizer* Sizer6;
	Sizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	SpnScaleX = new wxSpinCtrl( TabPanelGeometry, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 0, 1000, 0 );
	Sizer6->Add( SpnScaleX, 0, wxALL, 5 );
	
	SpnScaleY = new wxSpinCtrl( TabPanelGeometry, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 0, 1000, 0 );
	Sizer6->Add( SpnScaleY, 0, wxALL, 5 );
	
	SpnScaleZ = new wxSpinCtrl( TabPanelGeometry, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 0, 1000, 0 );
	Sizer6->Add( SpnScaleZ, 0, wxALL, 5 );
	
	Sizer2->Add( Sizer6, 1, wxEXPAND, 5 );
	
	LblUVTile = new wxStaticText( TabPanelGeometry, wxID_ANY, wxT("Texture tile:"), wxDefaultPosition, wxDefaultSize, 0 );
	LblUVTile->Wrap( -1 );
	Sizer2->Add( LblUVTile, 0, wxALL, 5 );
	
	wxBoxSizer* Sizer7;
	Sizer7 = new wxBoxSizer( wxHORIZONTAL );
	
	SpnTileU = new wxSpinCtrl( TabPanelGeometry, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 0, 100, 0 );
	Sizer7->Add( SpnTileU, 0, wxALL, 5 );
	
	SpnTileV = new wxSpinCtrl( TabPanelGeometry, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 0, 100, 0 );
	Sizer7->Add( SpnTileV, 0, wxALL, 5 );
	
	Sizer2->Add( Sizer7, 1, wxEXPAND, 5 );
	
	LblUVOffset = new wxStaticText( TabPanelGeometry, wxID_ANY, wxT("Texture offset:"), wxDefaultPosition, wxDefaultSize, 0 );
	LblUVOffset->Wrap( -1 );
	Sizer2->Add( LblUVOffset, 0, wxALL, 5 );
	
	wxBoxSizer* Sizer8;
	Sizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	SpnOffsetU = new wxSpinCtrl( TabPanelGeometry, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 0, 100, 0 );
	Sizer8->Add( SpnOffsetU, 0, wxALL, 5 );
	
	SpnOffsetV = new wxSpinCtrl( TabPanelGeometry, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 0, 100, 0 );
	Sizer8->Add( SpnOffsetV, 0, wxALL, 5 );
	
	Sizer2->Add( Sizer8, 1, wxEXPAND, 5 );
	
	Sizer1->Add( Sizer2, 1, wxEXPAND, 5 );
	
	TabPanelGeometry->SetSizer( Sizer1 );
	TabPanelGeometry->Layout();
	Sizer1->Fit( TabPanelGeometry );
	Tabs->AddPage( TabPanelGeometry, wxT("Geometry"), true );
	TabPanelSasha = new wxPanel( Tabs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* Sizer9;
	Sizer9 = new wxBoxSizer( wxVERTICAL );
	
	ChkGrapple1 = new wxCheckBox( TabPanelSasha, wxID_ANY, wxT("Grapple object 1 (Hookshot)"), wxDefaultPosition, wxDefaultSize, 0 );
	Sizer9->Add( ChkGrapple1, 0, wxALL, 5 );
	
	ChkGrapple2 = new wxCheckBox( TabPanelSasha, wxID_ANY, wxT("Grapple object 2 (Flexible)"), wxDefaultPosition, wxDefaultSize, 0 );
	Sizer9->Add( ChkGrapple2, 0, wxALL, 5 );
	
	ChkVision1 = new wxCheckBox( TabPanelSasha, wxID_ANY, wxT("Vision mode 1 (Hologram)"), wxDefaultPosition, wxDefaultSize, 0 );
	Sizer9->Add( ChkVision1, 0, wxALL, 5 );
	
	ChkVision2 = new wxCheckBox( TabPanelSasha, wxID_ANY, wxT("Vision mode 2 (Cloaked)"), wxDefaultPosition, wxDefaultSize, 0 );
	Sizer9->Add( ChkVision2, 0, wxALL, 5 );
	
	ChkSpawn = new wxCheckBox( TabPanelSasha, wxID_ANY, wxT("Enemy spawn point"), wxDefaultPosition, wxDefaultSize, 0 );
	Sizer9->Add( ChkSpawn, 0, wxALL, 5 );
	
	TabPanelSasha->SetSizer( Sizer9 );
	TabPanelSasha->Layout();
	Sizer9->Fit( TabPanelSasha );
	Tabs->AddPage( TabPanelSasha, wxT("Sasha"), false );
	
	Sizer0->Add( Tabs, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( Sizer0 );
	this->Layout();
	
	// Connect Events
	LstGeometry->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( AssetsFrame::OnSelect ), NULL, this );
	TxtName->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( AssetsFrame::OnNameChange ), NULL, this );
	BtnBrowseMaterial->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AssetsFrame::OnMaterialBrowse ), NULL, this );
	DrpPhysics->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( AssetsFrame::OnPhysicsChange ), NULL, this );
	SpnMass->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnMassChange ), NULL, this );
	SpnPositionX->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnPositionChange ), NULL, this );
	SpnPositionY->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnPositionChange ), NULL, this );
	SpnPositionZ->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnPositionChange ), NULL, this );
	SpnRotationX->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnRotationChange ), NULL, this );
	SpnRotationY->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnRotationChange ), NULL, this );
	SpnRotationZ->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnRotationChange ), NULL, this );
	SpnScaleX->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnScaleChange ), NULL, this );
	SpnScaleY->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnScaleChange ), NULL, this );
	SpnScaleZ->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnScaleChange ), NULL, this );
	SpnTileU->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnUVChange ), NULL, this );
	SpnTileV->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnUVChange ), NULL, this );
	SpnOffsetU->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnUVChange ), NULL, this );
	SpnOffsetV->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnUVChange ), NULL, this );
	ChkGrapple1->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AssetsFrame::OnChkGrapple1 ), NULL, this );
	ChkGrapple2->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AssetsFrame::OnChkGrapple2 ), NULL, this );
	ChkVision1->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AssetsFrame::OnChkVision1 ), NULL, this );
	ChkVision2->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AssetsFrame::OnChkVision2 ), NULL, this );
	ChkSpawn->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AssetsFrame::OnChkSpawn ), NULL, this );
}

AssetsFrame::~AssetsFrame()
{
	// Disconnect Events
	LstGeometry->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( AssetsFrame::OnSelect ), NULL, this );
	TxtName->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( AssetsFrame::OnNameChange ), NULL, this );
	BtnBrowseMaterial->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AssetsFrame::OnMaterialBrowse ), NULL, this );
	DrpPhysics->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( AssetsFrame::OnPhysicsChange ), NULL, this );
	SpnMass->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnMassChange ), NULL, this );
	SpnPositionX->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnPositionChange ), NULL, this );
	SpnPositionY->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnPositionChange ), NULL, this );
	SpnPositionZ->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnPositionChange ), NULL, this );
	SpnRotationX->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnRotationChange ), NULL, this );
	SpnRotationY->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnRotationChange ), NULL, this );
	SpnRotationZ->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnRotationChange ), NULL, this );
	SpnScaleX->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnScaleChange ), NULL, this );
	SpnScaleY->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnScaleChange ), NULL, this );
	SpnScaleZ->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnScaleChange ), NULL, this );
	SpnTileU->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnUVChange ), NULL, this );
	SpnTileV->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnUVChange ), NULL, this );
	SpnOffsetU->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnUVChange ), NULL, this );
	SpnOffsetV->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( AssetsFrame::OnUVChange ), NULL, this );
	ChkGrapple1->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AssetsFrame::OnChkGrapple1 ), NULL, this );
	ChkGrapple2->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AssetsFrame::OnChkGrapple2 ), NULL, this );
	ChkVision1->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AssetsFrame::OnChkVision1 ), NULL, this );
	ChkVision2->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AssetsFrame::OnChkVision2 ), NULL, this );
	ChkSpawn->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AssetsFrame::OnChkSpawn ), NULL, this );
}
