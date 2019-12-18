#ifndef __SYM_CAMERA
#define __SYM_CAMERA

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include "SYM_Maths.h"
#include "SYM_Geometry.h"
#include "SYM_Placeable.h"
#include "SYM_PhysicsObject.h"

namespace sym
{
	class CCamera : public CGeometry
	{
		public:
			//Initialise camera
			void Init();

			//Apply matrix
			void ApplyMatrix();

			//Update camera()
			void Update();

			//Camera shake
			void Shake(CVector3 Direction, float Strength, float CalmTime);

			//Frustum
			CFrustum Frustum;

			//Test if a vector is in front or behind the camera
			bool VectorInFront(CVector3 Vector);

		private:
			//Camera shake
			CVector3 ShakeDirection;
			float ShakeStrength;
			float ShakeSin;
			CTimer ShakeTimer;
			bool IsShaking;
	};


} //Namespace

#endif
