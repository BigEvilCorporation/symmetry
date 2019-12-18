#ifndef __GL_GRID
#define __GL_GRID

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <cmath>

#include "../../symmetry/SYM_Engine.h"

class GLGrid
{
	public:
		GLGrid();

		void Render(int View, float CentreX, float CentreY, float HalfWidth);
		void SetResolution(float r) { Resolution = r; }
		const float GetResolution() { return Resolution; }
		void SetMajorColour(sym::CColour Colour) { MajorColour = Colour; }
		void SetMinorColour(sym::CColour Colour) { MinorColour = Colour; }
		void SetMajorWidth(float Width) { MajorWidth = Width; }
		void SetMinorWidth(float Width) { MinorWidth = Width; }

	private:
		float Resolution, MajorWidth, MinorWidth;
		sym::CColour MajorColour, MinorColour;

		GLuint DisplayList;
};

#endif