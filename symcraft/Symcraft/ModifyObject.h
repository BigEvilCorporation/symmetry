#ifndef __MODIFY_OBJECT
#define __MODIFY_OBJECT

#include <vector>

#include "Gizmo.h"
#include "../../symmetry/SYM_Engine.h"

const int TOOL_MOVE_X = 1;
const int TOOL_MOVE_Y = 2;
const int TOOL_MOVE_Z = 3;

const int TOOL_ROTATE_X = 4;
const int TOOL_ROTATE_Y = 5;
const int TOOL_ROTATE_Z = 6;

const int TOOL_SCALE_X = 7;
const int TOOL_SCALE_Y = 8;
const int TOOL_SCALE_Z = 9;
const int TOOL_SCALE_U = 10;

extern sym::CGeometry *CurrentGeom;
extern int CurrentTool;
extern int TranslateStep;
extern int RotateStep;

void RenderCurrentGeomOverlay();
void DoModify(float x, float y);

///////////////////////////////////////////////////////
// UNDO
class UndoOperation
{
	public:
		//Pointer to the geometry the operation was performed on
		sym::CGeometry *Geom;

		//'Backup' of the geometry's state
		sym::CGeometry GeomState;
};

void PushUndoOperation();
void Undo();

#endif