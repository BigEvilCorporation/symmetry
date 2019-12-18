#include <sstream>
#include <map>

#include <wx/splash.h>
#include "AppMain.h"
#include "Scene.h"

#include "../../symmetry/SYM_Vector.h"

int GridResolutions[9] = { 1, 2, 4, 8, 16, 32, 64, 128, 256 };
int ScaleTypes[4] = { GIZMO_SCALE_U, GIZMO_SCALE_X, GIZMO_SCALE_Y, GIZMO_SCALE_Z };
int CurrentScaleType = GIZMO_SCALE_U;

CPhysicsTimer::CPhysicsTimer(wxFrame *Owner) : wxTimer()
{
	OwnerFrame = Owner;
}

void CPhysicsTimer::Notify()
{
	sym::Engine->UpdateGlobalTimer();
	sym::Engine->Physics.Update();
	OwnerFrame->Update();
}

bool SymcraftApp::InitSymEngine()
{
	//Set engine media paths
	sym::Engine->SystemPath = "..//system//";
	sym::Engine->ScenePath = "..//scenes//";
	sym::Engine->ScriptsPath = "..//scripts//";
	sym::Engine->ModelsPath = "..//models//";
	sym::Engine->MaterialsPath = "..//materials//";
	sym::Engine->TexturesPath = "..//textures//";
	sym::Engine->HeightmapsPath = "..//textures//heightmaps//";
	sym::Engine->FontsPath = "..//fonts//";
	sym::Engine->VideosPath = "..//videos//";
	sym::Engine->ShadersPath = "..//shaders//";
	sym::Engine->AnimationsPath = "..//animations//";
	sym::Engine->AudioPath = "..//audio//";

    if(!sym::Engine->Logger.Init("symcraft.log"))
    {
        wxMessageBox(_T("Error: Could not initialise logger"));
        return false;
    }

	sym::Engine->Logger.AddEntry("Initialising SDL");
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        wxMessageBox(_T("Error: Could not initialise SDL"));
        return false;
    }

	sym::Engine->Logger.AddEntry("Initialising OpenGL extensions");
	sym::SDL_GL_InitGLExt();

	sym::Engine->Logger.AddEntry("Initialising TTF");
    if (TTF_Init() == 1)
    {
        wxMessageBox(_T("Error: Could not initialise SDL_TTF"));
        return false;
    }

	sym::Engine->Logger.AddEntry("Initialising Shaders");
	if (!sym::Engine->ShaderManager.Init())
    {
        wxMessageBox(_T("Error: Could not initialise Cg"));
        return false;
    }

	sym::Engine->Logger.AddEntry("Initialising Physics");
    if(!sym::Engine->Physics.Init())
    {
        wxMessageBox(_T("Error: Could not initialise Newton"));
        return false;
    }

    return true;
}

IMPLEMENT_APP(SymcraftApp)

bool SymcraftApp::OnInit()
{
	sym::Engine->Logger.AddEntry("Starting splash");
	wxBitmap bitmap;
	/*
	if (bitmap.LoadFile(_T("../editor//splash.bmp"), wxBITMAP_TYPE_ANY))
	{
		wxSplashScreen* splash = new wxSplashScreen(bitmap, wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT, 5000, NULL, -1, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER|wxSTAY_ON_TOP);
	}
	*/

    //Init engine
	sym::Engine->Logger.AddEntry("Starting engine");
    if(!InitSymEngine()) return false;

    //Create main frame
	sym::Engine->Logger.AddEntry("Creating main frame");
    Frame = new MainFrame(0);

	sym::Engine->Logger.AddEntry("Showing frame");
    Frame->Show(true);

	sym::Engine->Logger.AddEntry("Maximizing frame");
	Frame->Maximize(true);

    return true;
}

MainFrame::MainFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	//Initialise values
	CurrentGridRes = 4;
	PhysicsRunning = false;

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	StatusBar = this->CreateStatusBar( 3, wxST_SIZEGRIP, wxID_ANY );
	Menu = new wxMenuBar( 0 );
	MenuFile = new wxMenu();
	wxMenuItem* MenuFileNew;
	MenuFileNew = new wxMenuItem( MenuFile, wxID_ANY, wxString( wxT("New") ) , wxEmptyString, wxITEM_NORMAL );
	MenuFile->Append( MenuFileNew );
	
	wxMenuItem* MenuFileOpen;
	MenuFileOpen = new wxMenuItem( MenuFile, wxID_ANY, wxString( wxT("Open...") ) + wxT('\t') + wxT("CTRL+O"), wxEmptyString, wxITEM_NORMAL );
	MenuFile->Append( MenuFileOpen );
	
	wxMenuItem* MenuFileSave;
	MenuFileSave = new wxMenuItem( MenuFile, wxID_ANY, wxString( wxT("Save") ) + wxT('\t') + wxT("CTRL+S"), wxEmptyString, wxITEM_NORMAL );
	MenuFile->Append( MenuFileSave );
	
	wxMenuItem* MenuFileSaveAs;
	MenuFileSaveAs = new wxMenuItem( MenuFile, wxID_ANY, wxString( wxT("Save as...") ) , wxEmptyString, wxITEM_NORMAL );
	MenuFile->Append( MenuFileSaveAs );
	
	MenuFile->AppendSeparator();
	
	wxMenuItem* MenuFileExit;
	MenuFileExit = new wxMenuItem( MenuFile, wxID_ANY, wxString( wxT("Exit") ) , wxEmptyString, wxITEM_NORMAL );
	MenuFile->Append( MenuFileExit );
	
	Menu->Append( MenuFile, wxT("&File") );

	MenuEdit = new wxMenu();
	wxMenuItem* MenuEditUndo;
	wxMenuItem* MenuEditCopy;
	MenuEditUndo = new wxMenuItem( MenuEdit, wxID_ANY, wxString( wxT("Undo") ) + wxT('\t') + wxT("CTRL+Z") , wxEmptyString, wxITEM_NORMAL );
	MenuEditCopy = new wxMenuItem( MenuEdit, wxID_ANY, wxString( wxT("Copy") ) + wxT('\t') + wxT("CTRL+C") , wxEmptyString, wxITEM_NORMAL );
	MenuEdit->Append( MenuEditUndo );
	MenuEdit->Append( MenuEditCopy );
	Menu->Append( MenuEdit, wxT("&Edit") );
	
	this->SetMenuBar( Menu );

	ToolIDAddPlane = wxNewId();
	ToolIDAddBox = wxNewId();
	ToolIDAddCylinder = wxNewId();
	ToolIDAddSphere = wxNewId();
	ToolIDAddCone = wxNewId();

	ToolIDSelect = wxNewId();
	ToolIDMove = wxNewId();
	ToolIDRotate = wxNewId();
	ToolIDScale = wxNewId();
	ToolIDClone = wxNewId();
	ToolIDUndo = wxNewId();
	ToolIDDelete = wxNewId();

	ToolIDMesh = wxNewId();
	ToolIDTerrain = wxNewId();
	ToolIDWater = wxNewId();
	ToolIDRope = wxNewId();

	int ToolIDRunStopPhysics = wxNewId();
	int ToolIDResetPhysics = wxNewId();

	ToolIDFullscreenTL = wxNewId();
	ToolIDFullscreenTR = wxNewId();
	ToolIDFullscreenBL = wxNewId();
	ToolIDFullscreenBR = wxNewId();
	ToolIDResetTL = wxNewId();
	ToolIDResetTR = wxNewId();
	ToolIDResetBL = wxNewId();
	ToolIDResetBR = wxNewId();
	
	ToolbarMain = this->CreateToolBar( wxTB_HORIZONTAL, wxID_ANY ); 
	ToolbarMain->AddTool( ToolIDAddPlane, wxT("Create Plane"), wxBitmap( wxT("../editor/icon_plane.bmp"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Create Plane"), wxT("Create Plane") );
	ToolbarMain->AddTool( ToolIDAddBox, wxT("Create Box"), wxBitmap( wxT("../editor/icon_cube.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxT("Create Box"), wxT("Create Box") );
	ToolbarMain->AddTool( ToolIDAddCylinder, wxT("Create Cylinder"), wxBitmap( wxT("../editor/icon_cylinder.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxT("Create Cylinder"), wxT("Create Cylinder") );
	ToolbarMain->AddTool( ToolIDAddSphere, wxT("Create Sphere"), wxBitmap( wxT("../editor/icon_sphere.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxT("Create Sphere"), wxT("Create Sphere") );
	ToolbarMain->AddTool( ToolIDAddCone, wxT("Create Cone"), wxBitmap( wxT("../editor/icon_cone.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxT("Create Cone"), wxT("Create Cone") );
	ToolbarMain->AddSeparator();
	ToolbarMain->AddTool( ToolIDMesh, wxT("Add mesh"), wxBitmap( wxT("../editor/icon_plane.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxT("Add mesh"), wxT("Add a .sym.mesh from file") );
	ToolbarMain->AddTool( ToolIDTerrain, wxT("Add terrain"), wxBitmap( wxT("../editor/icon_plane.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxT("Add terrain"), wxT("Load a terrain from a .PNG heightmap") );
	ToolbarMain->AddTool( ToolIDWater, wxT("Add water volume"), wxBitmap( wxT("../editor/icon_plane.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxT("Add water volume"), wxT("Add water (buoyancy) volume") );
	ToolbarMain->AddTool( ToolIDRope, wxT("Add rope"), wxBitmap( wxT("../editor/icon_plane.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxT("Add rope"), wxT("Add rope") );
	ToolbarMain->AddSeparator();
	ToolbarMain->AddTool( ToolIDSelect, wxT("Select"), wxBitmap( wxT("../editor/icon_select.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_RADIO, wxT("Select"), wxT("Selects an object") );
	ToolbarMain->AddTool( ToolIDMove, wxT("Move"), wxBitmap( wxT("../editor/icon_move.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_RADIO, wxT("Move"), wxT("Moves an object") );
	ToolbarMain->AddTool( ToolIDRotate, wxT("Rotate"), wxBitmap( wxT("../editor/icon_rotate.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_RADIO, wxT("Rotate"), wxT("Rotates an object") );
	ToolbarMain->AddTool( ToolIDScale, wxT("Scale"), wxBitmap( wxT("../editor/icon_scale.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_RADIO, wxT("Scale"), wxT("Scales an object") );
	ToolbarMain->AddTool( ToolIDClone, wxT("Clone"), wxBitmap( wxT("../editor/icon_plane.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxT("Clone"), wxT("Copies an object") );
	ToolbarMain->AddTool( ToolIDUndo, wxT("Undo"), wxBitmap( wxT("../editor/icon_plane.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxT("Undo"), wxT("Undo last operation") );
	ToolbarMain->AddTool( ToolIDDelete, wxT("Delete"), wxBitmap( wxT("../editor/icon_delete.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxT("Delete"), wxT("Deletes an object") );
	ToolbarMain->AddSeparator();
	TextScale = new wxStaticText( ToolbarMain, wxID_ANY, wxT("Scale:"), wxDefaultPosition, wxDefaultSize, 0 );
	TextScale->Wrap( -1 );
	ToolbarMain->AddControl( TextScale );
	wxString DropScaleChoices[] = { wxT("Uniform"), wxT("X Axis"), wxT("Y Axis"), wxT("Z Axis") };
	int DropScaleNChoices = sizeof( DropScaleChoices ) / sizeof( wxString );
	DropScale = new wxChoice( ToolbarMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, DropScaleNChoices, DropScaleChoices, 0 );
	DropScale->SetSelection( 0 );
	ToolbarMain->AddControl( DropScale );
	ToolbarMain->AddSeparator();
	TextGrid = new wxStaticText( ToolbarMain, wxID_ANY, wxT("Grid snap:"), wxDefaultPosition, wxDefaultSize, 0 );
	TextGrid->Wrap( -1 );
	ToolbarMain->AddControl( TextGrid );
	wxString DropGridChoices[] = { wxT("1"), wxT("2"), wxT("4"), wxT("8"), wxT("16"), wxT("32"), wxT("64"), wxT("128"), wxT("256") };
	int DropGridNChoices = sizeof( DropGridChoices ) / sizeof( wxString );
	DropGrid = new wxChoice( ToolbarMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, DropGridNChoices, DropGridChoices, 0 );
	DropGrid->SetSelection( 2 );
	ToolbarMain->AddControl( DropGrid );
	ToolbarMain->AddSeparator();
	TextRotate = new wxStaticText( ToolbarMain, wxID_ANY, wxT("Rotate snap:"), wxDefaultPosition, wxDefaultSize, 0 );
	TextRotate->Wrap( -1 );
	ToolbarMain->AddControl( TextRotate );
	SpinRotate = new wxSpinCtrl( ToolbarMain, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 50,-1 ), wxSP_ARROW_KEYS, 1, 360, 5 );
	ToolbarMain->AddControl( SpinRotate );
	ToolbarMain->AddSeparator();
	ToolbarMain->AddTool( ToolIDRunStopPhysics, wxT("Run/Stop Physics Simulation"), wxBitmap( wxT("../editor/icon_plane.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_CHECK, wxT("Run/Stop Physics Simulation"), wxT("Run/Stop Physics Simulation") );
	ToolbarMain->AddTool( ToolIDResetPhysics, wxT("Reset Physics Simulation"), wxBitmap( wxT("../editor/icon_plane.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxT("Reset Physics Simulation"), wxT("Reset Physics Simulation") );
	ToolbarMain->Realize();
	
	wxFlexGridSizer* SizerMain;
	SizerMain = new wxFlexGridSizer( 1, 2, 0, 0 );
	SizerMain->AddGrowableCol( 0 );
	SizerMain->AddGrowableRow( 0 );
	SizerMain->SetFlexibleDirection( wxBOTH );
	SizerMain->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxGridSizer* SizerViewports;
	SizerViewports = new wxGridSizer( 2, 2, 0, 0 );
	
	wxFlexGridSizer* SizerTopLeft;
	SizerTopLeft = new wxFlexGridSizer( 2, 1, 0, 0 );
	SizerTopLeft->AddGrowableCol( 0 );
	SizerTopLeft->AddGrowableRow( 0 );
	SizerTopLeft->SetFlexibleDirection( wxBOTH );
	SizerTopLeft->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	//Create first OGL canvas
	sym::Engine->Logger.AddEntry("Creating top left viewport");
	GLTopLeft = new GLViewport(this, VIEW_FRONT);

	SizerTopLeft->Add( GLTopLeft, 1, wxEXPAND | wxALL, 1 );

	//Grab context
	sym::Engine->Logger.AddEntry("Grabbing OpenGL context");
	GLContext = GLTopLeft->GetContext();
	
	ToolbarTopLeft = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL ); 
	wxString DropViewTopLeftChoices[] = { wxT("Top"), wxT("Front"), wxT("Left"), wxT("Perspective") };
	int DropViewTopLeftNChoices = sizeof( DropViewTopLeftChoices ) / sizeof( wxString );
	DropViewTopLeft = new wxChoice( ToolbarTopLeft, wxID_ANY, wxDefaultPosition, wxDefaultSize, DropViewTopLeftNChoices, DropViewTopLeftChoices, 0 );
	DropViewTopLeft->SetSelection( 1 );
	ToolbarTopLeft->AddControl( DropViewTopLeft );
	ToolbarTopLeft->AddSeparator();
	ToolbarTopLeft->AddTool( ToolIDFullscreenTL, wxT("Fullscreen"), wxBitmap( wxT("../editor/icon_plane.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxT("Fullscreen"), wxT("Fullscreen") );
	ToolbarTopLeft->AddTool( ToolIDResetTL, wxT("Reset View"), wxBitmap( wxT("../editor/icon_plane.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxT("Reset View"), wxT("Reset View") );
	ToolbarTopLeft->AddSeparator();
	RadioViewTopLeftSolid = new wxRadioButton( ToolbarTopLeft, wxID_ANY, wxT("Solid"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	ToolbarTopLeft->AddControl( RadioViewTopLeftSolid );
	RadioViewTopLeftWireframe = new wxRadioButton( ToolbarTopLeft, wxID_ANY, wxT("Wireframe"), wxDefaultPosition, wxDefaultSize, 0 );
	RadioViewTopLeftWireframe->SetValue( true ); 
	ToolbarTopLeft->AddControl( RadioViewTopLeftWireframe );
	ToolbarTopLeft->Realize();
	
	SizerTopLeft->Add( ToolbarTopLeft, 0, wxEXPAND, 5 );
	
	SizerViewports->Add( SizerTopLeft, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* SizerTopRight;
	SizerTopRight = new wxFlexGridSizer( 2, 1, 0, 0 );
	SizerTopRight->AddGrowableCol( 0 );
	SizerTopRight->AddGrowableRow( 0 );
	SizerTopRight->SetFlexibleDirection( wxBOTH );
	SizerTopRight->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	GLTopRight = new GLViewport(this, GLContext, VIEW_TOP);
	SizerTopRight->Add( GLTopRight, 1, wxEXPAND | wxALL, 1 );
	
	ToolbarTopRight = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL ); 
	wxString DropViewTopRightChoices[] = { wxT("Top"), wxT("Front"), wxT("Left"), wxT("Perspective") };
	int DropViewTopRightNChoices = sizeof( DropViewTopRightChoices ) / sizeof( wxString );
	DropViewTopRight = new wxChoice( ToolbarTopRight, wxID_ANY, wxDefaultPosition, wxDefaultSize, DropViewTopRightNChoices, DropViewTopRightChoices, 0 );
	DropViewTopRight->SetSelection( 0 );
	ToolbarTopRight->AddControl( DropViewTopRight );
	ToolbarTopRight->AddSeparator();
	ToolbarTopRight->AddTool( ToolIDFullscreenTR, wxT("Fullscreen"), wxBitmap( wxT("../editor/icon_plane.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxT("Fullscreen"), wxT("Fullscreen") );
	ToolbarTopRight->AddTool( ToolIDResetTR, wxT("Reset View"), wxBitmap( wxT("../editor/icon_plane.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxT("Reset View"), wxT("Reset View") );
	ToolbarTopRight->AddSeparator();
	RadioViewTopRightSolid = new wxRadioButton( ToolbarTopRight, wxID_ANY, wxT("Solid"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	ToolbarTopRight->AddControl( RadioViewTopRightSolid );
	RadioViewTopRightWireframe = new wxRadioButton( ToolbarTopRight, wxID_ANY, wxT("Wireframe"), wxDefaultPosition, wxDefaultSize, 0 );
	RadioViewTopRightWireframe->SetValue( true ); 
	ToolbarTopRight->AddControl( RadioViewTopRightWireframe );
	ToolbarTopRight->Realize();
	
	SizerTopRight->Add( ToolbarTopRight, 0, wxEXPAND, 5 );
	
	SizerViewports->Add( SizerTopRight, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* SizerBottomLeft;
	SizerBottomLeft = new wxFlexGridSizer( 2, 1, 0, 0 );
	SizerBottomLeft->AddGrowableCol( 0 );
	SizerBottomLeft->AddGrowableRow( 0 );
	SizerBottomLeft->SetFlexibleDirection( wxBOTH );
	SizerBottomLeft->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	GLBottomLeft = new GLViewport(this, GLContext, VIEW_LEFT);
	SizerBottomLeft->Add( GLBottomLeft, 1, wxEXPAND | wxALL, 1 );
	
	ToolbarBottomLeft = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL ); 
	wxString DropViewBottomLeftChoices[] = { wxT("Top"), wxT("Front"), wxT("Left"), wxT("Perspective") };
	int DropViewBottomLeftNChoices = sizeof( DropViewBottomLeftChoices ) / sizeof( wxString );
	DropViewBottomLeft = new wxChoice( ToolbarBottomLeft, wxID_ANY, wxDefaultPosition, wxDefaultSize, DropViewBottomLeftNChoices, DropViewBottomLeftChoices, 0 );
	DropViewBottomLeft->SetSelection( 2 );
	ToolbarBottomLeft->AddControl( DropViewBottomLeft );
	ToolbarBottomLeft->AddSeparator();
	ToolbarBottomLeft->AddTool( ToolIDFullscreenBL, wxT("Fullscreen"), wxBitmap( wxT("../editor/icon_plane.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxT("Fullscreen"), wxT("Fullscreen") );
	ToolbarBottomLeft->AddTool( ToolIDResetBL, wxT("Reset View"), wxBitmap( wxT("../editor/icon_plane.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxT("Reset View"), wxT("Reset View") );
	ToolbarBottomLeft->AddSeparator();
	RadioViewBottomLeftSolid = new wxRadioButton( ToolbarBottomLeft, wxID_ANY, wxT("Solid"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	ToolbarBottomLeft->AddControl( RadioViewBottomLeftSolid );
	RadioViewBottomLeftWireframe = new wxRadioButton( ToolbarBottomLeft, wxID_ANY, wxT("Wireframe"), wxDefaultPosition, wxDefaultSize, 0 );
	RadioViewBottomLeftWireframe->SetValue( true ); 
	ToolbarBottomLeft->AddControl( RadioViewBottomLeftWireframe );
	ToolbarBottomLeft->Realize();
	
	SizerBottomLeft->Add( ToolbarBottomLeft, 0, wxEXPAND, 5 );
	
	SizerViewports->Add( SizerBottomLeft, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* SizerBottomRight;
	SizerBottomRight = new wxFlexGridSizer( 2, 1, 0, 0 );
	SizerBottomRight->AddGrowableCol( 0 );
	SizerBottomRight->AddGrowableRow( 0 );
	SizerBottomRight->SetFlexibleDirection( wxBOTH );
	SizerBottomRight->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	GLBottomRight = new GLViewport(this, GLContext, VIEW_PERSPECTIVE);
	SizerBottomRight->Add( GLBottomRight, 1, wxEXPAND | wxALL, 1 );
	
	ToolbarBottomRight = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL ); 
	wxString DropViewBottomRightChoices[] = { wxT("Top"), wxT("Front"), wxT("Left"), wxT("Perspective") };
	int DropViewBottomRightNChoices = sizeof( DropViewBottomRightChoices ) / sizeof( wxString );
	DropViewBottomRight = new wxChoice( ToolbarBottomRight, wxID_ANY, wxDefaultPosition, wxDefaultSize, DropViewBottomRightNChoices, DropViewBottomRightChoices, 0 );
	DropViewBottomRight->SetSelection( 3 );
	ToolbarBottomRight->AddControl( DropViewBottomRight );
	ToolbarBottomRight->AddSeparator();
	ToolbarBottomRight->AddTool( ToolIDFullscreenBR, wxT("Fullscreen"), wxBitmap( wxT("../editor/icon_plane.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxT("Fullscreen"), wxT("Fullscreen") );
	ToolbarBottomRight->AddTool( ToolIDResetBR, wxT("Reset View"), wxBitmap( wxT("../editor/icon_plane.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxT("Reset View"), wxT("Reset View") );
	ToolbarBottomRight->AddSeparator();
	RadioViewBottomRightSolid = new wxRadioButton( ToolbarBottomRight, wxID_ANY, wxT("Solid"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	RadioViewBottomRightSolid->SetValue( true ); 
	ToolbarBottomRight->AddControl( RadioViewBottomRightSolid );
	RadioViewBottomRightWireframe = new wxRadioButton( ToolbarBottomRight, wxID_ANY, wxT("Wireframe"), wxDefaultPosition, wxDefaultSize, 0 );
	ToolbarBottomRight->AddControl( RadioViewBottomRightWireframe );
	ToolbarBottomRight->Realize();
	
	SizerBottomRight->Add( ToolbarBottomRight, 0, wxEXPAND, 5 );
	
	SizerViewports->Add( SizerBottomRight, 1, wxEXPAND, 5 );
	
	SizerMain->Add( SizerViewports, 1, wxEXPAND, 1 );

	this->SetSizer( SizerMain );

	GLTopLeft->Grid.SetResolution(4);
	GLTopRight->Grid.SetResolution(4);
	GLBottomLeft->Grid.SetResolution(4);
	GLBottomRight->Grid.SetResolution(4);

	GLTopLeft->SetRenderStyle(1);
	GLTopRight->SetRenderStyle(1);
	GLBottomLeft->SetRenderStyle(1);
	GLBottomRight->SetRenderStyle(0);

	GLTopLeft->CurrentGizmo.SetMode(SELECT_MODE);
	GLTopRight->CurrentGizmo.SetMode(SELECT_MODE);
	GLBottomLeft->CurrentGizmo.SetMode(SELECT_MODE);
	GLBottomRight->CurrentGizmo.SetMode(SELECT_MODE);

	//Create assets tool window
	//SYM::Engine->Logger.AddEntry("Creating assets frame");
	Assets = new AssetsFrame(this);
	Assets->Show(true);

	this->Layout();

	// Connect Events
	//this->Connect( TimerID, wxEVT_TIMER, wxTimerEventHandler( MainFrame::OnPhysicsTimer ) );
	this->Connect( wxID_ANY, wxEVT_ERASE_BACKGROUND, wxEraseEventHandler( MainFrame::OnClear ) );
	this->Connect( MenuFileNew->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnFileNew ) );
	this->Connect( MenuFileOpen->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnFileOpen ) );
	this->Connect( MenuFileSave->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnFileSave ) );
	this->Connect( MenuFileSaveAs->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnFileSaveAs ) );
	this->Connect( MenuFileExit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnFileExit ) );
	this->Connect( MenuEditUndo->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolUndo ) );
	this->Connect( MenuEditCopy->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolClone ) );
	this->Connect( ToolIDAddPlane, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolPlane ) );
	this->Connect( ToolIDAddBox, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolBox ) );
	this->Connect( ToolIDAddCylinder, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolCylinder ) );
	this->Connect( ToolIDAddSphere, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolSphere ) );
	this->Connect( ToolIDAddCone, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolCone ) );
	this->Connect( ToolIDMesh, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolMesh ) );
	this->Connect( ToolIDTerrain, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolTerrain ) );
	this->Connect( ToolIDWater, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolWater ) );
	this->Connect( ToolIDRope, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolRope ) );
	this->Connect( ToolIDRunStopPhysics, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolRunStopPhysics ) );
	this->Connect( ToolIDResetPhysics, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolResetPhysics ) );
	this->Connect( ToolIDSelect, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolSelect ) );
	this->Connect( ToolIDMove, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolMove ) );
	this->Connect( ToolIDRotate, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolRotate ) );
	this->Connect( ToolIDScale, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolScale ) );
	this->Connect( ToolIDClone, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolClone ) );
	this->Connect( ToolIDUndo, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolUndo ) );
	this->Connect( ToolIDDelete, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolDelete ) );
	DropScale->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MainFrame::OnDropScale ), NULL, this );
	DropGrid->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MainFrame::OnDropGrid ), NULL, this );
	SpinRotate->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( MainFrame::OnSpinRotate ), NULL, this );
	DropViewTopLeft->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MainFrame::OnDropViewTopLeft ), NULL, this );
	this->Connect( ToolIDFullscreenTL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolTopLeftFullscreen ) );
	this->Connect( ToolIDResetTL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolTopLeftReset ) );
	RadioViewTopLeftSolid->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MainFrame::OnRadioTopLeftSolid ), NULL, this );
	RadioViewTopLeftWireframe->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MainFrame::OnRadioTopLeftWireframe ), NULL, this );
	DropViewTopRight->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MainFrame::OnDropViewTopRight ), NULL, this );
	this->Connect( ToolIDFullscreenTR, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolTopRightFullscreen ) );
	this->Connect( ToolIDResetTR, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolTopRightReset ) );
	RadioViewTopRightSolid->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MainFrame::OnRadioTopRightSolid ), NULL, this );
	RadioViewTopRightWireframe->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MainFrame::OnRadioTopRightWireframe ), NULL, this );
	DropViewBottomLeft->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MainFrame::OnDropViewBottomLeft ), NULL, this );
	this->Connect( ToolIDFullscreenBL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolBottomLeftFullscreen ) );
	this->Connect( ToolIDResetBL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolBottomLeftReset ) );
	RadioViewBottomLeftSolid->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MainFrame::OnRadioBottomLeftSolid ), NULL, this );
	RadioViewBottomLeftWireframe->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MainFrame::OnRadioBottomLeftWireframe ), NULL, this );
	DropViewBottomRight->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MainFrame::OnDropViewBottomRight ), NULL, this );
	this->Connect( ToolIDFullscreenBR, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolBottomRightFullscreen ) );
	this->Connect( ToolIDResetBR, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolBottomRightReset ) );
	RadioViewBottomRightSolid->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MainFrame::OnRadioBottomRightSolid ), NULL, this );
	RadioViewBottomRightWireframe->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( MainFrame::OnRadioBottomRightWireframe ), NULL, this );

	//Assets dialog
	Assets->LstGeometry->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( MainFrame::OnSelect ), NULL, this );
	Assets->TxtName->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( MainFrame::OnNameChange ), NULL, this );
	Assets->BtnBrowseMaterial->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnMaterialBrowse ), NULL, this );
	Assets->DrpPhysics->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MainFrame::OnPhysicsChange ), NULL, this );
	Assets->SpnMass->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( MainFrame::OnMassChange ), NULL, this );
	Assets->SpnPositionX->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( MainFrame::OnPositionChange ), NULL, this );
	Assets->SpnPositionY->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( MainFrame::OnPositionChange ), NULL, this );
	Assets->SpnPositionZ->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( MainFrame::OnPositionChange ), NULL, this );
	Assets->SpnRotationX->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( MainFrame::OnRotationChange ), NULL, this );
	Assets->SpnRotationY->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( MainFrame::OnRotationChange ), NULL, this );
	Assets->SpnRotationZ->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( MainFrame::OnRotationChange ), NULL, this );
	Assets->SpnScaleX->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( MainFrame::OnScaleChange ), NULL, this );
	Assets->SpnScaleY->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( MainFrame::OnScaleChange ), NULL, this );
	Assets->SpnScaleZ->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( MainFrame::OnScaleChange ), NULL, this );
	Assets->SpnTileU->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( MainFrame::OnUVChange ), NULL, this );
	Assets->SpnTileV->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( MainFrame::OnUVChange ), NULL, this );
	Assets->SpnOffsetU->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( MainFrame::OnUVChange ), NULL, this );
	Assets->SpnOffsetV->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( MainFrame::OnUVChange ), NULL, this );

	Assets->ChkGrapple1->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MainFrame::OnChkGrapple1 ), NULL, this );
	Assets->ChkGrapple2->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MainFrame::OnChkGrapple2 ), NULL, this );
	Assets->ChkVision1->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MainFrame::OnChkVision1 ), NULL, this );
	Assets->ChkVision2->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MainFrame::OnChkVision2 ), NULL, this );
	Assets->ChkSpawn->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MainFrame::OnChkSpawn ), NULL, this );

	PhysicsTimer = new CPhysicsTimer(this);
	//PhysicsTimer->Start(5, wxTIMER_CONTINUOUS);
}

MainFrame::~MainFrame()
{

}

void MainFrame::OnClear(wxEraseEvent& evt)
{
	wxFrame::OnEraseBackground(evt);
	Update();
}

////////////////////////////////////////////////////////////////////////////////
// FILE MENU
void MainFrame::OnFileNew( wxCommandEvent& event )
{
	Scene.ClearScene();
	CurrentGeom = 0;
	CurrentFile = "";
	Update();
	Assets->LstGeometry->Clear();
}

void MainFrame::OnFileOpen( wxCommandEvent& event )
{
	wxFileDialog *OpenDialog= new wxFileDialog(this, _T("Open Symmetry scene file"), _(""), _(""), _("Symmetry scene files (*.sym.scene)|*.sym.scene"), wxOPEN);
	if ( OpenDialog->ShowModal() == wxID_OK )
	{
		Scene.ClearScene();
		CurrentGeom = 0;

		if(!Scene.Load(std::string(OpenDialog->GetFilename().mb_str())))
		{
		    wxMessageBox(_T("Error: Could not load file"));
			return;
		}
		else
		{
			CurrentFile = std::string(OpenDialog->GetFilename().mb_str());

			//Populate scene list
			for(std::map<std::string, sym::CGeometry*> ::iterator g = Scene.Geometry.begin(); g != Scene.Geometry.end(); g++)
			{
				//Add to scene list
				Assets->LstGeometry->Append(wxString((*g).first.c_str(), wxConvUTF8));
			}
		}
	}

	OpenDialog->Close();

	Update();
	UpdateAssetsFrame();
}

void MainFrame::OnFileSave( wxCommandEvent& event )
{
	if(CurrentFile.empty()) OnFileSaveAs(event);
	else
	{
		if(!Scene.Write(std::string(CurrentFile)))
		{
		    wxMessageBox(_T("Error: Could not write file"));
		}
	}
}

void MainFrame::OnFileSaveAs( wxCommandEvent& event )
{
	wxFileDialog *WriteDialog= new wxFileDialog(this, _T("Open Symmetry scene file"), _(""), _(""), _("Symmetry scene files (*.sym.scene)|*.sym.scene"), wxSAVE | wxOVERWRITE_PROMPT);
	if ( WriteDialog->ShowModal() == wxID_OK )
	{
		if(!Scene.Write(std::string(WriteDialog->GetFilename().mb_str())))
		{
		    wxMessageBox(_T("Error: Could not write file"));
		}
		else
		{
			CurrentFile = std::string(WriteDialog->GetFilename().mb_str());
		}
	}

	WriteDialog->Close();
}

void MainFrame::OnFileExit( wxCommandEvent& event )
{
	//sym::Engine->Physics.Shutdown();
	//sym::Engine->Logger.Close();

	Close(true);
}

////////////////////////////////////////////////////////////////////////////////
// CHANGE VIEWPORT DROPBOXES
void MainFrame::OnDropViewTopLeft( wxCommandEvent& event )
{
	GLTopLeft->SetViewType(DropViewTopLeft->GetCurrentSelection());
	Update();
}

void MainFrame::OnDropViewTopRight( wxCommandEvent& event )
{
	GLTopRight->SetViewType(DropViewTopRight->GetCurrentSelection());
	Update();
}

void MainFrame::OnDropViewBottomLeft( wxCommandEvent& event )
{
	GLBottomLeft->SetViewType(DropViewBottomLeft->GetCurrentSelection());
	Update();
}

void MainFrame::OnDropViewBottomRight( wxCommandEvent& event )
{
	GLBottomRight->SetViewType(DropViewBottomRight->GetCurrentSelection());
	Update();
}

////////////////////////////////////////////////////////////////////////////////
// VIEWPORT RESET BUTTONS
void MainFrame::OnToolTopLeftReset( wxCommandEvent& event )
{
	GLTopLeft->SetViewType(DropViewTopLeft->GetCurrentSelection());
	Update();
}

void MainFrame::OnToolTopRightReset( wxCommandEvent& event )
{
	GLTopRight->SetViewType(DropViewTopRight->GetCurrentSelection());
	Update();
}

void MainFrame::OnToolBottomLeftReset( wxCommandEvent& event )
{
	GLBottomLeft->SetViewType(DropViewBottomLeft->GetCurrentSelection());
	Update();
}

void MainFrame::OnToolBottomRightReset( wxCommandEvent& event )
{
	GLBottomRight->SetViewType(DropViewBottomRight->GetCurrentSelection());
	Update();
}

////////////////////////////////////////////////////////////////////////////////
// VIEWPORT DRAW MODE RADIO BUTTONS
void MainFrame::OnRadioTopLeftSolid( wxCommandEvent& event )
{
	GLTopLeft->SetRenderStyle(0);
	Update();
}

void MainFrame::OnRadioTopLeftWireframe( wxCommandEvent& event )
{
	GLTopLeft->SetRenderStyle(1);
	Update();
}

void MainFrame::OnRadioTopRightSolid( wxCommandEvent& event )
{
	GLTopRight->SetRenderStyle(0);
	Update();
}

void MainFrame::OnRadioTopRightWireframe( wxCommandEvent& event )
{
	GLTopRight->SetRenderStyle(1);
	Update();
}

void MainFrame::OnRadioBottomLeftSolid( wxCommandEvent& event )
{
	GLBottomLeft->SetRenderStyle(0);
	Update();
}

void MainFrame::OnRadioBottomLeftWireframe( wxCommandEvent& event )
{
	GLBottomLeft->SetRenderStyle(1);
	Update();
}

void MainFrame::OnRadioBottomRightSolid( wxCommandEvent& event )
{
	GLBottomRight->SetRenderStyle(0);
	Update();
}

void MainFrame::OnRadioBottomRightWireframe( wxCommandEvent& event )
{
	GLBottomRight->SetRenderStyle(1);
	Update();
}

////////////////////////////////////////////////////////////////////////////////
// GRID RESOLUTION DROPBOX
void MainFrame::OnDropGrid( wxCommandEvent& event )
{
	CurrentGridRes = GridResolutions[DropGrid->GetCurrentSelection()];
	GLTopLeft->Grid.SetResolution(CurrentGridRes);
	GLTopRight->Grid.SetResolution(CurrentGridRes);
	GLBottomLeft->Grid.SetResolution(CurrentGridRes);
	GLBottomRight->Grid.SetResolution(CurrentGridRes);
	TranslateStep = CurrentGridRes;
	Update();
}

////////////////////////////////////////////////////////////////////////////////
// SCALE TYPE DROPBOX
void MainFrame::OnDropScale( wxCommandEvent& event )
{
	CurrentScaleType = ScaleTypes[DropScale->GetCurrentSelection()];
	if(ToolbarMain->FindById(ToolIDScale)->IsToggled()) OnToolScale(event);
}

////////////////////////////////////////////////////////////////////////////////
// ROTATE SNAP SPINBOX
void MainFrame::OnSpinRotate( wxSpinEvent& event )
{
	RotateStep = SpinRotate->GetValue();
}

////////////////////////////////////////////////////////////////////////////////
// ADD BRUSH TOOLS
void MainFrame::OnToolBox( wxCommandEvent& event )
{
	CurrentGeom = Scene.AddBox("Geometry0", sym::LEVEL_COLLISION, sym::CVector3(32, 32, 32));
	Assets->LstGeometry->Append(wxString(CurrentGeom->Name.c_str(), wxConvUTF8));
	PushUndoOperation();
	Update();
	UpdateAssetsFrame();
}

void MainFrame::OnToolPlane( wxCommandEvent& event )
{
	CurrentGeom = Scene.AddPlane("Geometry0", sym::LEVEL_COLLISION, sym::CVector3(32, 1, 32));
	Assets->LstGeometry->Append(wxString(CurrentGeom->Name.c_str(), wxConvUTF8));
	PushUndoOperation();
	Update();
	UpdateAssetsFrame();
}

void MainFrame::OnToolMesh( wxCommandEvent& event )
{
	wxFileDialog *OpenDialog= new wxFileDialog(this, _T("Open Symmetry mesh file"), _(""), _(""), _("Symmetry mesh files (*.sym.mesh)|*.sym.mesh"), wxOPEN);
	if ( OpenDialog->ShowModal() == wxID_OK )
	{
		CurrentGeom = Scene.AddModel("Geometry0", std::string(OpenDialog->GetFilename().mb_str()), sym::PHYSICS_OBJ_BOX);

		if(!CurrentGeom)
		{
		    wxMessageBox(_T("Error: Could not load file"));
			return;
		}
	}

	OpenDialog->Close();

	Assets->LstGeometry->Append(wxString(CurrentGeom->Name.c_str(), wxConvUTF8));
	PushUndoOperation();
	Update();
	UpdateAssetsFrame();
}

////////////////////////////////////////////////////////////////////////////////
// PHYSICS TOOLS
void MainFrame::OnToolRunStopPhysics( wxCommandEvent& event )
{
	//if(PhysicsRunning) sym::Engine->Physics.Stop();
	//else
		sym::Engine->Physics.Run();
}

void MainFrame::OnToolResetPhysics( wxCommandEvent& event )
{

}

void MainFrame::OnPhysicsTimer(wxTimerEvent& WXUNUSED(event))
{
	sym::Engine->UpdateGlobalTimer();
	sym::Engine->Physics.Update();
	Update();
}

////////////////////////////////////////////////////////////////////////////////
// MODIFY TOOLS
void MainFrame::OnToolSelect( wxCommandEvent& event )
{
	GLTopLeft->CurrentGizmo.SetMode(SELECT_MODE);
	GLTopRight->CurrentGizmo.SetMode(SELECT_MODE);
	GLBottomLeft->CurrentGizmo.SetMode(SELECT_MODE);
	GLBottomRight->CurrentGizmo.SetMode(SELECT_MODE);
	Update();
}

void MainFrame::OnToolMove( wxCommandEvent& event )
{
	GLTopLeft->CurrentGizmo.SetMode(GIZMO_TRANSLATE);
	GLTopRight->CurrentGizmo.SetMode(GIZMO_TRANSLATE);
	GLBottomLeft->CurrentGizmo.SetMode(GIZMO_TRANSLATE);
	GLBottomRight->CurrentGizmo.SetMode(GIZMO_TRANSLATE);
	Update();
}

void MainFrame::OnToolRotate( wxCommandEvent& event )
{
	GLTopLeft->CurrentGizmo.SetMode(GIZMO_ROTATE);
	GLTopRight->CurrentGizmo.SetMode(GIZMO_ROTATE);
	GLBottomLeft->CurrentGizmo.SetMode(GIZMO_ROTATE);
	GLBottomRight->CurrentGizmo.SetMode(GIZMO_ROTATE);
	Update();
}

void MainFrame::OnToolScale( wxCommandEvent& event )
{
	GLTopLeft->CurrentGizmo.SetMode(CurrentScaleType);
	GLTopRight->CurrentGizmo.SetMode(CurrentScaleType);
	GLBottomLeft->CurrentGizmo.SetMode(CurrentScaleType);
	GLBottomRight->CurrentGizmo.SetMode(CurrentScaleType);
	Update();
}

void MainFrame::OnToolClone( wxCommandEvent& event )
{
	if(CurrentGeom)
	{
		sym::CGeometry *NewGeom = new sym::CGeometry;

		*NewGeom = *CurrentGeom;
		NewGeom->Mesh = new sym::CMesh;
		*NewGeom->Mesh = *CurrentGeom->Mesh;

		NewGeom->RigidBody = 0;

		//Quick way of recreating physics body
		NewGeom->Scale(sym::CVector3(1.0f, 1.0f, 1.0f));

		NewGeom->Name += "_copy";
		Scene.AddGeometry(NewGeom);
		CurrentGeom = NewGeom;
		Assets->LstGeometry->Append(wxString(CurrentGeom->Name.c_str(), wxConvUTF8));
		Update();
		UpdateAssetsFrame();
	}
}

void MainFrame::OnToolUndo( wxCommandEvent& event )
{
	Undo();
	Update();
	UpdateAssetsFrame();
}

void MainFrame::OnToolDelete( wxCommandEvent& event )
{
	if(CurrentGeom)
	{
		int Result = Assets->LstGeometry->FindString(wxString(CurrentGeom->Name.c_str(), wxConvUTF8));
		Assets->LstGeometry->Delete(Result);
		CurrentGeom->DestroyNewtonBody();
		Scene.DeleteGeometry(CurrentGeom);
		CurrentGeom = 0;
		Update();
		UpdateAssetsFrame();
	}
}

void MainFrame::Update()
{
	wxFrame::Update();
}

////////////////////////////////////////////////////////////////////////////////
// ASSETS DIALOG
void MainFrame::OnSelect( wxCommandEvent& event )
{
	UpdateAssetsFrame();
}

void MainFrame::PopulateGeometryList()
{
	Assets->LstGeometry->Clear();

	//Populate scene list
	for(std::map<std::string, sym::CGeometry*> ::iterator g = Scene.Geometry.begin(); g != Scene.Geometry.end(); g++)
	{
		//Add to scene list
		Assets->LstGeometry->Append(wxString((*g).first.c_str(), wxConvUTF8));
	}
}

void MainFrame::UpdateAssetsFrame()
{
	if(Assets->LstGeometry->GetSelection() < 0)
    {
        CurrentGeom = 0;
        Update();

        return;
    }

    wxString Name = Assets->LstGeometry->GetStringSelection();
	std::string SelectedGeom = std::string(Name.mb_str());
    CurrentGeom = Scene.GetGeometry(SelectedGeom);

	if(CurrentGeom)
	{
		Assets->TxtName->SetValue(Name);
		Assets->SpnMass->SetValue(CurrentGeom->Mass);
		Assets->TxtMaterial->SetValue(wxString(CurrentGeom->MatFilename.c_str(), wxConvUTF8));

		Assets->SpnPositionX->SetValue(CurrentGeom->GetPosition().x);
		Assets->SpnPositionY->SetValue(CurrentGeom->GetPosition().y);
		Assets->SpnPositionZ->SetValue(CurrentGeom->GetPosition().z);

		Assets->SpnRotationX->SetValue(CurrentGeom->GetRotation().ToEuler().x);
		Assets->SpnRotationY->SetValue(CurrentGeom->GetRotation().ToEuler().y);
		Assets->SpnRotationZ->SetValue(CurrentGeom->GetRotation().ToEuler().z);

		Assets->SpnScaleX->SetValue(CurrentGeom->GetDimentions().x);
		Assets->SpnScaleY->SetValue(CurrentGeom->GetDimentions().y);
		Assets->SpnScaleZ->SetValue(CurrentGeom->GetDimentions().z);

		Assets->SpnOffsetU->SetValue(CurrentGeom->TextureOffset.x);
		Assets->SpnOffsetV->SetValue(CurrentGeom->TextureOffset.y);

		Assets->SpnTileU->SetValue(CurrentGeom->TextureTile.x);
		Assets->SpnTileV->SetValue(CurrentGeom->TextureTile.y);

		Assets->DrpPhysics->SetSelection(CurrentGeom->PhysicsType);

		Assets->ChkGrapple1->SetValue((CurrentGeom->CustomData & GRAPPLE1) == GRAPPLE1);
		Assets->ChkGrapple2->SetValue((CurrentGeom->CustomData & GRAPPLE2) == GRAPPLE2);
		Assets->ChkVision1->SetValue((CurrentGeom->CustomData & VISION1) == VISION1);
		Assets->ChkVision2->SetValue((CurrentGeom->CustomData & VISION2) == VISION2);
		Assets->ChkSpawn->SetValue((CurrentGeom->CustomData & SPAWN) == SPAWN);
	}

    Update();
}

void MainFrame::OnNameChange( wxCommandEvent& event )
{
	if(CurrentGeom) 
	{
		//Remove from list
		int Result = Assets->LstGeometry->FindString(wxString(CurrentGeom->Name.c_str(), wxConvUTF8));
		Assets->LstGeometry->Delete(Result);

		//Set new name
		wxString wxName = Assets->TxtName->GetValue();
		std::string Name = std::string(wxName.mb_str());
		Scene.RenameGeometry(CurrentGeom, Name);

		//Re-add to list
		Assets->LstGeometry->Append(wxString(CurrentGeom->Name.c_str(), wxConvUTF8));
	}
}

void MainFrame::OnMaterialBrowse( wxCommandEvent& event )
{
	if(CurrentGeom)
	{
		wxFileDialog *OpenDialog= new wxFileDialog(this, _T("Open Symmetry material file"), _(""), _(""), _("Symmetry material files (*.sym.material)|*.sym.material"), wxOPEN);
		if ( OpenDialog->ShowModal() == wxID_OK )
		{
			std::string Filename = std::string(OpenDialog->GetFilename().mb_str());

			if(!CurrentGeom->SetMaterial(Filename))
			{
				wxMessageBox(_T("Error: Could not load material file"));
				return;
			}

			wxString wxFilename(Filename.c_str(), wxConvUTF8);
			Assets->TxtMaterial->SetValue(wxFilename);
		}

		OpenDialog->Close();

		Update();
	}
}

void MainFrame::OnPhysicsChange( wxCommandEvent& event )
{
	if(CurrentGeom)
	{
		int Physics = Assets->DrpPhysics->GetSelection();
		CurrentGeom->PhysicsType = Physics;

		//Setup collision body
		switch(Physics)
		{
			case sym::PHYSICS_OBJ_BOX:
				CurrentGeom->CreateBox(CurrentGeom->Mesh->AABB.Size);
				break;

			case sym::PHYSICS_OBJ_SPHERE:
				CurrentGeom->CreateSphere(CurrentGeom->Mesh->AABB.Size);
				break;

			case sym::PHYSICS_OBJ_CAPSULE:
				CurrentGeom->CreateCapsule(CurrentGeom->Mesh->AABB.Size);
				break;

			case sym::PHYSICS_OBJ_CUSTOM:
			{
				std::vector<sym::CVector3> Vertices;

				for(std::vector<sym::CMeshSubset>::iterator i = CurrentGeom->Mesh->Subsets.begin(); i != CurrentGeom->Mesh->Subsets.end(); i++)
				{
					Vertices.insert(Vertices.end(), i->Vertices.begin(), i->Vertices.end());
				}

				CurrentGeom->CreateCustom(Vertices);
				break;
			}

			case sym::LEVEL_COLLISION:
			{
				CurrentGeom->CreateLevel(CurrentGeom->Mesh->Subsets[0].Vertices, CurrentGeom->Mesh->Subsets[0].Faces);
				break;
			}

			case sym::NO_PHYSICS:
				break;

			default: 
				CurrentGeom->CreateBox(CurrentGeom->Mesh->AABB.Size);
				break;
		}

		//Set physics geometry
		CurrentGeom->SetGeometry(dynamic_cast<sym::CGeometry*>(CurrentGeom));

		Update();
	}
}

void MainFrame::OnMassChange( wxSpinEvent& event )
{
	if(CurrentGeom) CurrentGeom->SetMass(Assets->SpnMass->GetValue());
}

void MainFrame::OnPositionChange( wxSpinEvent& event )
{
	if(CurrentGeom)
	{
		sym::CVector3 Position;
		Position.x = Assets->SpnPositionX->GetValue();
		Position.y = Assets->SpnPositionY->GetValue();
		Position.z = Assets->SpnPositionZ->GetValue();
		Update();
	}
}

void MainFrame::OnRotationChange( wxSpinEvent& event )
{
	if(CurrentGeom)
	{
		sym::CVector3 Euler;
		Euler.x = Assets->SpnRotationX->GetValue() * sym::Maths::DEGREES_TO_RADIANS;
		Euler.y = Assets->SpnRotationY->GetValue() * sym::Maths::DEGREES_TO_RADIANS;
		Euler.z = Assets->SpnRotationZ->GetValue() * sym::Maths::DEGREES_TO_RADIANS;

		sym::CQuaternion Quat;
		Quat.FromEuler(Euler);
		CurrentGeom->SetRotation(Quat);

		Update();
	}
}

void MainFrame::OnScaleChange( wxSpinEvent& event )
{
	if(CurrentGeom)
	{
		if(CurrentGeom->Type != sym::SYM_MDL)
		{
			sym::CVector3 Scale;
			Scale.x = Assets->SpnScaleX->GetValue();
			Scale.y = Assets->SpnScaleY->GetValue();
			Scale.z = Assets->SpnScaleZ->GetValue();

			CurrentGeom->Scale(Scale);

			Update();
		}
	}
}

void MainFrame::OnUVChange( wxSpinEvent& event )
{
	if(CurrentGeom)
	{
		sym::CVector3 UVTile;
		sym::CVector3 UVOffset;

		UVTile.x = Assets->SpnTileU->GetValue();
		UVTile.y = Assets->SpnTileV->GetValue();
		UVTile.z = 1.0f;

		UVOffset.x = Assets->SpnOffsetU->GetValue();
		UVOffset.y = Assets->SpnOffsetV->GetValue();
		
		CurrentGeom->TextureTile = UVTile;
		CurrentGeom->TextureOffset = UVOffset;

		Update();
	}
}

void MainFrame::OnChkGrapple1( wxCommandEvent& event )
{
	if(CurrentGeom) CurrentGeom->CustomData ^= GRAPPLE1;
}

void MainFrame::OnChkGrapple2( wxCommandEvent& event )
{
	if(CurrentGeom) CurrentGeom->CustomData ^= GRAPPLE2;
}

void MainFrame::OnChkVision1( wxCommandEvent& event )
{
	if(CurrentGeom) CurrentGeom->CustomData ^= VISION1;
}

void MainFrame::OnChkVision2( wxCommandEvent& event )
{
	if(CurrentGeom) CurrentGeom->CustomData ^= VISION2;
}

void MainFrame::OnChkSpawn( wxCommandEvent& event )
{
	if(CurrentGeom) CurrentGeom->CustomData ^= SPAWN;
}