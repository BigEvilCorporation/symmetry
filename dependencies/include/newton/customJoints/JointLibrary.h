#ifndef NEWTON_CUSTOM_JOINTS_H_INCLUDED_
#define NEWTON_CUSTOM_JOINTS_H_INCLUDED_

#include "Newton.h"


#ifndef _MSC_VER
	#ifndef _NEWTON_USE_LIB
		#define _NEWTON_USE_LIB
	#endif
#endif



#ifdef _NEWTON_USE_LIB
	#define JOINTLIBRARY_API
#else
	#ifdef JOINTLIBRARY_LIB
		#define JOINTLIBRARY_API 
	#else
		#ifdef JOINTLIBRARY_EXPORTS
			#define JOINTLIBRARY_API __declspec(dllexport)
		#else
			#define JOINTLIBRARY_API __declspec(dllimport)
		#endif
	#endif
#endif


#ifdef __cplusplus 
extern "C" {
#endif

	typedef struct CustomJoint{} CustomJoint;

	typedef int (*PlayerCanPuchThisBodyCalback) (CustomJoint *me, const NewtonBody* hitBody);

	// Generic 6 degree of Freedom Joint
	JOINTLIBRARY_API CustomJoint *CreateCustomJoint6DOF (const dFloat* pinsAndPivoChildFrame, const dFloat* pinsAndPivoParentFrame, const NewtonBody* child, const NewtonBody* parent);
	JOINTLIBRARY_API void CustomJoint6DOF_SetLinearLimits (CustomJoint* customJoint6DOF, const dFloat* minLinearLimits, const dFloat* maxLinearLimits);
	JOINTLIBRARY_API void CustomJoint6DOF_SetAngularLimits (CustomJoint* customJoint6DOF, const dFloat* minAngularLimits, const dFloat* maxAngularLimits);
	JOINTLIBRARY_API void CustomJoint6DOF_GetLinearLimits (CustomJoint* customJoint6DOF, dFloat* minLinearLimits, dFloat* maxLinearLimits);
	JOINTLIBRARY_API void CustomJoint6DOF_GetAngularLimits (CustomJoint* customJoint6DOF, dFloat* minAngularLimits, dFloat* maxAngularLimits);
	JOINTLIBRARY_API void CustomJoint6DOF_SetReverseUniversal (CustomJoint* customJoint6DOF, int order);


	// player controller functions
	JOINTLIBRARY_API CustomJoint *CreateCustomPlayerController (const dFloat* pins, const NewtonBody* player, dFloat maxStairStepFactor);
	JOINTLIBRARY_API void CustomPlayerControllerSetPushActorCallback (CustomJoint* playerController, PlayerCanPuchThisBodyCalback callback);
	JOINTLIBRARY_API void CustomPlayerControllerSetVelocity (CustomJoint* playerController, dFloat forwardSpeed, dFloat sideSpeed, dFloat heading);
	JOINTLIBRARY_API void CustomPlayerControllerSetMaxSlope (CustomJoint* playerController, dFloat maxSlopeAngleIndRadian);
	JOINTLIBRARY_API dFloat CustomPlayerControllerGetMaxSlope (CustomJoint* playerController);
	JOINTLIBRARY_API const NewtonCollision* CustomPlayerControllerGetVerticalSensorShape (CustomJoint* playerController);
	JOINTLIBRARY_API const NewtonCollision* CustomPlayerControllerGetHorizontalSensorShape (CustomJoint* playerController);
	JOINTLIBRARY_API const NewtonCollision* CustomPlayerControllerGetDynamicsSensorShape (CustomJoint* playerController);

	// Multi rigid BodyCar controller functions
	JOINTLIBRARY_API CustomJoint *CreateCustomMultiBodyVehicle (const dFloat* frontDir, const dFloat* upDir, const NewtonBody* carBody);
	JOINTLIBRARY_API int CustomMultiBodyVehicleAddTire (CustomJoint *car, const void* userData, const dFloat* localPosition, 
													    dFloat mass, dFloat radius, dFloat width,
													    dFloat suspensionLength, dFloat springConst, dFloat springDamper);
	JOINTLIBRARY_API int CustomMultiBodyVehicleAddSlipDifferencial (CustomJoint *car, int leftTireIndex, int rightToreIndex, dFloat maxFriction);
	JOINTLIBRARY_API int CustomMultiBodyVehicleGetTiresCount(CustomJoint *car);
	JOINTLIBRARY_API const NewtonBody* CustomMultiBodyVehicleGetTireBody(CustomJoint *car, int tireIndex);
	JOINTLIBRARY_API dFloat CustomMultiBodyVehicleGetSpeed(CustomJoint *car);
	JOINTLIBRARY_API dFloat CustomMultiBodyVehicleGetTireSteerAngle (CustomJoint *car, int index);
	JOINTLIBRARY_API void CustomMultiBodyVehicleApplyTorque (CustomJoint *car, int tireIndex, dFloat torque);
	JOINTLIBRARY_API void CustomMultiBodyVehicleApplySteering (CustomJoint *car, int tireIndex, dFloat angle);
	JOINTLIBRARY_API void CustomMultiBodyVehicleApplyBrake (CustomJoint *car, int tireIndex, dFloat brakeTorque);
	JOINTLIBRARY_API void CustomMultiBodyVehicleApplyTireRollingDrag (CustomJoint *car, int index, dFloat angularDampingCoef);



	// genetic joint functions
	JOINTLIBRARY_API int CustomGetJointID (CustomJoint *joint);
	JOINTLIBRARY_API void CustomSetJointID (CustomJoint *joint, int rttI);
	JOINTLIBRARY_API const NewtonBody* CustomGetBody0 (CustomJoint *joint);
	JOINTLIBRARY_API const NewtonBody* CustomGetBody1 (CustomJoint *joint);
	JOINTLIBRARY_API int CustomGetBodiesCollisionState (CustomJoint *joint);
	JOINTLIBRARY_API void CustomSetBodiesCollisionState (CustomJoint *joint, int state);
	JOINTLIBRARY_API void CustomDestroyJoint(CustomJoint *joint);

#ifdef __cplusplus 
}
#endif


#endif