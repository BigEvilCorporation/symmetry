#ifndef __SYM_PLACEABLE
#define __SYM_PLACEABLE

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include <string>

#include "SYM_Maths.h"
#include "SYM_Vector.h"
#include "SYM_Matrix.h"
#include "SYM_Quaternion.h"

namespace sym
{
	class CPlaceable
    {
        public:
            CPlaceable();

            //Object name
            std::string Name;

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
			//void SetQuaternion(CQuaternion Quaternion);
			void SetMatrix(CMatrix4 Mat);
			void LookAt(CVector3 Forward, CVector3 Target, float Factor);

			//Get values
			CVector3 GetPosition() { return Position; }
			CQuaternion GetRotation() { return Quat; }
			//CQuaternion GetQuaternion() { return Quat; }
			CMatrix4 GetMatrix() { return Matrix; }
			CVector3 GetForward();
			CVector3 GetRight();
			CVector3 GetUp();

			virtual void HasMoved() {};

        protected:
            //Transformation
			CVector3 Position;
			//CVector3 Euler;
			CQuaternion Quat;

			//Matrix
			CMatrix4 Matrix;
    };

} //Namespace



#endif // SYM_PLACEABLE_H_INCLUDED
