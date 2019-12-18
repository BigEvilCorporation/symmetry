#ifdef WIN32
#include <windows.h>
#endif

#include <GL/glu.h>
#include <GL/gl.h>

#include <cmath>
#include <sstream>

#include "AppGLViewport.h"
#include "../../symmetry/SYM_Engine.h"

BEGIN_EVENT_TABLE(GLViewport, wxGLCanvas)
    EVT_MOTION(GLViewport::MouseMoved)
    EVT_LEFT_DOWN(GLViewport::LeftMouseDown)
    EVT_LEFT_UP(GLViewport::LeftMouseUp)
    EVT_RIGHT_DOWN(GLViewport::RightMouseDown)
    EVT_RIGHT_UP(GLViewport::RightMouseUp)
    EVT_MIDDLE_DOWN(GLViewport::MiddleMouseDown)
    EVT_MIDDLE_UP(GLViewport::MiddleMouseUp)
    EVT_LEAVE_WINDOW(GLViewport::MouseLeftWindow)
    EVT_SIZE(wxGLCanvas::OnSize)
    EVT_KEY_DOWN(GLViewport::keyPressed)
    EVT_KEY_UP(GLViewport::keyReleased)
    EVT_MOUSEWHEEL(GLViewport::MouseWheelMoved)
    EVT_PAINT(GLViewport::render)
	EVT_ERASE_BACKGROUND(GLViewport::EraseBackground)
	EVT_IDLE(GLViewport::OnIdle)
	EVT_SIZE(GLViewport::OnResize)
END_EVENT_TABLE()

//Font
SYM::SYM_FONT *Arial24 = 0;

float RoundTo(float Value, int Nearest)
{
    return floor(Value / Nearest) * Nearest;
}

GLViewport::GLViewport(wxFrame* parent, MainFrame *CustomParent, int Type) : wxGLCanvas(parent, wxID_ANY,  wxDefaultPosition, wxDefaultSize, 0, wxT("GLViewport"),  0)
{
    ViewType = Type;

	ParentFrame = CustomParent;

    SelectedGeom = 0;
    LeftMouse = false;
    RightMouse = false;
    DeltaMouseX = 0;
    DeltaMouseY = 0;
    PrevMouseX = 0;
    PrevMouseY = 0;
    OrthoZoom = 100.0f;
    CamX = 0.0f;
    CamY = 0.0f;
    GridResolution = 5;
    RotationSnap = 5;
	SnapX = SnapY = (float)GridResolution * 0.5f;
}

GLViewport::GLViewport(wxFrame* parent, MainFrame *CustomParent, wxGLContext* Context, int Type) : wxGLCanvas(parent, Context, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxT("GLViewport"), 0, wxNullPalette)
{
    ViewType = Type;

	ParentFrame = CustomParent;

    SelectedGeom = 0;
    LeftMouse = false;
    RightMouse = false;
    DeltaMouseX = 0;
    DeltaMouseY = 0;
    PrevMouseX = 0;
    PrevMouseY = 0;
    OrthoZoom = 100.0f;
    CamX = 0.0f;
    CamY = 0.0f;
    GridResolution = 5;
    RotationSnap = 5;
	SnapX = SnapY = (float)GridResolution * 0.5f;
}

bool GLViewport::Init()
{
    SetCurrent();

	LeftMouse = false;
    RightMouse = false;
    DeltaMouseX = 0;
    DeltaMouseY = 0;
    PrevMouseX = 0;
    PrevMouseY = 0;
    OrthoZoom = 100.0f;
    CamX = 0.0f;
    CamY = 0.0f;
    GridResolution = 5;
    RotationSnap = 5;

    //Load font
    if(!Arial24)
    {
        Arial24 = new SYM::SYM_FONT;
        if(!Arial24->LoadTTFFont("arial.ttf", 24))
        {
            wxMessageBox(_T("Error: Could not load arial.ttf"));
            return false;
        }

        Arial24->SetColour(255, 255, 255);
    }

    switch(ViewType)
    {
        case CAM_PERSPECTIVE:
            ViewportText = "Perspective";
            Camera.SetPosition(SYM::SYM_VECTOR3(100.0f, 50.0f, 100.0f));
            Camera.SetRotation(SYM::SYM_VECTOR3(0.0f, 0.0f, 0.0f));
            Camera.Yaw(45.0f);
            Camera.Pitch(-20.0f);
            break;

        case CAM_TOP:
            ViewportText = "Top";
            Camera.SetRotation(SYM::SYM_VECTOR3(0.0f, 0.0f, 0.0f));
            Camera.Pitch(-90.0f);
            break;

        case CAM_FRONT:
            ViewportText = "Front";
            Camera.SetRotation(SYM::SYM_VECTOR3(0.0f, 0.0f, 0.0f));
            break;

        case CAM_LEFT:
            ViewportText = "Left";
            Camera.SetRotation(SYM::SYM_VECTOR3(0.0f, 0.0f, 0.0f));
            Camera.Yaw(-90.0f);
            break;

        default: break;
    }

    return true;
}

void GLViewport::EraseBackground(wxEraseEvent& WXUNUSED(event))
{

}

void GLViewport::OnResize(wxSizeEvent &event)
{
	wxGLCanvas::OnSize(event);

    Refresh();
	Update();
}

void GLViewport::OnIdle(wxIdleEvent & event)
{
	//Refresh();
	//event.RequestMore();
}

void GLViewport::Setup3DViewport(float LeftX, float LeftY, float BottomX, float BottomY)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glViewport(LeftX, LeftY, BottomX - LeftX, BottomY - LeftY);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float ratio_w_h = (BottomX - LeftX)/(BottomY - LeftY);
    gluPerspective(45, ratio_w_h, 0.1f, 1000.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	//Light colours
	GLfloat ambientLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat diffuseLight[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat specularLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat position[] = { 100.0f, 100.0f, -100.0f, 1.0f };

	//Position lights
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

}

void GLViewport::Setup2DViewport(float LeftX, float LeftY, float BottomX, float BottomY)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glDisable(GL_DEPTH_TEST);

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);

	GLfloat ambientLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight);

    glViewport(LeftX, LeftY, BottomX - LeftX, BottomY - LeftY);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho (-OrthoZoom, OrthoZoom, -OrthoZoom * BottomY / BottomX, OrthoZoom * BottomY / BottomX, -OrthoZoom, OrthoZoom);

	glTranslatef(CamX, CamY, 0);
	DrawGrid(CamX, CamY, (float)getWidth()/2 * OrthoZoom, GridResolution);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int GLViewport::getWidth()
{
    return GetSize().x;
}

int GLViewport::getHeight()
{
    return GetSize().y;
}

void GLViewport::render( wxPaintEvent& evt )
{
    if(!IsShown()) return;

    wxGLCanvas::SetCurrent();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(ViewType == CAM_PERSPECTIVE) Setup3DViewport(0.0f, 0.0f, (float)getWidth(), (float)getHeight());
    else Setup2DViewport(0.0f, 0.0f, (float)getWidth(), (float)getHeight());

	if(ViewType == CAM_PERSPECTIVE)
	{
		if(SYM::Engine->Scene.Skybox.IsLoaded)
		{
		    //Push matrix
		    glPushMatrix();

		    //get camera's inverse matrix
			SYM::SYM_MATRIX4 RotMatrix = Camera.GetMatrix().GetInverse();

		    //Remove translation
		    RotMatrix.set(3, 0, 0);
		    RotMatrix.set(3, 1, 0);
		    RotMatrix.set(3, 2, 0);

		    //Apply matrix
		    glMultMatrixf(RotMatrix.GetAsFloatArray());

			//Disable depth testing
			glDisable(GL_DEPTH_TEST);

			//Draw skybox
			SYM::Engine->Scene.Skybox.Render();

			//Enable depth testing
			glEnable(GL_DEPTH_TEST);

            //Pop matrix
			glPopMatrix();
		}
	}

	//Apply camera
	Camera.ApplyMatrix();

    switch(ViewType)
    {
        case CAM_PERSPECTIVE:
            DrawGrid(0.0f, 0.0f, 100.0f, GridResolution);
            break;
        default: break;
    }

    //Render geometry in render list
    for(std::map<std::string, SYM::SYM_GEOMETRY*> ::iterator g = SYM::Engine->Scene.Geometry.begin(); g != SYM::Engine->Scene.Geometry.end(); g++)
    {
        (*g).second->Render();
        (*g).second->RenderCollision(SYM::SYM_COLOUR(255, 0, 0));
    }

    //Render outline of selected geom
    if(SelectedGeom) SelectedGeom->RenderCollision(SYM::SYM_COLOUR(0, 0, 255));

    //Render bounding box of selected geom
    //if(SelectedGeom) SelectedGeom->GetWorldAABB().Render();

    SYM::glEnable2D();
    if(Arial24) Arial24->RenderText(ViewportText, 5, 5);
    SYM::glDisable2D();

    SwapBuffers();

	SYM::Engine->Scene.DrawSky = false;
}

void GLViewport::DrawGrid(float CentreX, float CentreY, float Size, int Resolution)
{
    int Spacing[2] = {Resolution, Resolution * 10};

	Size = RoundTo(Size, 50);

	float OffsetX = std::fmod(CentreX, Resolution * Resolution * 2);
	float OffsetY = std::fmod(CentreY, Resolution * Resolution * 2);

    for(int grid = 0; grid < 2 ; grid++)
    {
        if(grid > 0)
		{
			glColor3ub(0, 255, 0);
			glLineWidth(2.0f);
		}
		else
		{
			glColor3ub(100, 100, 100);
			glLineWidth(1.0f);
		}

		glBegin(GL_LINES);

        for(float x = -CentreX - Size; x < -CentreX + Size; x += Spacing[grid])
        {
			if(ViewType == CAM_FRONT)
			{
				glVertex3f(x + OffsetX, -CentreY - Size, 0.0f);
				glVertex3f(x + OffsetX, -CentreY + Size, 0.0f);
			}
			else if(ViewType == CAM_LEFT)
			{
				glVertex3f(0.0f, -CentreY - Size, x + OffsetX);
				glVertex3f(0.0f, -CentreY + Size, x + OffsetX);
			}
			else
			{
				glVertex3f(x + OffsetX, 0.0f, -CentreY - Size);
				glVertex3f(x + OffsetX, 0.0f, -CentreY + Size);
			}
        }

        for(float y = -CentreY - Size; y < -CentreY + Size; y += Spacing[grid])
        {
			if(ViewType == CAM_FRONT)
			{
				glVertex3f(-CentreX - Size, y + OffsetY, 0.0f);
				glVertex3f(-CentreX + Size, y + OffsetY, 0.0f);
			}
			else if(ViewType == CAM_LEFT)
			{
				glVertex3f(0.0f, y + OffsetY, -CentreX - Size);
				glVertex3f(0.0f, y + OffsetY, -CentreX + Size);
			}
			else
			{
				glVertex3f(-CentreX - Size, 0.0f, y + OffsetY);
				glVertex3f(-CentreX + Size, 0.0f, y + OffsetY);
			}
        }

		glEnd();
    }

	glLineWidth(1.0f);
}

void GLViewport::SetTool(int ToolType)
{
    Tool = ToolType;

    switch(Tool)
    {
        case TOOL_SCALE_UNIFORM:
            //SetCursor(wxCursor(wxCURSOR_SIZENESW));
            break;
    }

}

void GLViewport::MouseMoved(wxMouseEvent& event)
{
    //Get mouse coords of GL canvas
    int X = event.GetX();
    int Y = event.GetY();

    //Calculate delta
    DeltaMouseX = X - PrevMouseX;
    DeltaMouseY = Y - PrevMouseY;
    PrevMouseX = X;
    PrevMouseY = Y;

    if(MiddleMouse && wxGetKeyState(WXK_ALT))
    {
        if(ViewType == CAM_PERSPECTIVE)
        {
            Camera.Pitch((float)-DeltaMouseY * 0.25);
            Camera.Yaw((float)-DeltaMouseX * 0.25);
        }
        else
        {
            CamY -= (float)DeltaMouseY * 0.5f;
            CamX += (float)DeltaMouseX * 0.5f;
        }

        Refresh();
		Update();
    }
    else if(MiddleMouse && wxGetKeyState(WXK_CONTROL))
    {
        if(ViewType == CAM_PERSPECTIVE)
        {
            Camera.Move((float)-DeltaMouseY);
        }
        else
        {
            OrthoZoom += ((float)DeltaMouseY);
            if(OrthoZoom < 1.0f) OrthoZoom = 1.0f;
        }

        Refresh();
		Update();
    }
    else if(LeftMouse)
    {
        SYM::SYM_VECTOR3 Position;
        SYM::SYM_VECTOR3 Rotation;
        if(SelectedGeom) Position = SelectedGeom->GetPosition();
        if(SelectedGeom) Rotation = SelectedGeom->GetRotation();

        switch(Tool)
        {
            case TOOL_SCALE_UNIFORM:
                if(SelectedGeom)
				{
					if(SelectedGeom->Type != SYM_TERR)
					{
						if(DeltaMouseX > 0) SelectedGeom->Scale(SYM::SYM_VECTOR3(1.05f, 1.05f, 1.05f));
						else if(DeltaMouseX < 0) SelectedGeom->Scale(SYM::SYM_VECTOR3(0.95f, 0.95f, 0.95f));
					}
				}
                break;

            case TOOL_SCALE_X:
                if (ViewType == CAM_TOP)
                    {
                        if(SelectedGeom)
                        {
							if(SelectedGeom->Type != SYM_TERR)
							{
								if(DeltaMouseX > 0) SelectedGeom->Scale(SYM::SYM_VECTOR3(1.05f, 1.0f, 1.0f));
								else if(DeltaMouseX < 0) SelectedGeom->Scale(SYM::SYM_VECTOR3(0.95f, 1.0f, 1.0f));

								if(DeltaMouseY > 0) SelectedGeom->Scale(SYM::SYM_VECTOR3(1.0f, 1.0f, 0.95f));
								else if(DeltaMouseY < 0) SelectedGeom->Scale(SYM::SYM_VECTOR3(1.0f, 1.0f, 1.05f));
							}
                        }
                    }
                    else if (ViewType == CAM_LEFT)
                    {
                        if(SelectedGeom)
                        {
							if(SelectedGeom->Type != SYM_TERR)
							{
								if(DeltaMouseY > 0) SelectedGeom->Scale(SYM::SYM_VECTOR3(1.0f, 0.95f, 1.0f));
								else if(DeltaMouseY < 0) SelectedGeom->Scale(SYM::SYM_VECTOR3(1.0f, 1.05f, 1.0f));

								if(DeltaMouseX > 0) SelectedGeom->Scale(SYM::SYM_VECTOR3(1.0f, 1.0f, 1.05f));
								else if(DeltaMouseX < 0) SelectedGeom->Scale(SYM::SYM_VECTOR3(1.0f, 1.0f, 0.95f));
							}
                        }
                    }
                    else if (ViewType == CAM_FRONT)
                    {
                        if(SelectedGeom)
                        {
							if(SelectedGeom->Type != SYM_TERR)
							{
								if(DeltaMouseX > 0) SelectedGeom->Scale(SYM::SYM_VECTOR3(1.05f, 1.0f, 1.0f));
								else if(DeltaMouseX < 0) SelectedGeom->Scale(SYM::SYM_VECTOR3(0.95f, 1.0f, 1.0f));

								if(DeltaMouseY > 0) SelectedGeom->Scale(SYM::SYM_VECTOR3(1.0f, 0.95f, 1.0f));
								else if(DeltaMouseY < 0) SelectedGeom->Scale(SYM::SYM_VECTOR3(1.0f, 1.05f, 1.0f));
							}
                        }
                    }

                    break;

            case TOOL_MOVE:
                if (ViewType == CAM_TOP)
                {
					if(wxGetKeyState(WXK_SHIFT))
                    {
                        Snap(&Position.x, &Position.z, GridResolution, GridResolution);
                    }
					else
					{
						Position.x += (float)DeltaMouseX * (OrthoZoom * 0.0025f);
						Position.z += (float)DeltaMouseY * (OrthoZoom * 0.0025f);
					}
                }
                else if (ViewType == CAM_LEFT)
                {
                    if(wxGetKeyState(WXK_SHIFT))
                    {
                        Snap(&Position.z, &Position.y, GridResolution, -GridResolution);
                    }
                    else
                    {
                        Position.z += (float)DeltaMouseX * (OrthoZoom * 0.0025f);
                        Position.y -= (float)DeltaMouseY * (OrthoZoom * 0.0025f);
                    }
                }
                else if (ViewType == CAM_FRONT)
                {
                    if(wxGetKeyState(WXK_SHIFT))
                    {
                        Snap(&Position.x, &Position.y, GridResolution, -GridResolution);
                    }
                    else
                    {
                        Position.x += (float)DeltaMouseX * (OrthoZoom * 0.0025f);
                        Position.y -= (float)DeltaMouseY * (OrthoZoom * 0.0025f);
                    }
                }

                break;

            case TOOL_ROTATE:
                if (ViewType == CAM_TOP)
                {
                    if(wxGetKeyState(WXK_SHIFT)) Snap(&Rotation.y, &Rotation.y, -GridResolution, -GridResolution);
                    else Rotation.y -= (float)DeltaMouseX * 0.1f;
                }

                 if (ViewType == CAM_LEFT)
                {
                    if(wxGetKeyState(WXK_SHIFT)) Snap(&Rotation.x, &Rotation.x, GridResolution, GridResolution);
                    else Rotation.x += (float)DeltaMouseX * 0.1f;
                }

                 if (ViewType == CAM_FRONT)
                {
                    if(wxGetKeyState(WXK_SHIFT)) Snap(&Rotation.z, &Rotation.z, -GridResolution, -GridResolution);
                    else Rotation.z -= (float)DeltaMouseX * 0.1f;
                }

                break;

            default:break;
        }

        if(SelectedGeom) SelectedGeom->SetPosition(Position);
        if(SelectedGeom) SelectedGeom->SetRotation(Rotation);
        ParentFrame->RefreshViewports();
    }

    //Display mouse coords in status bar
    stringstream MouseCoords;
    MouseCoords << "Mouse X: " << X << " Y: " << Y << " :: " << ViewportText << " :: Geometry: " << SYM::Engine->Scene.Geometry.size();
    ParentFrame->SetStatusText(wxString(MouseCoords.str().c_str(), wxConvUTF8), 1);
}

void GLViewport::LeftMouseDown(wxMouseEvent& event)
{
    LeftMouse = true;
}

void GLViewport::LeftMouseUp(wxMouseEvent& event)
{
    LeftMouse = false;
    ParentFrame->RefreshViewports();
}

void GLViewport::RightMouseDown(wxMouseEvent& event)
{
    RightMouse = true;
}

void GLViewport::RightMouseUp(wxMouseEvent& event)
{
    RightMouse = false;
    ParentFrame->RefreshViewports();
}

void GLViewport::MiddleMouseDown(wxMouseEvent& event)
{
    MiddleMouse = true;
}

void GLViewport::MiddleMouseUp(wxMouseEvent& event)
{
    MiddleMouse = false;
    ParentFrame->RefreshViewports();
}

void GLViewport::MouseWheelMoved(wxMouseEvent& event)
{
    if(ViewType == CAM_PERSPECTIVE) Camera.Move((float)event.GetWheelRotation()/event.GetWheelDelta() * 20);
    else OrthoZoom -= (float)event.GetWheelRotation()/event.GetWheelDelta() * 20;

    if(OrthoZoom < 1.0f) OrthoZoom = 1.0f;

    Refresh();
}

void GLViewport::MouseLeftWindow(wxMouseEvent& event)
{
	LeftMouse = false;
	RightMouse = false;
}

void GLViewport::keyPressed(wxKeyEvent& event)
{

}

void GLViewport::keyReleased(wxKeyEvent& event)
{
    ParentFrame->RefreshViewports();
}

void GLViewport::Snap(float *X, float *Y, int StepX, int StepY)
{
	SnapX += (float)DeltaMouseX * (OrthoZoom * 0.0025f);
	SnapY += (float)DeltaMouseY * (OrthoZoom * 0.0025f);

	if(SnapX < 0)
	{
		*X -= StepX;
		SnapX = (float)GridResolution * 0.5f;
	}
	if(SnapX > GridResolution)
	{
		*X += StepX;
		SnapX = (float)GridResolution * 0.5f;
	}
	if(SnapY < 0)
	{
		*Y -= StepY;
		SnapY = (float)GridResolution * 0.5f;
	}
	if(SnapY > GridResolution)
	{
		*Y += StepY;
		SnapY = (float)GridResolution * 0.5f;
	}

	*X = RoundTo(*X, StepX);
	*Y = RoundTo(*Y, StepY);
}
