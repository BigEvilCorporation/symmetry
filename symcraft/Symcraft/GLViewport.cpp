#include <sstream>

#include "GLViewport.h"
#include "Scene.h"

BEGIN_EVENT_TABLE(GLViewport, wxGLCanvas)
	//Window events
	EVT_PAINT(GLViewport::OnRender)
	EVT_ERASE_BACKGROUND(GLViewport::OnClear)
	EVT_LEAVE_WINDOW(GLViewport::OnMouseLeftWindow)

	//Input
	EVT_MOUSEWHEEL(GLViewport::OnMouseWheel)
	EVT_LEFT_DOWN(GLViewport::OnLeftMouseDown)
    EVT_LEFT_UP(GLViewport::OnLeftMouseUp)
	EVT_MIDDLE_DOWN(GLViewport::OnMiddleMouseDown)
    EVT_MIDDLE_UP(GLViewport::OnMiddleMouseUp)
	EVT_RIGHT_DOWN(GLViewport::OnRightMouseDown)
    EVT_RIGHT_UP(GLViewport::OnRightMouseUp)
	EVT_MOTION(GLViewport::OnMouseMove)
END_EVENT_TABLE()

GLViewport::GLViewport(wxFrame* parent, int InitialType) : wxGLCanvas(parent, wxID_ANY,  wxDefaultPosition, wxDefaultSize, 0, wxT("GLViewport"),  0)
{
	DeltaMouseX = DeltaMouseY = PrevMouseX = PrevMouseY = 0;
	LeftMouse = MiddleMouse = RightMouse = false;
	SetViewType(InitialType);
	SetRenderStyle(RENDER_SOLID);
}

GLViewport::GLViewport(wxFrame* parent, wxGLContext* Context, int InitialType) : wxGLCanvas(parent, Context, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxT("GLViewport"), 0, wxNullPalette)
{
	DeltaMouseX = DeltaMouseY = PrevMouseX = PrevMouseY = 0;
	LeftMouse = MiddleMouse = RightMouse = false;
	SetViewType(InitialType);
	SetRenderStyle(RENDER_SOLID);
}	

int GLViewport::GetWidth()
{
    return GetSize().x;
}

int GLViewport::GetHeight()
{
    return GetSize().y;
}

void GLViewport::OnClear(wxEraseEvent& WXUNUSED(event))
{
	Update();
}

void GLViewport::OnResize(wxSizeEvent &event)
{
	wxGLCanvas::OnSize(event);
	Update();
}

void GLViewport::OnMouseWheel(wxMouseEvent& event)
{
    if(Type == 3)
	{
		Camera.Move((float)event.GetWheelRotation()/event.GetWheelDelta() * 20);
		if(CurrentGeom)	CurrentGizmo.SetLength(Camera.GetPosition().GetDistance(CurrentGeom->GetPosition()) / 15);
	}
    else
	{
		Zoom -= (float)event.GetWheelRotation()/event.GetWheelDelta() * 20;
		if(Zoom < 1.0f) Zoom = 1.0f;
		CurrentGizmo.SetLength(Zoom / 10);
	}

    Update();
}

void GLViewport::OnMouseLeftWindow(wxMouseEvent& event)
{
	LeftMouse = false;
	MiddleMouse = false;
	RightMouse = false;
	CurrentTool = 0;
	event.Skip();
}

void GLViewport::OnLeftMouseDown(wxMouseEvent& event)
{
	wxGLCanvas::SetCurrent();
	if(!RightMouse && CurrentGizmo.GetMode() == SELECT_MODE)
	{
		sym::RaycastData HitData;

		switch(Type)
		{
			case VIEW_FRONT:
			{
				sym::CVector3 Start = ScreenToWorld(PrevMouseX, PrevMouseY, 0);
				HitData = sym::Engine->Physics.Raycast(sym::CVector3(Start.x, Start.y, 10000), sym::CVector3(Start.x, Start.y, -10000));
			}
			break;

			case VIEW_TOP:
			{
				sym::CVector3 Start = ScreenToWorld(PrevMouseX, PrevMouseY, 0);
				HitData = sym::Engine->Physics.Raycast(sym::CVector3(Start.x, 10000, Start.z), sym::CVector3(Start.x, -10000, Start.z));
			}
			break;

			case VIEW_LEFT:
			{
				sym::CVector3 Start = ScreenToWorld(PrevMouseX, PrevMouseY, 0);
				HitData = sym::Engine->Physics.Raycast(sym::CVector3(-10000, Start.y, Start.z), sym::CVector3(10000, Start.y, Start.z));
			}
			break;

			case VIEW_PERSPECTIVE:
			{
				sym::CVector3 Start = ScreenToWorld(PrevMouseX, PrevMouseY, 0);
				HitData = sym::Engine->Physics.Raycast(Start, Start + (Camera.GetForward() * 10000));
			}
			break;

			default:
				break;
		}

		CurrentGeom = HitData.Geometry;
		GetParent()->Update();
	}

    LeftMouse = true;

	event.Skip();
}

void GLViewport::OnLeftMouseUp(wxMouseEvent& event)
{
    LeftMouse = false;
	PushUndoOperation();
	CurrentTool = 0;
	event.Skip();
}

void GLViewport::OnMiddleMouseDown(wxMouseEvent& event)
{
    MiddleMouse = true;
	event.Skip();
}

void GLViewport::OnMiddleMouseUp(wxMouseEvent& event)
{
    MiddleMouse = false;
	event.Skip();
}

void GLViewport::OnRightMouseDown(wxMouseEvent& event)
{
	RightMouse = true;
	event.Skip();
}

void GLViewport::OnRightMouseUp(wxMouseEvent& event)
{
	RightMouse = false;
	event.Skip();
}

void GLViewport::OnMouseMove(wxMouseEvent& event)
{
	wxGLCanvas::SetCurrent();

    //Get mouse coords of GL canvas
    int X = event.GetX();
    int Y = event.GetY();

    //Calculate delta
    DeltaMouseX = X - PrevMouseX;
    DeltaMouseY = Y - PrevMouseY;
    PrevMouseX = X;
    PrevMouseY = Y;

	if(CurrentTool > 0)
	{
		//A tool is currently in use, pass mouse delta and skip
		DoModify(DeltaMouseX, -DeltaMouseY);
		GetParent()->Update();
		event.Skip();
		return;
	}

	if(MiddleMouse)
	{
		Camera.Fly(DeltaMouseY);
		Camera.Strafe(-DeltaMouseX);
	}
	else if(RightMouse)
	{
		if(Type == VIEW_PERSPECTIVE)
		{
			//Left + Right mouse - rotate around current selection
			if(LeftMouse && CurrentGeom)
			{
				//Get current position of camera and geom
				sym::CVector3 CamPos = Camera.GetPosition();
				sym::CVector3 GeomPos = CurrentGeom->GetPosition();

				//Translate to object's position
				Camera.SetPosition(GeomPos);

				//Rotate
				sym::CQuaternion Yaw;
				sym::CQuaternion Pitch;
				Yaw.FromAxis(Camera.GetRotation().ToEuler().y - (float)DeltaMouseX / 50.0f, sym::CVector3(0, 1, 0));
				Pitch.FromAxis(Camera.GetRotation().ToEuler().x - (float)DeltaMouseY / 50.0f, sym::CVector3(1, 0, 0));

				Camera.SetRotation(Yaw * Pitch);

				//Translate back to distance between geom/camera
				Camera.Move(-CamPos.GetDistance(GeomPos));
			}
			//else just rotate around current position
			else
			{
				sym::CQuaternion Yaw;
				sym::CQuaternion Pitch;
				Yaw.FromAxis(Camera.GetRotation().ToEuler().y - (float)DeltaMouseX / 50.0f, sym::CVector3(0, 1, 0));
				Pitch.FromAxis(Camera.GetRotation().ToEuler().x - (float)DeltaMouseY / 50.0f, sym::CVector3(1, 0, 0));

				Camera.SetRotation(Yaw * Pitch);
			}
		}
	}
	else if(LeftMouse)
	{
		//TODO: Switch tool type

		if(CurrentGeom)
		{
			switch(CurrentGizmo.GetMode())
			{
				case GIZMO_TRANSLATE:
					//Move mode
					if(MousePickGizmo(X, Y) == GIZMO_X) CurrentTool = TOOL_MOVE_X;
					if(MousePickGizmo(X, Y) == GIZMO_Y) CurrentTool = TOOL_MOVE_Y;
					if(MousePickGizmo(X, Y) == GIZMO_Z) CurrentTool = TOOL_MOVE_Z;
					break;
				
				case GIZMO_SCALE_U:
					//Scale mode
					if(MousePickGizmo(X, Y) == GIZMO_X) CurrentTool = TOOL_SCALE_U;
					if(MousePickGizmo(X, Y) == GIZMO_Y) CurrentTool = TOOL_SCALE_U;
					if(MousePickGizmo(X, Y) == GIZMO_Z) CurrentTool = TOOL_SCALE_U;
					break;

				case GIZMO_SCALE_X:
					if(MousePickGizmo(X, Y) == GIZMO_X) CurrentTool = TOOL_SCALE_X;
					break;

				case GIZMO_SCALE_Y:
					if(MousePickGizmo(X, Y) == GIZMO_Y) CurrentTool = TOOL_SCALE_Y;
					break;

				case GIZMO_SCALE_Z:
					if(MousePickGizmo(X, Y) == GIZMO_Z) CurrentTool = TOOL_SCALE_Z;
					break;

				case GIZMO_ROTATE:
					//Rotate mode
					if(MousePickGizmo(X, Y) == GIZMO_X) CurrentTool = TOOL_ROTATE_X;
					if(MousePickGizmo(X, Y) == GIZMO_Y) CurrentTool = TOOL_ROTATE_Y;
					if(MousePickGizmo(X, Y) == GIZMO_Z) CurrentTool = TOOL_ROTATE_Z;
					break;

				default: break;
			}
		}
	}
	
	Update();
	event.Skip();
}

void GLViewport::OnRender( wxPaintEvent& evt )
{
    if(!IsShown()) return;
    wxGLCanvas::SetCurrent();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(Type == VIEW_PERSPECTIVE)
	{
		Setup3DViewport(0.0f, 0.0f, (float)GetWidth(), (float)GetHeight());

		//Render skybox
	    glPushMatrix();

	    //Get camera's inverse matrix
		sym::CMatrix4 RotMatrix = Camera.GetMatrix().GetInverse();

	    //Remove translation
	    RotMatrix.set(3, 0, 0);
	    RotMatrix.set(3, 1, 0);
	    RotMatrix.set(3, 2, 0);

	    //Apply matrix
	    glMultMatrixf(RotMatrix.GetAsFloatArray());

		//Disable depth testing
		glDisable(GL_DEPTH_TEST);

		//Draw skybox
		//Scene.Skybox.Render();

		//Enable depth testing
		glEnable(GL_DEPTH_TEST);

        //Pop matrix
		glPopMatrix();

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		//Light colours
		sym::CColour Ambient = Scene.GetAmbientLight();
		sym::CColour Sunlight = Scene.GetSunlight();
		sym::CVector3 SunPosition = Scene.GetSunPosition();
		GLfloat ambientLight[] = { (float)Ambient.r / 255.0f, (float)Ambient.g / 255.0f, (float)Ambient.b / 255.0f, 1.0f };
		GLfloat diffuseLight[] = { 0.6f, 0.6f, 0.6f, 1.0f };
		GLfloat specularLight[] = { (float)Sunlight.r / 255.0f, (float)Sunlight.g / 255.0f, (float)Sunlight.b / 255.0f, 1.0f };
		GLfloat position[] = { SunPosition.x, SunPosition.y, SunPosition.z, 1.0f };

		//Position lights
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	}
	else Setup2DViewport(0.0f, 0.0f, (float)GetWidth(), (float)GetHeight());

	//Apply camera
	Camera.ApplyMatrix();

	//Draw grid
	switch(Type)
	{
		case VIEW_PERSPECTIVE:
			Grid.Render(Type, 0, 0, 100);
			break;
		
		case VIEW_FRONT:
			Grid.Render(Type, Camera.GetPosition().x, Camera.GetPosition().y, Zoom * ((float)GetWidth() / 2) / ((float)GetWidth() / 2));
			break;

		case VIEW_TOP:
			Grid.Render(Type, Camera.GetPosition().x, Camera.GetPosition().z, Zoom * ((float)GetWidth() / 2) / ((float)GetWidth() / 2));
			break;

		case VIEW_LEFT:
			Grid.Render(Type, Camera.GetPosition().y, Camera.GetPosition().z, Zoom * ((float)GetWidth() / 2) / ((float)GetWidth() / 2));
			break;

		default: break;
	}

	//Render geometry in render list
    for(std::map<std::string, sym::CGeometry*> ::iterator g = Scene.Geometry.begin(); g != Scene.Geometry.end(); g++)
    {
        if(Style == RENDER_SOLID) (*g).second->Render();
		(*g).second->RenderWireframe(sym::CColour(255, 255, 255));
    }

	if(CurrentGeom)
	{
		//Render current geometry overlay
		RenderCurrentGeomOverlay();

		//Render gizmo on current geom
		CurrentGizmo.SetMatrix(CurrentGeom->GetMatrix());
		CurrentGizmo.Render(GIZMO_RENDER);
	}

	SwapBuffers();

	glDisable(GL_CULL_FACE);
}

void GLViewport::SetViewType(int ViewType)
{
	Type = ViewType;
	Zoom = 100.0f;
	CurrentGizmo.SetLength(10.0f);

	Camera.SetPosition(sym::CVector3(0.0f, 0.0f, 0.0f));
	Camera.SetRotation(sym::CQuaternion(sym::CQuaternion()));

	switch(Type)
	{
		case VIEW_TOP:
			Camera.Pitch(-90.0f);
			break;

		case VIEW_LEFT:
			Camera.Yaw(-90.0f);
			break;

		case VIEW_PERSPECTIVE:
		{
			Camera.SetPosition(sym::CVector3(100.0f, 70.0f, 100.0f));
			sym::CQuaternion Yaw, Pitch;
			Yaw.FromAxis(45.0f, sym::CVector3(0, 1, 0));
			Pitch.FromAxis(270.0f, sym::CVector3(1, 0, 0));
			Camera.SetRotation(Yaw * Pitch);
			break;
		}

		default:
			break;
	}

	GetParent()->Refresh();
}

void GLViewport::Setup3DViewport(float LeftX, float LeftY, float BottomX, float BottomY)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glViewport(LeftX, LeftY, BottomX - LeftX, BottomY - LeftY);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float ratio_w_h = (BottomX - LeftX)/(BottomY - LeftY);
    gluPerspective(45, ratio_w_h, 0.1f, 10000.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
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

    glOrtho (-Zoom, Zoom, -Zoom * BottomY / BottomX, Zoom * BottomY / BottomX, -10000, 10000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int GLViewport::MousePickGizmo(int x, int y)
{
	wxGLCanvas::SetCurrent();

	//Setup viewport
	if(Type == VIEW_PERSPECTIVE) Setup3DViewport(0.0f, 0.0f, (float)GetWidth(), (float)GetHeight());
	else Setup2DViewport(0.0f, 0.0f, (float)GetWidth(), (float)GetHeight());

	//Apply camera
	Camera.ApplyMatrix();

	//Turn off fog, lighting, textures
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_FOG);
	glDisable(GL_LIGHTING);

	//Render gizmo
	glClear(GL_COLOR_BUFFER_BIT);
	CurrentGizmo.Render(GIZMO_PICKING);

	GLint Viewport[4];
	glGetIntegerv(GL_VIEWPORT, Viewport);

	unsigned char Pixel[3];

	glReadBuffer(GL_BACK);
	glReadPixels(x, Viewport[3] - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, Pixel);

	if((int)Pixel[1] == 101 && (int)Pixel[2] == 101) return (int)Pixel[0];

	return -1;
}

sym::CVector3 GLViewport::ScreenToWorld(int screenx, int screeny, int screenz)
{
	//Where the values will be stored
	int win[4]; 
	GLint viewport[4]; 

	//Retrieves the viewport and stores it in the variable
	glGetIntegerv(GL_VIEWPORT, viewport); 

	//Where the 16 doubles of the matrix are to be stored
	GLdouble modelview[16]; 

	//Retrieve the matrix
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview); 

	GLdouble projection[16]; 
	glGetDoublev(GL_PROJECTION_MATRIX, projection);

	GLdouble winX = 0.0;
	GLdouble winY = 0.0;
	GLdouble winZ = 0.0; //The coordinates to pass in

	winX = dFloat(screenx); //Store the x cord
	winY = dFloat(screeny); //Store the y cord
	winZ = dFloat(screenz); //Store the Z cord

	//Now windows coordinates start with (0,0) being at the top left 
	//whereas OpenGL cords start lower left so we have to do this to convert it: 
	//Remember we got viewport value before 
	winY = (dFloat)viewport[3] - winY; 

	GLdouble objx;
	GLdouble objy;
	GLdouble objz;

	// use the real GL view port
	glGetIntegerv(GL_VIEWPORT, viewport); 
	gluUnProject (winX, winY, winZ, modelview, projection, viewport, &objx, &objy, &objz);

	return sym::CVector3(float(objx), float(objy), float(objz));
}