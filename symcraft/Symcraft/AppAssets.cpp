
#include "AppAssets.h"

AssetsFrame::AssetsFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* Sizer;
	Sizer = new wxBoxSizer( wxVERTICAL );

	Tabs = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );

	///////////////////////////////////////////////////////
	// SCENE TAB
	TabScene = new wxPanel( Tabs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* SceneSizer;
	SceneSizer = new wxBoxSizer( wxHORIZONTAL );

	SceneList = new wxListBox( TabScene, wxID_ANY, wxDefaultPosition, wxSize( 200,200 ), 0, NULL, wxLB_ALWAYS_SB );
	SceneSizer->Add( SceneList, 0, wxALL, 5 );

	wxFlexGridSizer* SceneSizer2;
	SceneSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	SceneSizer2->SetFlexibleDirection( wxBOTH );
	SceneSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	ButtonDelete = new wxButton( TabScene, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	SceneSizer2->Add( ButtonDelete, 0, wxALL, 5 );

	ButtonClone = new wxButton( TabScene, wxID_ANY, wxT("Clone"), wxDefaultPosition, wxDefaultSize, 0 );
	SceneSizer2->Add( ButtonClone, 0, wxALL, 5 );

	NameLabel = new wxStaticText( TabScene, wxID_ANY, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	NameLabel->Wrap( -1 );
	SceneSizer2->Add( NameLabel, 0, wxALL, 5 );

	TextName = new wxTextCtrl( TabScene, wxID_ANY, wxT("None"), wxDefaultPosition, wxSize( 150,-1 ), 0 );
	SceneSizer2->Add( TextName, 0, wxALL, 5 );

	MassLabel = new wxStaticText( TabScene, wxID_ANY, wxT("Mass:"), wxDefaultPosition, wxDefaultSize, 0 );
	MassLabel->Wrap( -1 );
	SceneSizer2->Add( MassLabel, 0, wxALL, 5 );

	SpinMass = new wxSpinCtrl( TabScene, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0 );
	SceneSizer2->Add( SpinMass, 0, wxALL, 5 );

	MaterialLabel = new wxStaticText( TabScene, wxID_ANY, wxT("Material:"), wxDefaultPosition, wxDefaultSize, 0 );
	MaterialLabel->Wrap( -1 );
	SceneSizer2->Add( MaterialLabel, 0, wxALL, 5 );

	TextMaterial = new wxTextCtrl( TabScene, wxID_ANY, wxT("None"), wxDefaultPosition, wxSize( 150,-1 ), wxTE_READONLY );
	SceneSizer2->Add( TextMaterial, 0, wxALL, 5 );

	TxtTexTile = new wxStaticText( TabScene, wxID_ANY, wxT("Texture Tiling:"), wxDefaultPosition, wxDefaultSize, 0 );
	TxtTexTile->Wrap( -1 );
	SceneSizer2->Add( TxtTexTile, 0, wxALL, 5 );
	
	wxBoxSizer* TexTileSizer;
	TexTileSizer = new wxBoxSizer( wxHORIZONTAL );
	
	TexTileX = new wxSpinCtrl( TabScene, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 1.0 );
	TexTileSizer->Add( TexTileX, 1, wxALL|wxEXPAND, 5 );
	
	TexTileY = new wxSpinCtrl( TabScene, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 1.0 );
	TexTileSizer->Add( TexTileY, 1, wxALL, 5 );
	
	SceneSizer2->Add( TexTileSizer, 1, wxEXPAND, 5 );
	
	TxtTexOffset = new wxStaticText( TabScene, wxID_ANY, wxT("Texture Offset:"), wxDefaultPosition, wxDefaultSize, 0 );
	TxtTexOffset->Wrap( -1 );
	SceneSizer2->Add( TxtTexOffset, 0, wxALL, 5 );
	
	wxBoxSizer* TexOffsetSizer;
	TexOffsetSizer = new wxBoxSizer( wxHORIZONTAL );
	
	TexOffsetX = new wxSpinCtrl( TabScene, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0 );
	TexOffsetSizer->Add( TexOffsetX, 1, wxALL, 5 );
	
	TexOffsetY = new wxSpinCtrl( TabScene, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0 );
	TexOffsetSizer->Add( TexOffsetY, 1, wxALL, 5 );
	
	SceneSizer2->Add( TexOffsetSizer, 1, wxEXPAND, 5 );
	
	TxtTexRotate = new wxStaticText( TabScene, wxID_ANY, wxT("Texture Rotate:"), wxDefaultPosition, wxDefaultSize, 0 );
	TxtTexRotate->Wrap( -1 );
	SceneSizer2->Add( TxtTexRotate, 0, wxALL, 5 );
	
	wxBoxSizer* TexRotateSizer;
	TexRotateSizer = new wxBoxSizer( wxHORIZONTAL );
	
	TexRotateZ = new wxSpinCtrl( TabScene, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 360, 0 );
	TexRotateSizer->Add( TexRotateZ, 1, wxALL, 5 );
	
	SceneSizer2->Add( TexRotateSizer, 1, wxEXPAND, 5 );

	SceneSizer->Add( SceneSizer2, 1, wxEXPAND, 5 );

	TabScene->SetSizer( SceneSizer );
	TabScene->Layout();
	SceneSizer->Fit( TabScene );
	Tabs->AddPage( TabScene, wxT("Scene"), false );

	///////////////////////////////////////////////////////
	// MODELS TAB
	TabModels = new wxPanel( Tabs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* ModelsSizer;
	ModelsSizer = new wxBoxSizer( wxHORIZONTAL );

	ModelsList = new wxListBox( TabModels, wxID_ANY, wxDefaultPosition, wxSize( 200,200 ), 0, NULL, wxLB_ALWAYS_SB );
	ModelsSizer->Add( ModelsList, 0, wxALL, 5 );

	wxFlexGridSizer* ModelsSizer2;
	ModelsSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	ModelsSizer2->SetFlexibleDirection( wxBOTH );
	ModelsSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	ButtonOpenMesh = new wxButton( TabModels, wxID_ANY, wxT("Open..."), wxDefaultPosition, wxDefaultSize, 0 );
	ModelsSizer2->Add( ButtonOpenMesh, 0, wxALL, 5 );

	ButtonImportMesh = new wxButton( TabModels, wxID_ANY, wxT("Import..."), wxDefaultPosition, wxDefaultSize, 0 );
	ModelsSizer2->Add( ButtonImportMesh, 0, wxALL, 5 );

	ButtonPlace = new wxButton( TabModels, wxID_ANY, wxT("Place"), wxDefaultPosition, wxDefaultSize, 0 );
	ModelsSizer2->Add( ButtonPlace, 0, wxALL, 5 );

	ModelsSizer->Add( ModelsSizer2, 1, wxEXPAND, 5 );

	TabModels->SetSizer( ModelsSizer );
	TabModels->Layout();
	ModelsSizer->Fit( TabModels );
	Tabs->AddPage( TabModels, wxT("Meshes"), false );

	///////////////////////////////////////////////////////
	// MATERIALS TAB
	TabMaterials = new wxPanel( Tabs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* MaterialsSizer;
	MaterialsSizer = new wxBoxSizer( wxHORIZONTAL );

	MaterialsList = new wxListBox( TabMaterials, wxID_ANY, wxDefaultPosition, wxSize( 200,200 ), 0, NULL, wxLB_ALWAYS_SB );
	MaterialsSizer->Add( MaterialsList, 0, wxALL, 5 );

	wxFlexGridSizer* MaterialsSizer2;
	MaterialsSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	MaterialsSizer2->SetFlexibleDirection( wxBOTH );
	MaterialsSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	ButtonOpenMat = new wxButton( TabMaterials, wxID_ANY, wxT("Open..."), wxDefaultPosition, wxDefaultSize, 0 );
	MaterialsSizer2->Add( ButtonOpenMat, 0, wxALL, 5 );

	ButtonImportMat = new wxButton( TabMaterials, wxID_ANY, wxT("Import..."), wxDefaultPosition, wxDefaultSize, 0 );
	MaterialsSizer2->Add( ButtonImportMat, 0, wxALL, 5 );

	ButtonNewMat = new wxButton( TabMaterials, wxID_ANY, wxT("New..."), wxDefaultPosition, wxDefaultSize, 0 );
	MaterialsSizer2->Add( ButtonNewMat, 0, wxALL, 5 );

	ButtonEditMat = new wxButton( TabMaterials, wxID_ANY, wxT("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
	MaterialsSizer2->Add( ButtonEditMat, 0, wxALL, 5 );

	ButtonApplyMat = new wxButton( TabMaterials, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	MaterialsSizer2->Add( ButtonApplyMat, 0, wxALL, 5 );

	MaterialsSizer->Add( MaterialsSizer2, 1, wxEXPAND, 5 );

	TabMaterials->SetSizer( MaterialsSizer );
	TabMaterials->Layout();
	MaterialsSizer->Fit( TabMaterials );
	Tabs->AddPage( TabMaterials, wxT("Materials"), true );

	///////////////////////////////////////////////////////
	// TERRAIN TAB
	TabTerrain = new wxPanel( Tabs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );

	wxBoxSizer* TerrainSizer;
	TerrainSizer = new wxBoxSizer( wxHORIZONTAL );
	
	TerrainList = new wxListBox( TabTerrain, wxID_ANY, wxDefaultPosition, wxSize( 200,200 ), 0, NULL, wxLB_ALWAYS_SB ); 
	TerrainSizer->Add( TerrainList, 0, wxALL, 5 );
	
	wxFlexGridSizer* TerrainSizer2;
	TerrainSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	TerrainSizer2->SetFlexibleDirection( wxBOTH );
	TerrainSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	TerrOpen = new wxButton( TabTerrain, wxID_ANY, wxT("Load Heightmap"), wxDefaultPosition, wxDefaultSize, 0 );
	TerrainSizer2->Add( TerrOpen, 0, wxALL, 5 );
	
	wxBoxSizer* DummySizer;
	DummySizer = new wxBoxSizer( wxVERTICAL );
	
	TerrainSizer2->Add( DummySizer, 1, wxEXPAND, 5 );
	
	TxtTerrScale = new wxStaticText( TabTerrain, wxID_ANY, wxT("Scale:"), wxDefaultPosition, wxDefaultSize, 0 );
	TxtTerrScale->Wrap( -1 );
	TerrainSizer2->Add( TxtTerrScale, 0, wxALL, 5 );
	
	wxBoxSizer* TerrainSizer3;
	TerrainSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	TerrScaleX = new wxSpinCtrl( TabTerrain, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100, 1 );
	TerrainSizer3->Add( TerrScaleX, 1, wxALL|wxEXPAND, 5 );
	
	TerrScaleY = new wxSpinCtrl( TabTerrain, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100, 1 );
	TerrainSizer3->Add( TerrScaleY, 1, wxALL|wxEXPAND, 5 );
	
	TerrScaleZ = new wxSpinCtrl( TabTerrain, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100, 1 );
	TerrainSizer3->Add( TerrScaleZ, 1, wxALL|wxEXPAND, 5 );
	
	TerrainSizer2->Add( TerrainSizer3, 1, wxEXPAND, 5 );

	ButtonTerrApply = new wxButton( TabTerrain, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	TerrainSizer2->Add( ButtonTerrApply, 0, wxALL, 5 );
	
	TerrainSizer->Add( TerrainSizer2, 1, wxEXPAND, 5 );
	
	TabTerrain->SetSizer( TerrainSizer );
	TabTerrain->Layout();
	TerrainSizer->Fit( TabTerrain );

	Tabs->AddPage( TabTerrain, wxT("Terrain"), false );

	///////////////////////////////////////////////////////
	// SKYBOX TAB
	TabSkybox = new wxPanel( Tabs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );

	wxFlexGridSizer* SkyboxSizer;
	SkyboxSizer = new wxFlexGridSizer( 2, 3, 0, 0 );
	SkyboxSizer->SetFlexibleDirection( wxBOTH );
	SkyboxSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	TxtSkyNorth = new wxStaticText( TabSkybox, wxID_ANY, wxT("North:"), wxDefaultPosition, wxDefaultSize, 0 );
	TxtSkyNorth->Wrap( -1 );
	SkyboxSizer->Add( TxtSkyNorth, 0, wxALL, 5 );
	
	InputSkyNorth = new wxTextCtrl( TabSkybox, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	SkyboxSizer->Add( InputSkyNorth, 0, wxALL, 5 );
	
	ButtonSkyNorth = new wxButton( TabSkybox, wxID_ANY, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	SkyboxSizer->Add( ButtonSkyNorth, 0, wxALL, 5 );
	
	TxtSkySouth = new wxStaticText( TabSkybox, wxID_ANY, wxT("South:"), wxDefaultPosition, wxDefaultSize, 0 );
	TxtSkySouth->Wrap( -1 );
	SkyboxSizer->Add( TxtSkySouth, 0, wxALL, 5 );
	
	InputSkySouth = new wxTextCtrl( TabSkybox, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	SkyboxSizer->Add( InputSkySouth, 0, wxALL, 5 );
	
	ButtonSkySouth = new wxButton( TabSkybox, wxID_ANY, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	SkyboxSizer->Add( ButtonSkySouth, 0, wxALL, 5 );
	
	TxtSkyEast = new wxStaticText( TabSkybox, wxID_ANY, wxT("East:"), wxDefaultPosition, wxDefaultSize, 0 );
	TxtSkyEast->Wrap( -1 );
	SkyboxSizer->Add( TxtSkyEast, 0, wxALL, 5 );
	
	InputSkyEast = new wxTextCtrl( TabSkybox, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	SkyboxSizer->Add( InputSkyEast, 0, wxALL, 5 );
	
	ButtonSkyEast = new wxButton( TabSkybox, wxID_ANY, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	SkyboxSizer->Add( ButtonSkyEast, 0, wxALL, 5 );
	
	TxtSkyWest = new wxStaticText( TabSkybox, wxID_ANY, wxT("West:"), wxDefaultPosition, wxDefaultSize, 0 );
	TxtSkyWest->Wrap( -1 );
	SkyboxSizer->Add( TxtSkyWest, 0, wxALL, 5 );
	
	InputSkyWest = new wxTextCtrl( TabSkybox, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	SkyboxSizer->Add( InputSkyWest, 0, wxALL, 5 );
	
	ButtonSkyWest = new wxButton( TabSkybox, wxID_ANY, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	SkyboxSizer->Add( ButtonSkyWest, 0, wxALL, 5 );
	
	TxtSkyTop = new wxStaticText( TabSkybox, wxID_ANY, wxT("Top:"), wxDefaultPosition, wxDefaultSize, 0 );
	TxtSkyTop->Wrap( -1 );
	SkyboxSizer->Add( TxtSkyTop, 0, wxALL, 5 );
	
	InputSkyTop = new wxTextCtrl( TabSkybox, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	SkyboxSizer->Add( InputSkyTop, 0, wxALL, 5 );
	
	ButtonSkyTop = new wxButton( TabSkybox, wxID_ANY, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	SkyboxSizer->Add( ButtonSkyTop, 0, wxALL, 5 );
	
	TxtSkyBottom = new wxStaticText( TabSkybox, wxID_ANY, wxT("Bottom:"), wxDefaultPosition, wxDefaultSize, 0 );
	TxtSkyBottom->Wrap( -1 );
	SkyboxSizer->Add( TxtSkyBottom, 0, wxALL, 5 );
	
	InputSkyBottom = new wxTextCtrl( TabSkybox, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	SkyboxSizer->Add( InputSkyBottom, 0, wxALL, 5 );
	
	ButtonSkyBottom = new wxButton( TabSkybox, wxID_ANY, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	SkyboxSizer->Add( ButtonSkyBottom, 0, wxALL, 5 );
	
	ButtonSkyApply = new wxButton( TabSkybox, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	SkyboxSizer->Add( ButtonSkyApply, 0, wxALL, 5 );
	
	TabSkybox->SetSizer( SkyboxSizer );
	TabSkybox->Layout();
	SkyboxSizer->Fit( TabSkybox );

	Tabs->AddPage( TabSkybox, wxT("Skybox"), false );

	Sizer->Add( Tabs, 1, wxEXPAND | wxALL, 5 );

	this->SetSizer( Sizer );
	this->Layout();
}

AssetsFrame::~AssetsFrame()
{

}
