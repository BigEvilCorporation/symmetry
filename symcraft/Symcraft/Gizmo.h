#ifndef __GIZMO
#define __GIZMO

#include "../../symmetry/SYM_Engine.h"
#include "../../symmetry/SYM_Placeable.h"

const int GIZMO_TRANSLATE	= 0;
const int GIZMO_ROTATE		= 1;
const int GIZMO_SCALE_U		= 2;
const int GIZMO_SCALE_X		= 3;
const int GIZMO_SCALE_Y		= 4;
const int GIZMO_SCALE_Z		= 5;
const int SELECT_MODE		= 6;

const int GIZMO_RENDER	= 0;
const int GIZMO_PICKING = 1;

const int GIZMO_X = 1;
const int GIZMO_Y = 2;
const int GIZMO_Z = 3;

class Gizmo : public sym::CPlaceable
{
	public:
		Gizmo();
		void Render(int RenderMode);
		void SetMode(int M) { Mode = M; }
		int GetMode() { return Mode; }
		void SetLength(float L) { Length = L; }
		void SetColourX(sym::CColour Colour) { ColourX = Colour; }
		void SetColourY(sym::CColour Colour) { ColourY = Colour; }
		void SetColourZ(sym::CColour Colour) { ColourZ = Colour; }

	private:
		float Length;
		sym::CColour ColourX, ColourY, ColourZ;
		int Mode;

		void DrawCircle(float Radius, int Axis);
};

#endif