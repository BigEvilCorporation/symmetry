#ifdef WIN32
#include <windows.h>
#endif

#include <GL/glu.h>
#include <GL/gl.h>

#include <sstream>

#include "AppOpenGL.h"
#include "../../symmetry/SYM_Engine.h"

BEGIN_EVENT_TABLE(GLCanvas, wxGLCanvas)
    EVT_MOTION(GLCanvas::MouseMoved)
    EVT_LEFT_DOWN(GLCanvas::LeftMouseDown)
    EVT_LEFT_UP(GLCanvas::LeftMouseUp)
    EVT_RIGHT_DOWN(GLCanvas::RightMouseDown)
    EVT_RIGHT_UP(GLCanvas::RightMouseUp)
    EVT_LEAVE_WINDOW(GLCanvas::MouseLeftWindow)
    EVT_SIZE(GLCanvas::resized)
    EVT_KEY_DOWN(GLCanvas::keyPressed)
    EVT_KEY_UP(GLCanvas::keyReleased)
    EVT_MOUSEWHEEL(GLCanvas::MouseWheelMoved)
    EVT_PAINT(GLCanvas::render)
END_EVENT_TABLE()

void GLCanvas::MouseMoved(wxMouseEvent& event)
{
    //Get mouse coords of GL canvas
    int X = event.GetX();
    int Y = event.GetY();

    //Calculate delta
    DeltaMouseX = X - PrevMouseX;
    DeltaMouseY = Y - PrevMouseY;
    PrevMouseX = X;
    PrevMouseY = Y;

    if(LeftMouse)
    {
        Camera.Pitch((float)-DeltaMouseY * 0.25);
        Camera.Yaw((float)-DeltaMouseX * 0.25);
        Refresh();
    }

    if(RightMouse)
    {
        Camera.Move((float)-DeltaMouseY);
        Refresh();
    }
}

void GLCanvas::LeftMouseDown(wxMouseEvent& event)
{
    LeftMouse = true;
}

void GLCanvas::LeftMouseUp(wxMouseEvent& event)
{
    LeftMouse = false;
}

void GLCanvas::RightMouseDown(wxMouseEvent& event)
{
    RightMouse = true;
}

void GLCanvas::RightMouseUp(wxMouseEvent& event)
{
    RightMouse = false;
}

void GLCanvas::MouseWheelMoved(wxMouseEvent& event)
{
    int WheelDelta = event.GetWheelDelta();

    Camera.Move((float)WheelDelta * 0.25);
    Refresh();
}

void GLCanvas::MouseLeftWindow(wxMouseEvent& event)
{

}

void GLCanvas::keyPressed(wxKeyEvent& event) {}
void GLCanvas::keyReleased(wxKeyEvent& event) {}

GLCanvas::GLCanvas(wxFrame* parent, int Type) : wxGLCanvas(parent, wxID_ANY,  wxDefaultPosition, wxDefaultSize, 0, wxT("GLCanvas"),  0)
{
    LeftMouse = false;
    RightMouse = false;
    DeltaMouseX = 0;
    DeltaMouseY = 0;
    PrevMouseX = 0;
    PrevMouseY = 0;

    Camera.SetPosition(SYM::SYM_VECTOR3(-10.0f, -10.0f, -10.0f));
    Camera.SetRotation(SYM::SYM_VECTOR3(0.0f, 0.0f, 0.0f));
    Camera.Yaw(45.0f);
    Camera.Pitch(-10.0f);
}

GLCanvas::GLCanvas(wxFrame* parent, wxGLContext* Context, int Type) : wxGLCanvas(parent, Context, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxT("GLCanvas"), 0, wxNullPalette)
{
    LeftMouse = false;
    RightMouse = false;
    DeltaMouseX = 0;
    DeltaMouseY = 0;
    PrevMouseX = 0;
    PrevMouseY = 0;

    Camera.SetPosition(SYM::SYM_VECTOR3(-10.0f, -10.0f, -10.0f));
    Camera.SetRotation(SYM::SYM_VECTOR3(0.0f, 0.0f, 0.0f));
    Camera.Yaw(45.0f);
    Camera.Pitch(-10.0f);
}

void GLCanvas::resized(wxSizeEvent& evt)
{
    wxGLCanvas::OnSize(evt);

    Refresh();
}

void GLCanvas::prepare3DViewport(int topleft_x, int topleft_y, int bottomright_x, int bottomright_y)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glViewport(topleft_x, topleft_y, bottomright_x-topleft_x, bottomright_y-topleft_y);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float ratio_w_h = (float)(bottomright_x-topleft_x)/(float)(bottomright_y-topleft_y);
    gluPerspective(45, ratio_w_h, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

int GLCanvas::getWidth()
{
    return GetSize().x;
}

int GLCanvas::getHeight()
{
    return GetSize().y;
}

void GLCanvas::render( wxPaintEvent& evt )
{
    if(!IsShown()) return;

    wxGLCanvas::SetCurrent();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    prepare3DViewport(0, 0, getWidth(), getHeight());

    //Apply camera
    Camera.ApplyMatrix();


    SwapBuffers();
}
