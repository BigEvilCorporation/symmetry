#ifndef APPOPENGL_H_INCLUDED
#define APPOPENGL_H_INCLUDED

#ifdef WIN32
#include <windows.h>
#endif

#include <wx/wx.h>
#include <wx/glcanvas.h>

#include "../../symmetry/SYM_Engine.h"

class GLCanvas : public wxGLCanvas
{

public:
	GLCanvas(wxFrame* parent, int Type = 0);
	GLCanvas(wxFrame* parent, wxGLContext* Context, int Type = 0);

	void resized(wxSizeEvent& evt);

	int getWidth();
	int getHeight();

	void render(wxPaintEvent& evt);
	void prepare3DViewport(int topleft_x, int topleft_y, int bottomright_x, int bottomright_y);

	//Camera
	SYM::SYM_CAMERA Camera;

	// events
	void MouseMoved(wxMouseEvent& event);
	void LeftMouseDown(wxMouseEvent& event);
	void LeftMouseUp(wxMouseEvent& event);
	void RightMouseDown(wxMouseEvent& event);
	void RightMouseUp(wxMouseEvent& event);

	void MouseWheelMoved(wxMouseEvent& event);
	void MouseLeftWindow(wxMouseEvent& event);

	void keyPressed(wxKeyEvent& event);
	void keyReleased(wxKeyEvent& event);

	bool LeftMouse;
	bool RightMouse;
	int DeltaMouseX;
	int DeltaMouseY;
	int PrevMouseX;
	int PrevMouseY;

	DECLARE_EVENT_TABLE()
};

#endif
