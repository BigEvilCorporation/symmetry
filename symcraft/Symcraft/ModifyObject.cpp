#include "ModifyObject.h"
#include "Scene.h"

sym::CGeometry *CurrentGeom = 0;
int CurrentTool = 0;
int TranslateStep = 4;
int RotateStep = 5;

std::vector<UndoOperation> UndoStack;

void RenderCurrentGeomOverlay()
{
	if(CurrentGeom)
	{
		CurrentGeom->RenderCollision(sym::CColour(255, 0, 0));
		CurrentGeom->RenderWireframe(sym::CColour(0, 255, 0));
	}
}

void DoModify(float x, float y)
{
	if(!CurrentGeom) return;

	sym::CVector3 Pos = CurrentGeom->GetPosition();
	sym::CQuaternion Rot = CurrentGeom->GetRotation();
	sym::CVector3 Dim = CurrentGeom->GetDimentions();

	float Scale = 0.0f;

	//CurrentOperation.Geom = CurrentGeom;

	switch(CurrentTool)
	{
		case TOOL_MOVE_X:
			CurrentGeom->SetPosition(Pos + sym::CVector3(x * TranslateStep, 0.0f, 0.0f));
			//CurrentOperation.OperationType = TOOL_MOVE_X;
			//CurrentOperation.VectorValue += sym::CVector3(x * TranslateStep, 0.0f, 0.0f);
			break;

		case TOOL_MOVE_Y:
			CurrentGeom->SetPosition(Pos + sym::CVector3(0.0f, y * TranslateStep, 0.0f));
			//CurrentOperation.OperationType = TOOL_MOVE_Y;
			//CurrentOperation.VectorValue += sym::CVector3(0.0f, y * TranslateStep, 0.0f);
			break;

		case TOOL_MOVE_Z:
			CurrentGeom->SetPosition(Pos + sym::CVector3(0.0f, 0.0f, -y * TranslateStep));
			//CurrentOperation.OperationType = TOOL_MOVE_Z;
			//CurrentOperation.VectorValue += sym::CVector3(0.0f, 0.0f, -y * TranslateStep);
			break;

		case TOOL_ROTATE_X:
			CurrentGeom->SetPosition(sym::CVector3(0, 0, 0));
			CurrentGeom->RotateAxis(x * RotateStep, sym::CVector3(0, 0, 1));
			CurrentGeom->SetPosition(Pos);

			//CurrentOperation.OperationType = TOOL_ROTATE_X;
			//CurrentOperation.FloatValue += x * RotateStep;
			break;

		case TOOL_ROTATE_Y:
			CurrentGeom->SetPosition(sym::CVector3(0, 0, 0));
			CurrentGeom->RotateAxis(y * RotateStep, sym::CVector3(1, 0, 0));
			CurrentGeom->SetPosition(Pos);

			//CurrentOperation.OperationType = TOOL_ROTATE_Y;
			//CurrentOperation.FloatValue += y * RotateStep;
			break;

		case TOOL_ROTATE_Z:
			CurrentGeom->SetPosition(sym::CVector3(0, 0, 0));
			CurrentGeom->RotateAxis(x * RotateStep, sym::CVector3(0, 1, 0));
			CurrentGeom->SetPosition(Pos);

			//CurrentOperation.OperationType = TOOL_ROTATE_Z;
			//CurrentOperation.FloatValue += x * RotateStep;
			break;

		case TOOL_SCALE_X:
			if(Dim.x > 0.0f && x != 0.0f)
			{
				Scale = 1.0f + ((TranslateStep/Dim.x) * x);
				if(Scale > 0.0f) CurrentGeom->Scale(sym::CVector3(Scale, 1.0f, 1.0f));

				//CurrentOperation.OperationType = TOOL_SCALE_X;
				//CurrentOperation.FloatValues.push_back(Scale);
			}
			break;

		case TOOL_SCALE_Y:
			if(Dim.y > 0.0f && y != 0.0f)
			{
				Scale = 1.0f + ((TranslateStep/Dim.y) * y);
				if(Scale > 0.0f) CurrentGeom->Scale(sym::CVector3(1.0f, Scale, 1.0f));

				//CurrentOperation.OperationType = TOOL_SCALE_Y;
				//CurrentOperation.FloatValues.push_back(Scale);
				PushUndoOperation();
			}
			break;

		case TOOL_SCALE_Z:
			if(Dim.z > 0 && y != 0.0f)
			{
				Scale = 1.0f + ((TranslateStep/Dim.y) * y);
				if(Scale > 0.0f) CurrentGeom->Scale(sym::CVector3(1.0f, 1.0f, Scale));

				//CurrentOperation.OperationType = TOOL_SCALE_Z;
				//CurrentOperation.FloatValues.push_back(Scale);
			}
			break;

		case TOOL_SCALE_U:
			{
				if(Dim.x > 0.0f && Dim.y > 0.0f && Dim.z > 0.0f)
				{
					float ScaleX = 1.0f, ScaleY = 1.0f, ScaleZ = 1.0f;
					if(x != 0.0f)
					{
						ScaleX = 1.0f + ((TranslateStep/Dim.x) * x);
						ScaleY = 1.0f + ((TranslateStep/Dim.y) * x);
						ScaleZ = 1.0f + ((TranslateStep/Dim.z) * x);
					}
					
					CurrentGeom->Scale(sym::CVector3(ScaleX, ScaleY, ScaleZ));

					//CurrentOperation.OperationType = TOOL_SCALE_U;
					//CurrentOperation.VectorValues.push_back(sym::CVector3(ScaleX, ScaleY, ScaleZ));
				}

				break;
			}
			break;

		default: break;
	}
}

void PushUndoOperation()
{
	/*
	if(CurrentGeom)
	{
		UndoOperation Op;
		Op.Geom = CurrentGeom;
		Op.GeomState = *CurrentGeom;
		UndoStack.push_back(Op);
	}
	*/
}

void Undo()
{
	/*
	if(UndoStack.size() > 0)
	{
		UndoOperation Op = UndoStack[UndoStack.size() - 1];

		if(Op.Geom)
		{
			*Op.Geom = Op.GeomState;
			CurrentGeom = Op.Geom;
		}
		else
		{
			//Geom doesn't exist, it was a delete operation
			Scene.AddGeometry(&Op.GeomState);
			CurrentGeom = &Op.GeomState;
		}
		
		UndoStack.pop_back();
	}
	*/
}
