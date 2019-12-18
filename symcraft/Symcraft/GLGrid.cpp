#include "GLGrid.h"

GLGrid::GLGrid()
{
	MajorColour = sym::CColour(50, 50, 50);
	MinorColour = sym::CColour(10, 10, 10);
	Resolution = 5.0f;
	MajorWidth = 2.0f;
	MinorWidth = 1.0f;
	DisplayList = 0;
}

void GLGrid::Render(int View, float CentreX, float CentreY, float HalfWidth)
{
	int Spacing[2] = {Resolution, Resolution * 8};
	float Thickness[2] = { 1.0f, 2.0f };
	sym::CColour Colours[2] = { MinorColour, MajorColour };

	for(int Grid = 0; Grid < 2; Grid++)
	{
		glColor3ub(Colours[Grid].r, Colours[Grid].g, Colours[Grid].b);
		glLineWidth(Thickness[Grid]);

		HalfWidth = sym::Maths::RoundToNearest(HalfWidth, Spacing[Grid]) + Spacing[Grid];

		glBegin(GL_LINES);

		float OffsetX = CentreX - std::fmod(CentreX, Spacing[Grid]);
		float OffsetY = CentreY - std::fmod(CentreY, Spacing[Grid]);

		switch(View)
		{
			//Top
			case 0:
				for(float x = OffsetX - HalfWidth; x < OffsetX + HalfWidth; x += Spacing[Grid])
				{
					glVertex3f(x, 0, CentreY - HalfWidth);
					glVertex3f(x, 0, CentreY + HalfWidth);
				}

				for(float y = OffsetY - HalfWidth; y < OffsetY + HalfWidth; y += Spacing[Grid])
				{
					glVertex3f(CentreX - HalfWidth, 0, y);
					glVertex3f(CentreX + HalfWidth, 0, y);
				}
				break;
				
			//Front
			case 1:
				for(float x = OffsetX - HalfWidth; x < OffsetX + HalfWidth; x += Spacing[Grid])
				{
					glVertex3f(x, CentreY - HalfWidth, 0);
					glVertex3f(x, CentreY + HalfWidth, 0);
				}

				for(float y = OffsetY - HalfWidth; y < OffsetY + HalfWidth; y += Spacing[Grid])
				{
					glVertex3f(CentreX - HalfWidth, y, 0);
					glVertex3f(CentreX + HalfWidth, y, 0);
				}
				break;
			
			//Left
			case 2:
				for(float x = OffsetX - HalfWidth; x < OffsetX + HalfWidth; x += Spacing[Grid])
				{
					glVertex3f(0, x, CentreY - HalfWidth);
					glVertex3f(0, x, CentreY + HalfWidth);
				}

				for(float y = OffsetY - HalfWidth; y < OffsetY + HalfWidth; y += Spacing[Grid])
				{
					glVertex3f(0, CentreX - HalfWidth, y);
					glVertex3f(0, CentreX + HalfWidth, y);
				}
				break;

			//Perspective
			default:
				for(float x = OffsetX - HalfWidth; x < OffsetX + HalfWidth + Spacing[Grid]; x += Spacing[Grid])
				{
					glVertex3f(x, 0, CentreY - HalfWidth);
					glVertex3f(x, 0, CentreY + HalfWidth);
				}

				for(float y = OffsetY - HalfWidth; y < OffsetY + HalfWidth + Spacing[Grid]; y += Spacing[Grid])
				{
					glVertex3f(CentreX - HalfWidth, 0, y);
					glVertex3f(CentreX + HalfWidth, 0, y);
				}
				break;
		}

		glEnd();
	}

	glLineWidth(1.0f);
	glColor3ub(0, 0, 0);
}
