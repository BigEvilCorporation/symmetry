#ifndef APPASSETS_H_INCLUDED
#define APPASSETS_H_INCLUDED

#ifdef WIN32
#include <windows.h>
#endif

#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>
#include <wx/menu.h>
#include <wx/frame.h>

#include "AppOpenGL.h"
#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/spinctrl.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>
#include <wx/menu.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class AssetsFrame
///////////////////////////////////////////////////////////////////////////////
class AssetsFrame : public wxFrame
{
	private:

	protected:

	public:
		AssetsFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Assets"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxCAPTION|wxFRAME_FLOAT_ON_PARENT|wxFRAME_TOOL_WINDOW|wxTAB_TRAVERSAL );
		~AssetsFrame();

		wxNotebook* Tabs;

		//Scene tab
		wxPanel* TabScene;
		wxListBox* SceneList;
		wxButton* ButtonSelect;
		wxButton* ButtonDelete;
		wxButton* ButtonClone;
		wxStaticText* NameLabel;
		wxTextCtrl* TextName;
		wxStaticText* MassLabel;
		wxSpinCtrl* SpinMass;
		wxStaticText* MaterialLabel;
		wxTextCtrl* TextMaterial;

		//Texture tile/offset
		wxStaticText* TxtTexTile;
		wxSpinCtrl* TexTileX;
		wxSpinCtrl* TexTileY;
		wxStaticText* TxtTexOffset;
		wxSpinCtrl* TexOffsetX;
		wxSpinCtrl* TexOffsetY;
		wxStaticText* TxtTexRotate;
		wxSpinCtrl* TexRotateZ;

		//Models tab
		wxPanel* TabModels;
		wxListBox* ModelsList;
		wxButton* ButtonOpenMesh;
		wxButton* ButtonImportMesh;
		wxButton* ButtonPlace;

		//Materials tab
		wxPanel* TabMaterials;
		wxListBox* MaterialsList;
		wxButton* ButtonOpenMat;
		wxButton* ButtonImportMat;
		wxButton* ButtonNewMat;
		wxButton* ButtonEditMat;
		wxButton* ButtonApplyMat;

		//Terrain tab
		wxPanel* TabTerrain;
		wxListBox* TerrainList;
		wxButton* TerrOpen;
		wxStaticText* TxtTerrScale;
		wxSpinCtrl* TerrScaleX;
		wxSpinCtrl* TerrScaleY;
		wxSpinCtrl* TerrScaleZ;
		wxButton* ButtonTerrApply;

		//Skybox tab
		wxPanel* TabSkybox;
		wxStaticText* TxtSkyNorth;
		wxTextCtrl* InputSkyNorth;
		wxButton* ButtonSkyNorth;
		wxStaticText* TxtSkySouth;
		wxTextCtrl* InputSkySouth;
		wxButton* ButtonSkySouth;
		wxStaticText* TxtSkyEast;
		wxTextCtrl* InputSkyEast;
		wxButton* ButtonSkyEast;
		wxStaticText* TxtSkyWest;
		wxTextCtrl* InputSkyWest;
		wxButton* ButtonSkyWest;
		wxStaticText* TxtSkyTop;
		wxTextCtrl* InputSkyTop;
		wxButton* ButtonSkyTop;
		wxStaticText* TxtSkyBottom;
		wxTextCtrl* InputSkyBottom;
		wxButton* ButtonSkyBottom;
		wxButton* ButtonSkyApply;
};

#endif // APPASSETS_H_INCLUDED
