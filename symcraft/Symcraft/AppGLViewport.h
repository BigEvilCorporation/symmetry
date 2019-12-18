#ifndef APPGLVIEWPORT_H_INCLUDED
#define APPGLVIEWPORT_H_INCLUDED

#define CAM_PERSPECTIVE 0
#define CAM_TOP         1
#define CAM_FRONT       2
#define CAM_LEFT        3

#define TOOL_MOVE           0
#define TOOL_ROTATE_X       1
#define TOOL_ROTATE_Y       2
#define TOOL_ROTATE         3
#define TOOL_SCALE_X        4
#define TOOL_SCALE_Y        5
#define TOOL_SCALE_UNIFORM  6

#ifdef WIN32
#include <windows.h>
#endif

#include <wx/wx.h>
#include <wx/glcanvas.h>

#include <map>

#include "../../symmetry/SYM_Engine.h"

#include "AppMain.h"

float RoundTo(float Value, int Nearest);

//Forward declaration
class MainFrame;

class GLViewport : public wxGLCanvas
{

public:
	GLViewport(wxFrame* parent, MainFrame *CustomParent, int Type = 0);
	GLViewport(wxFrame* parent, MainFrame *CustomParent, wxGLContext* Context, int Type = 0);

    bool Init();

	MainFrame *ParentFrame;

	//Viewport type - 0 = 2D, 1 = 3D
	int ViewType;

	int getWidth();
	int getHeight();

	void render(wxPaintEvent& evt);
	void EraseBackground(wxEraseEvent& WXUNUSED(event));
	void OnIdle(wxIdleEvent & event);
	void OnResize(wxSizeEvent &event);
	void Setup3DViewport(float LeftX, float LeftY, float BottomX, float BottomY);
	void Setup2DViewport(float LeftX, float LeftY, float BottomX, float BottomY);

	//Camera
	SYM::SYM_CAMERA Camera;

	string ViewportText;

	// events
	void MouseMoved(wxMouseEvent& event);
	void LeftMouseDown(wxMouseEvent& event);
	void LeftMouseUp(wxMouseEvent& event);
	void RightMouseDown(wxMouseEvent& event);
	void RightMouseUp(wxMouseEvent& event);
	void MiddleMouseDown(wxMouseEvent& event);
	void MiddleMouseUp(wxMouseEvent& event);

	void MouseWheelMoved(wxMouseEvent& event);
	void MouseLeftWindow(wxMouseEvent& event);

	void keyPressed(wxKeyEvent& event);
	void keyReleased(wxKeyEvent& event);

	bool LeftMouse;
	bool RightMouse;
	bool MiddleMouse;

	int DeltaMouseX;
	int DeltaMouseY;
	int PrevMouseX;
	int PrevMouseY;
	float OrthoZoom;

	int GridResolution;
	int RotationSnap;
	float SnapX, SnapY;

	void Snap(float *X, float *Y, int StepX, int StepY);

	float CamX, CamY;

	void SetTool(int ToolType);
	int Tool;

	SYM::SYM_GEOMETRY *SelectedGeom;

	//Utils
	void DrawGrid(float CentreX, float CentreY, float Size, int Resolution);
	void DrawGridXY(float r, float g, float b, GLfloat size, GLfloat resolution);
	void DrawGridXZ(float r, float g, float b, GLfloat size, GLfloat resolution);
	void DrawGridYZ(float r, float g, float b, GLfloat size, GLfloat resolution);

	DECLARE_EVENT_TABLE()
};

#endif // APPOPENGL_H_INCLUDED
