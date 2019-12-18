#ifndef __SYM_CHARACTER
#define __SYM_CHARACTER

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include "NewtonUtils/NewtonCharacterController.h"

#include "SYM_Model.h"
#include "SYM_Maths.h"
#include "SYM_Timer.h"

namespace sym
{
	class CCharacterController : public CModel
	{
		public:
			bool Create(std::string ModelFilename);

			void SetMovementVector(CVector3 MovVec);
			void SetHeading(float Degrees);
			void Jump(float Height);

			void SetUpVector(CVector3 UpVector);
			void ReleaseUpVector();

			void ApplyControlJoint();
			void ReleaseControlJoint();

			void SetMovement(float Forward, float Side, float Heading);

			void ApplyInverseDynamics();
			void ReleaseInverseDynamics();

			float FindFloor(CVector3 Centre, float MaxDist);

		private:
			//Newton callbacks
			static void OnApplyForceAndTorque(const NewtonBody* Body, float timestep, int threadIndex);
			static void OnSetTransform(const NewtonBody* body, const float* matrix, int threadIndex);
			static float OnRayCastHit(const NewtonBody* Body, const float* Normal, int CollisionID, void* UserData, float IntersetParam);
			static void OnLevelCollision(const NewtonJoint* contact, float timestep, int threadIndex);
			void CharLevelCollision(void* contact, NewtonMaterial *material);

			//Up vector joint
			NewtonJoint* UpVectorJoint;

			//Newton character controller joint
			NewtonPlayerController *ControlJoint;

			float RunSpeed;
			float TurnSpeed;
			float JumpHeight;

			float TargetHeading;

			bool UseInverseDynamics;

			CVector3 Movement;
	};
}

#endif