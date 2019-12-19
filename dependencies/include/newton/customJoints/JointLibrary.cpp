//////////////////////////////////////////////////////////////////////
#include "CustomJointLibraryStdAfx.h"
#include "JointLibrary.h"
#include "Custom6DOF.h"
#include "NewtonCustomJoint.h"
#include "CustomPlayerController.h"
#include "CustomMultiBodyVehicle.h"


// Generic 6 degree of Freedom Joint
CustomJoint *CreateCustomJoint6DOF (const dFloat* pinsAndPivoChildFrame, const dFloat* pinsAndPivoParentFrame, const NewtonBody* child, const NewtonBody* parent)
{
	return (CustomJoint *) new Custom6DOF (*(dMatrix*) pinsAndPivoChildFrame, *(dMatrix*) pinsAndPivoParentFrame, child, parent);
}


void CustomJoint6DOF_SetLinearLimits (CustomJoint* customJoint6DOF, const dFloat* minLinearLimits, const dFloat* maxLinearLimits)
{
	((Custom6DOF*)customJoint6DOF)->SetLinearLimits (*(dVector*)minLinearLimits, *(dVector*)maxLinearLimits); 
}

void CustomJoint6DOF_SetAngularLimits (CustomJoint* customJoint6DOF, const dFloat* minAngularLimits, const dFloat* maxAngularLimits)
{
	((Custom6DOF*)customJoint6DOF)->SetAngularLimits(*(dVector*)minAngularLimits, *(dVector*)maxAngularLimits); 
}

void CustomJoint6DOF_GetLinearLimits (CustomJoint* customJoint6DOF, dFloat* minLinearLimits, dFloat* maxLinearLimits)
{
	((Custom6DOF*)customJoint6DOF)->GetLinearLimits (*(dVector*)minLinearLimits, *(dVector*)maxLinearLimits); 
}

void CustomJoint6DOF_GetAngularLimits (CustomJoint* customJoint6DOF, dFloat* minAngularLimits, dFloat* maxAngularLimits)
{
	((Custom6DOF*)customJoint6DOF)->GetAngularLimits(*(dVector*)minAngularLimits, *(dVector*)maxAngularLimits); 
}

void CustomJoint6DOF_SetReverseUniversal (CustomJoint* customJoint6DOF, int order)
{
	((Custom6DOF*)customJoint6DOF)->SetReverserUniversal(order);
}




// player controller functions 
class PlayerController: public CustomPlayerController 
{
	public: 

	typedef int (*PlayerCanPuchBody) (CustomJoint *me, const NewtonBody* hitBody);

	PlayerController (const dVector& pin, const NewtonBody* child, dFloat maxStairStepFactor)
		:CustomPlayerController (pin, child, maxStairStepFactor)
	{
		m_canPuchOtherBodies = CanPushThisBodyCallback;
	}

	virtual bool CanPushBody (const NewtonBody* hitBody) const 
	{
		if (m_canPuchOtherBodies) {
			return m_canPuchOtherBodies ((CustomJoint *)this, hitBody) ? true : false;
		} 
		return true;
	}


	static int CanPushThisBodyCallback(CustomJoint *me, const NewtonBody* hitBody) 
	{
		return 1;
	}


	PlayerCanPuchBody m_canPuchOtherBodies;
};

CustomJoint *CreateCustomPlayerController (const dFloat* pin, const NewtonBody* player, dFloat maxStairStepFactor)
{
	return (CustomJoint *) new CustomPlayerController (*(dVector*) pin, player, maxStairStepFactor);
}

void CustomPlayerControllerSetPushActorCallback (CustomJoint* playerController, PlayerCanPuchThisBodyCalback callback)
{
	((PlayerController*)playerController)->m_canPuchOtherBodies = (PlayerController::PlayerCanPuchBody)callback;
}

void CustomPlayerControllerSetVelocity (CustomJoint* playerController, dFloat forwardSpeed, dFloat sideSpeed, dFloat heading)
{
	((PlayerController*)playerController)->SetVelocity (forwardSpeed, sideSpeed, heading);
}

void CustomPlayerControllerSetMaxSlope (CustomJoint* playerController, dFloat maxSlopeAngleIndRadian)
{
	((PlayerController*)playerController)->SetMaxSlope (maxSlopeAngleIndRadian);
}
dFloat CustomPlayerControllerGetMaxSlope (CustomJoint* playerController)
{
	return ((PlayerController*)playerController)->GetMaxSlope();
}

const NewtonCollision* CustomPlayerControllerGetVerticalSensorShape (CustomJoint* playerController)
{
	return ((PlayerController*)playerController)->GetVerticalSensorShape();
}

const NewtonCollision* CustomPlayerControllerGetHorizontalSensorShape (CustomJoint* playerController)
{
	return ((PlayerController*)playerController)->GetHorizontalSensorShape ();
}

const NewtonCollision* CustomPlayerControllerGetDynamicsSensorShape (CustomJoint* playerController)
{
	return ((PlayerController*)playerController)->GetDynamicsSensorShape ();
}


// MultiBody Vehicle interface
CustomJoint *CreateCustomMultiBodyVehicle (const dFloat* frontDir, const dFloat* upDir, const NewtonBody* carBody)
{
	return (CustomJoint *) new CustomMultiBodyVehicle (*((dVector*) frontDir), *((dVector*) upDir), carBody);
}


int CustomMultiBodyVehicleAddTire (CustomJoint *car, const void* userData, const dFloat* localPosition, 
								  dFloat mass, dFloat radius, dFloat width,
								  dFloat suspensionLength, dFloat springConst, dFloat springDamper)
{
	return ((CustomMultiBodyVehicle*)car)->AddSingleSuspensionTire ((void*)userData, *((dVector*) localPosition), 
																	mass, radius, width, suspensionLength, springConst, springDamper);
}




int CustomMultiBodyVehicleAddSlipDifferencial (CustomJoint *car, int leftTireIndex, int rightToreIndex, dFloat maxFriction)
{
	return ((CustomMultiBodyVehicle*)car)->AddSlipDifferencial(leftTireIndex, rightToreIndex, maxFriction);
}


int CustomMultiBodyVehicleGetTiresCount(CustomJoint *car)
{
	return ((CustomMultiBodyVehicle*)car)->GetTiresCount();
}

const NewtonBody* CustomMultiBodyVehicleGetTireBody(CustomJoint *car, int tireIndex)
{
	return ((CustomMultiBodyVehicle*)car)->GetTireBody(tireIndex);
}

dFloat CustomMultiBodyVehicleGetSpeed(CustomJoint *car)
{
	return ((CustomMultiBodyVehicle*)car)->GetSpeed();
}

void CustomMultiBodyVehicleApplyTorque (CustomJoint *car, int tireIndex, dFloat torque)
{
	((CustomMultiBodyVehicle*)car)->ApplyTireTorque(tireIndex, torque);
}

void CustomMultiBodyVehicleApplySteering (CustomJoint *car, int tireIndex, dFloat angle)
{
	((CustomMultiBodyVehicle*)car)->ApplyTireSteerAngle(tireIndex, angle);
}

void CustomMultiBodyVehicleApplyBrake (CustomJoint *car, int tireIndex, dFloat brakeTorque)
{
	((CustomMultiBodyVehicle*)car)->ApplyTireBrake (tireIndex, brakeTorque);
}

void CustomMultiBodyVehicleApplyTireRollingDrag (CustomJoint *car, int tireIndex, dFloat angularDampingCoef)
{
	((CustomMultiBodyVehicle*)car)->ApplyTireRollingDrag (tireIndex, angularDampingCoef);
}

dFloat CustomMultiBodyVehicleGetTireSteerAngle (CustomJoint *car, int tireIndex)
{
	return ((CustomMultiBodyVehicle*)car)->GetSetTireSteerAngle (tireIndex);
}


// common Joints functions
int CustomGetJointID (CustomJoint *joint)
{
	return ((NewtonCustomJoint*)joint)->GetJointID ();
}

void CustomSetJointID (CustomJoint *joint, int rttI)
{
	((NewtonCustomJoint*)joint)->SetJointID(rttI);
}

const NewtonBody* CustomGetBody0 (CustomJoint *joint)
{
	return ((NewtonCustomJoint*)joint)->GetBody0();
}

const NewtonBody* CustomGetBody1 (CustomJoint *joint)
{
	return ((NewtonCustomJoint*)joint)->GetBody1();
}

int CustomGetBodiesCollisionState (CustomJoint *joint)
{
	return ((NewtonCustomJoint*)joint)->GetBodiesCollisionState();
}

void CustomSetBodiesCollisionState (CustomJoint *joint, int state)
{
	((NewtonCustomJoint*)joint)->SetBodiesCollisionState(state);
}

void CustomDestroyJoint(CustomJoint *joint)
{
	delete ((NewtonCustomJoint*)joint);
}
