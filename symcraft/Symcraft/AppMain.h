#ifndef APPMAIN_H_INCLUDED
#define APPMAIN_H_INCLUDED

#ifdef WIN32
#include <windows.h>
#endif

#include "wx/wxprec.h"
#include <wx/setup.h>
#include <wx/wx.h>

#include <wx/statusbr.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/spinctrl.h>
#include <wx/toolbar.h>
#include <wx/panel.h>
#include <wx/sizer.h>

#include "Assets.h"
#include "GLViewport.h"
#include "ModifyObject.h"

#include "../../symmetry/SYM_Engine.h"

//Forward declaration
class GLViewport;

class SymcraftApp : public wxApp
{
    public:
        virtual bool OnInit();

    private:
        //Init Symmetry engine
		bool InitSymEngine();
};

DECLARE_APP(SymcraftApp)

class CPhysicsTimer : public wxTimer
{
	public:
		CPhysicsTimer(wxFrame *Owner);
		void Notify();

	private:
		wxFrame *OwnerFrame;
};

///////////////////////////////////////////////////////////////////////////////
/// Class MainFrame
///////////////////////////////////////////////////////////////////////////////
class MainFrame : public wxFrame 
{
	private:
	
	protected:
		wxStatusBar* StatusBar;
		wxMenuBar* Menu;
		wxMenu* MenuFile;
		wxMenu* MenuEdit;
		wxToolBar* ToolbarMain;
		wxStaticText* TextScale;
		wxChoice* DropScale;
		wxStaticText* TextGrid;
		wxChoice* DropGrid;
		wxStaticText* TextRotate;
		wxSpinCtrl* SpinRotate;

		wxToolBar* ToolbarTopLeft;
		wxChoice* DropViewTopLeft;
		wxRadioButton* RadioViewTopLeftSolid;
		wxRadioButton* RadioViewTopLeftWireframe;

		wxToolBar* ToolbarTopRight;
		wxChoice* DropViewTopRight;
		wxRadioButton* RadioViewTopRightSolid;
		wxRadioButton* RadioViewTopRightWireframe;

		wxToolBar* ToolbarBottomLeft;
		wxChoice* DropViewBottomLeft;
		wxRadioButton* RadioViewBottomLeftSolid;
		wxRadioButton* RadioViewBottomLeftWireframe;

		wxToolBar* ToolbarBottomRight;
		wxChoice* DropViewBottomRight;
		wxRadioButton* RadioViewBottomRightSolid;
		wxRadioButton* RadioViewBottomRightWireframe;

		//Assets window
		AssetsFrame *Assets;
		
		// Virtual event handlers, overide them in your derived class
		void OnClear(wxEraseEvent& evt);
		void OnFileNew( wxCommandEvent& event );
		void OnFileOpen( wxCommandEvent& event );
		void OnFileSave( wxCommandEvent& event );
		void OnFileSaveAs( wxCommandEvent& event );
		void OnFileExit( wxCommandEvent& event );

		//Brush creation tools
		void OnToolPlane( wxCommandEvent& event );
		void OnToolBox( wxCommandEvent& event );
		virtual void OnToolCylinder( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnToolSphere( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnToolCone( wxCommandEvent& event ){ event.Skip(); }

		void OnToolMesh( wxCommandEvent& event );
		virtual void OnToolTerrain( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnToolWater( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnToolRope( wxCommandEvent& event ){ event.Skip(); }

		void OnToolRunStopPhysics( wxCommandEvent& event );
		void OnToolResetPhysics( wxCommandEvent& event );

		void OnToolSelect( wxCommandEvent& event );
		void OnToolMove( wxCommandEvent& event );
		void OnToolRotate( wxCommandEvent& event );
		void OnToolScale( wxCommandEvent& event );
		void OnToolClone( wxCommandEvent& event );
		void OnToolUndo( wxCommandEvent& event );
		void OnToolDelete( wxCommandEvent& event );
		void OnDropScale( wxCommandEvent& event );
		void OnDropGrid( wxCommandEvent& event );
		void OnSpinRotate( wxSpinEvent& event );

		void OnDropViewTopLeft( wxCommandEvent& event );
		void OnDropViewTopRight( wxCommandEvent& event );
		void OnDropViewBottomLeft( wxCommandEvent& event );
		void OnDropViewBottomRight( wxCommandEvent& event );

		virtual void OnToolTopLeftFullscreen( wxCommandEvent& event ){ event.Skip(); }
		void OnToolTopLeftReset( wxCommandEvent& event );
		void OnRadioTopLeftSolid( wxCommandEvent& event );
		void OnRadioTopLeftWireframe( wxCommandEvent& event );
		
		virtual void OnToolTopRightFullscreen( wxCommandEvent& event ){ event.Skip(); }
		void OnToolTopRightReset( wxCommandEvent& event );
		void OnRadioTopRightSolid( wxCommandEvent& event );
		void OnRadioTopRightWireframe( wxCommandEvent& event );
		
		virtual void OnToolBottomLeftFullscreen( wxCommandEvent& event ){ event.Skip(); }
		void OnToolBottomLeftReset( wxCommandEvent& event );
		void OnRadioBottomLeftSolid( wxCommandEvent& event );
		void OnRadioBottomLeftWireframe( wxCommandEvent& event );
		
		virtual void OnToolBottomRightFullscreen( wxCommandEvent& event ){ event.Skip(); }
		void OnToolBottomRightReset( wxCommandEvent& event );
		void OnRadioBottomRightSolid( wxCommandEvent& event );
		void OnRadioBottomRightWireframe( wxCommandEvent& event );

		//Assets dialog
		void OnSelect( wxCommandEvent& event );
		void OnNameChange( wxCommandEvent& event );
		void OnMaterialBrowse( wxCommandEvent& event );
		void OnPhysicsChange( wxCommandEvent& event );
		void OnMassChange( wxSpinEvent& event );
		void OnPositionChange( wxSpinEvent& event );
		void OnRotationChange( wxSpinEvent& event );
		void OnScaleChange( wxSpinEvent& event );
		void OnUVChange( wxSpinEvent& event );

		void OnChkGrapple1( wxCommandEvent& event );
		void OnChkGrapple2( wxCommandEvent& event );
		void OnChkVision1( wxCommandEvent& event );
		void OnChkVision2( wxCommandEvent& event );
		void OnChkSpawn( wxCommandEvent& event );

		//Tool button ID's
		int ToolIDAddPlane;
		int ToolIDAddBox;
		int ToolIDAddCylinder;
		int ToolIDAddSphere;
		int ToolIDAddCone;

		int ToolIDSelect;
		int ToolIDMove;
		int ToolIDRotate;
		int ToolIDScale;
		int ToolIDClone;
		int ToolIDUndo;
		int ToolIDDelete;

		int ToolIDMesh;
		int ToolIDTerrain;
		int ToolIDWater;
		int ToolIDRope;

		int ToolIDRunStopPhysics;
		int ToolIDResetPhysics;

		int ToolIDFullscreenTL;
		int ToolIDFullscreenTR;
		int ToolIDFullscreenBL;
		int ToolIDFullscreenBR;
		int ToolIDResetTL;
		int ToolIDResetTR;
		int ToolIDResetBL;
		int ToolIDResetBR;

		//OpenGL canvases for viewports
		GLViewport *GLTopLeft;
		GLViewport *GLTopRight;
		GLViewport *GLBottomLeft;
		GLViewport *GLBottomRight;

		//Current grid resolution
		int CurrentGridRes;
		
		//Shared OGL context
		wxGLContext* GLContext;

		void Update();
		void PopulateGeometryList();
		void UpdateAssetsFrame();

		CPhysicsTimer *PhysicsTimer;
		
		bool PhysicsRunning;

		std::string CurrentFile;

	public:
		MainFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Symcraft - Symmetry Engine Scene Editor"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1035,756 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~MainFrame();

		void OnPhysicsTimer(wxTimerEvent& event);
};

//Custom geometry data flags
enum
{
	GRAPPLE1 = 1,
	GRAPPLE2 = 2,
	VISION1 = 4,
	VISION2 = 8,
	VISION3 = 16,
	SPAWN = 32
};

MainFrame *Frame;

#endif // APPMAIN_H_INCLUDED
