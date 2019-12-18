#ifndef __GL_VIEWPORT
#define __GL_VIEWPORT

#ifdef WIN32
#include <windows.h>
#endif

#include <wx/wx.h>
#include <wx/glcanvas.h>

#include "GLGrid.h"
#include "ModifyObject.h"
#include "Gizmo.h"

#include "../../symmetry/SYM_Engine.h"

const int VIEW_TOP = 0;
const int VIEW_FRONT = 1;
const int VIEW_LEFT = 2;
const int VIEW_PERSPECTIVE = 3;

const int RENDER_SOLID = 0;
const int RENDER_WIREFRAME = 1;

class GLViewport : public wxGLCanvas
{
	public:
		GLViewport(wxFrame* parent, int InitialType);
		GLViewport(wxFrame* parent, wxGLContext* Context, int InitialType);

		void SetViewType(int ViewType);
		void SetRenderStyle(int RenderStyle) { Style = RenderStyle; }

		//Grid
		GLGrid Grid;

		//Gizmo
		Gizmo CurrentGizmo;

	private:

		//Viewport type and render style
		int Type, Style;

		int GetWidth();
		int GetHeight();

		float Zoom;

		int DeltaMouseX, DeltaMouseY, PrevMouseX, PrevMouseY;
		bool LeftMouse, MiddleMouse, RightMouse;

		void OnRender(wxPaintEvent& evt);
		void OnClear(wxEraseEvent& WXUNUSED(event));
		void OnIdle(wxIdleEvent & event);
		void OnResize(wxSizeEvent &event);

		void OnMouseWheel(wxMouseEvent& event);
		void OnMouseMove(wxMouseEvent& event);
		void OnMouseLeftWindow(wxMouseEvent& event);
		void OnLeftMouseDown(wxMouseEvent& event);
		void OnLeftMouseUp(wxMouseEvent& event);
		void OnMiddleMouseDown(wxMouseEvent& event);
		void OnMiddleMouseUp(wxMouseEvent& event);
		void OnRightMouseDown(wxMouseEvent& event);
		void OnRightMouseUp(wxMouseEvent& event);

		void Setup3DViewport(float LeftX, float LeftY, float BottomX, float BottomY);
		void Setup2DViewport(float LeftX, float LeftY, float BottomX, float BottomY);

		int MousePickGizmo(int x, int y);
		sym::CVector3 ScreenToWorld(int screenx, int screeny, int screenz);

		//Camera
		sym::CCamera Camera;

		DECLARE_EVENT_TABLE()
};

#endif