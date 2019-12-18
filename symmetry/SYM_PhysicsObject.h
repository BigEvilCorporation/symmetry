#ifndef __SYM_PHYSICSOBJECT
#define __SYM_PHYSICSOBJECT

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include "SYM_Placeable.h"
#include "SYM_Physics.h"

namespace sym
{
	const int PHYSICS_OBJ_BOX		= 0;
	const int PHYSICS_OBJ_SPHERE	= 1;
	const int PHYSICS_OBJ_CAPSULE	= 2;
	const int PHYSICS_OBJ_CUSTOM	= 3;
	const int LEVEL_COLLISION		= 4;
	const int NO_PHYSICS			= 5;

    class CPhysicsBody;

	class CPhysicsObject : public CPlaceable, public CPhysicsBody
    {
        public:
            CPhysicsObject();
            ~CPhysicsObject();

            //Transformation - relative
			void Move(float Distance);
			void Strafe(float Distance);
			void Fly(float Distance);
			void Pitch(float Degrees);
			void Yaw(float Degrees);
			void Roll(float Degrees);
			void RotateAxis(float Degrees, CVector3 Axis);

			//Transformation - absolute
			void SetPosition(CVector3 Pos);
			void SetRotation(CQuaternion Rot);
			void LookAt(CVector3 Forward, CVector3 Target, float Factor);

			//Physics type
			int PhysicsType;
    };

}

#endif // __SYM_PHYSICSOBJECT
