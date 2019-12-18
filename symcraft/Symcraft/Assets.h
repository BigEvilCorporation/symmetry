
#ifndef __noname__
#define __noname__

#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/spinctrl.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/checkbox.h>
#include <wx/notebook.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class AssetsFrame
///////////////////////////////////////////////////////////////////////////////
class AssetsFrame : public wxFrame 
{
	private:
	
	public:
		wxNotebook* Tabs;
		wxPanel* TabPanelGeometry;
		wxListBox* LstGeometry;
		wxStaticText* LblName;
		wxTextCtrl* TxtName;
		wxStaticText* LblMaterial;
		wxTextCtrl* TxtMaterial;
		wxButton* BtnBrowseMaterial;
		wxStaticText* LblPhysics;
		wxChoice* DrpPhysics;
		wxStaticText* LblMass;
		wxSpinCtrl* SpnMass;
		wxStaticText* LblPosition;
		wxSpinCtrl* SpnPositionX;
		wxSpinCtrl* SpnPositionY;
		wxSpinCtrl* SpnPositionZ;
		wxStaticText* LblRotation;
		wxSpinCtrl* SpnRotationX;
		wxSpinCtrl* SpnRotationY;
		wxSpinCtrl* SpnRotationZ;
		wxStaticText* LblScale;
		wxSpinCtrl* SpnScaleX;
		wxSpinCtrl* SpnScaleY;
		wxSpinCtrl* SpnScaleZ;
		wxStaticText* LblUVTile;
		wxSpinCtrl* SpnTileU;
		wxSpinCtrl* SpnTileV;
		wxStaticText* LblUVOffset;
		wxSpinCtrl* SpnOffsetU;
		wxSpinCtrl* SpnOffsetV;
		wxPanel* TabPanelSasha;
		wxCheckBox* ChkGrapple1;
		wxCheckBox* ChkGrapple2;
		wxCheckBox* ChkVision1;
		wxCheckBox* ChkVision2;
		wxCheckBox* ChkSpawn;
	
	public:
		
		AssetsFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Assets"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 530, 350 ), long style = wxCAPTION|wxCLOSE_BOX|wxFRAME_FLOAT_ON_PARENT|wxFRAME_NO_TASKBAR|wxFRAME_TOOL_WINDOW|wxSYSTEM_MENU|wxTAB_TRAVERSAL );
		~AssetsFrame();
	
};

#endif //__noname__
