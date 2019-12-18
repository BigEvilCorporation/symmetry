#include "Gizmo.h"

Gizmo::Gizmo()
{
	ColourX = sym::CColour(255, 0, 0);
	ColourY = sym::CColour(0, 0, 255);
	ColourZ = sym::CColour(0, 255, 0);
	Length = 10.0f;
	Mode = GIZMO_TRANSLATE;
}

void Gizmo::Render(int RenderMode)
{
	//Get axis
	sym::CVector3 AxisX(Length, 0.0f, 0.0f);
	sym::CVector3 AxisY(0.0f, Length, 0.0f);
	sym::CVector3 AxisZ(0.0f, 0.0f, Length);

	glLineWidth(6.0f);
	glDisable(GL_DEPTH_TEST);

	switch(Mode)
	{
		case GIZMO_TRANSLATE:

			glTranslatef(GetMatrix().GetTranslation().x, GetMatrix().GetTranslation().y, GetMatrix().GetTranslation().z);

			glBegin(GL_LINES);
			
				if(RenderMode == GIZMO_RENDER) glColor3ub(ColourX.r, ColourX.g, ColourX.b);
				else glColor3ub(GIZMO_X, 101, 101);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glVertex3f(AxisX.x, AxisX.y, AxisX.z);

				if(RenderMode == GIZMO_RENDER) glColor3ub(ColourY.r, ColourY.g, ColourY.b);
				else glColor3ub(GIZMO_Y, 101, 101);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glVertex3f(AxisY.x, AxisY.y, AxisY.z);

				if(RenderMode == GIZMO_RENDER) glColor3ub(ColourZ.r, ColourZ.g, ColourZ.b);
				else glColor3ub(GIZMO_Z, 101, 101);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glVertex3f(AxisZ.x, AxisZ.y, AxisZ.z);

			glEnd();
			break;

		case GIZMO_SCALE_U:

			glMultMatrixf(GetMatrix().GetAsFloatArray());

			glBegin(GL_LINES);
			
				if(RenderMode == GIZMO_RENDER) glColor3ub(ColourX.r, ColourX.g, ColourX.b);
				else glColor3ub(GIZMO_X, 101, 101);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glVertex3f(AxisX.x, AxisX.y, AxisX.z);

				if(RenderMode == GIZMO_RENDER) glColor3ub(ColourY.r, ColourY.g, ColourY.b);
				else glColor3ub(GIZMO_Y, 101, 101);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glVertex3f(AxisY.x, AxisY.y, AxisY.z);

				if(RenderMode == GIZMO_RENDER) glColor3ub(ColourZ.r, ColourZ.g, ColourZ.b);
				else glColor3ub(GIZMO_Z, 101, 101);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glVertex3f(AxisZ.x, AxisZ.y, AxisZ.z);

			glEnd();
			break;

		case GIZMO_SCALE_X:

			glMultMatrixf(GetMatrix().GetAsFloatArray());

			glBegin(GL_LINES);
			
				if(RenderMode == GIZMO_RENDER) glColor3ub(ColourX.r, ColourX.g, ColourX.b);
				else glColor3ub(GIZMO_X, 101, 101);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glVertex3f(AxisX.x, AxisX.y, AxisX.z);

			glEnd();
			break;

		case GIZMO_SCALE_Y:

			glMultMatrixf(GetMatrix().GetAsFloatArray());

			glBegin(GL_LINES);

				if(RenderMode == GIZMO_RENDER) glColor3ub(ColourY.r, ColourY.g, ColourY.b);
				else glColor3ub(GIZMO_Y, 101, 101);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glVertex3f(AxisY.x, AxisY.y, AxisY.z);

			glEnd();
			break;

		case GIZMO_SCALE_Z:

			glMultMatrixf(GetMatrix().GetAsFloatArray());

			glBegin(GL_LINES);

				if(RenderMode == GIZMO_RENDER) glColor3ub(ColourZ.r, ColourZ.g, ColourZ.b);
				else glColor3ub(GIZMO_Z, 101, 101);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glVertex3f(AxisZ.x, AxisZ.y, AxisZ.z);

			glEnd();
			break;

		case GIZMO_ROTATE:

			glTranslatef(GetMatrix().GetTranslation().x, GetMatrix().GetTranslation().y, GetMatrix().GetTranslation().z);
			
			if(RenderMode == GIZMO_RENDER) glColor3ub(ColourX.r, ColourX.g, ColourX.b);
			else glColor3ub(GIZMO_X, 101, 101);
			DrawCircle(Length * 1.5f, GIZMO_X);

			if(RenderMode == GIZMO_RENDER) glColor3ub(ColourY.r, ColourY.g, ColourY.b);
			else glColor3ub(GIZMO_Y, 101, 101);
			DrawCircle(Length * 1.5f, GIZMO_Y);

			if(RenderMode == GIZMO_RENDER) glColor3ub(ColourZ.r, ColourZ.g, ColourZ.b);
			else glColor3ub(GIZMO_Z, 101, 101);
			DrawCircle(Length * 1.5f, GIZMO_Z);
			break;

		default: break;
	}

	glEnable(GL_DEPTH_TEST);
	glLineWidth(1.0f);
	glColor3ub(0, 0, 0);
}

void Gizmo::DrawCircle(float Radius, int Axis)
{
	glBegin(GL_LINE_STRIP);	

		for(float Angle = 0.0f; Angle <= (2.0f * 3.14159); Angle += 0.01f)
		{		
			float X = Radius * sin(Angle);
			float Y = Radius * cos(Angle);

			switch(Axis)
			{
				case GIZMO_X:
					glVertex3f(X, Y, 0);
					break;

				case GIZMO_Y:
					glVertex3f(0, X, Y);
					break;

				case GIZMO_Z:
					glVertex3f(X, 0, Y);
					break;

				default: break;
			}
		}

	glEnd();
}