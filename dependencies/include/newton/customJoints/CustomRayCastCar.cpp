// CustomRayCastCar.cpp: implementation of the CustomRayCastCar class.
//
//////////////////////////////////////////////////////////////////////
#include "CustomJointLibraryStdAfx.h"
#include "CustomRayCastCar.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define VEHICLE_LOCAL_ROOM 16

#define VEHICLE_DEFAULT_ROLLING_FRICTION (0.01f)



CustomRayCastCar::CustomRayCastCar(int maxTireCount, const dMatrix& cordenateSytem, NewtonBody* carBody)
	:NewtonCustomJoint(3 * maxTireCount, carBody, NULL)
{
	dVector com;
	dMatrix tmp;

	// set the chassis matrix at the center of mass
	NewtonBodyGetCentreOfMass(m_body0, &com[0]);
	com.m_w = 1.0f;

	// set the joint reference point at the center of mass of the body
	dMatrix chassisMatrix (cordenateSytem);
	chassisMatrix.m_posit += chassisMatrix.RotateVector(com);

	CalculateLocalMatrix (chassisMatrix, m_localFrame, tmp);

	// allocate space for the tires;
	m_tiresCount = 0;
	m_tires = new Tire[maxTireCount];

	m_curSpeed = 0.0f;
	m_aerodynamicDrag = 0.1f; 
	m_aerodynamicDownForce = 0.1f; 

	dFloat Ixx; 
	dFloat Iyy; 
	dFloat Izz; 
	NewtonBodyGetMassMatrix(m_body0, &m_mass, &Ixx, &Iyy, &Izz);

	// register the callback for tire integration
	NewtonUserJointSetFeedbackCollectorCallback (m_joint, IntegrateTires);
}


CustomRayCastCar::~CustomRayCastCar()
{
	NewtonWorld *world;

	world = NewtonBodyGetWorld (m_body0);
	for (int i = 0; i < m_tiresCount; i ++) {
		NewtonReleaseCollision (world, m_tires[i].m_shape);
	}

	if(m_tires) {
		delete[] m_tires;
	}
}

void CustomRayCastCar::IntegrateTires (const NewtonJoint* me, dFloat timestep, int threadIndex)
{
	CustomRayCastCar* joint;  

	// get the pointer to the joint class
	joint = (CustomRayCastCar*) NewtonJointGetUserData (me);  
	joint->IntegrateTires (timestep, threadIndex);
}


int CustomRayCastCar::GetTiresCount() const
{
	return m_tiresCount;
}




void CustomRayCastCar::GetInfo (NewtonJointRecord* info) const
{
}

//this function is to be overloaded by a derive class
void CustomRayCastCar::SetSteering (dFloat angle)
{
}



//this function is to be overloaded by a derive class
void CustomRayCastCar::SetTorque (dFloat torque)
{
}

dFloat CustomRayCastCar::GetSpeed() const
{
	return m_curSpeed;
}


CustomRayCastCar::Tire& CustomRayCastCar::GetTire (int index) const
{
	return m_tires[index];
}

dFloat CustomRayCastCar::GetParametricPosition (int index) const
{
	return m_tires[index].m_posit / m_tires[index].m_suspensionLength;
}

void CustomRayCastCar::SetTireSteerAngle (int index, dFloat angle)
{
	m_tires[index].m_steerAngle = angle;
	m_tires[index].m_localAxis.m_z = dCos (angle);
	m_tires[index].m_localAxis.m_x = dSin (angle);
}

void CustomRayCastCar::SetTireTorque (int index, dFloat torque)
{
//	torque=100.0;
	m_tires[index].m_torque = torque;
}



void CustomRayCastCar::AddSingleSuspensionTire (
	void *userData,
	const dVector& localPosition, 
	dFloat mass,
	dFloat radius, 
	dFloat width,
	dFloat suspensionLength,
	dFloat springConst,
	dFloat springDamper,
	int castMode)
{
	// calculate the tire local base pose matrix
	dMatrix bodyMatrix;
	m_tires[m_tiresCount].m_torque = 0.0f;
	m_tires[m_tiresCount].m_harpoint = m_localFrame.UntransformVector(localPosition);              
	m_tires[m_tiresCount].m_localAxis = m_localFrame.UnrotateVector(dVector (0.0f, 0.0f, 1.0f, 0.0f));
	m_tires[m_tiresCount].m_localAxis.m_w = 0.0f;
	m_tires[m_tiresCount].m_userData = userData;
	m_tires[m_tiresCount].m_angularVelocity = 0.0f;
	m_tires[m_tiresCount].m_spinAngle = 0.0f;
	m_tires[m_tiresCount].m_steerAngle = 0.0f;
	m_tires[m_tiresCount].m_posit = suspensionLength;
	m_tires[m_tiresCount].m_suspensionLength = suspensionLength;
	m_tires[m_tiresCount].m_tireLoad = 0.0f;

	m_tires[m_tiresCount].m_springConst = springConst;
	m_tires[m_tiresCount].m_springDamper = springDamper;
	m_tires[m_tiresCount].m_groundFriction = 1.0f;
	m_tires[m_tiresCount].m_fixedRollingFriction = VEHICLE_DEFAULT_ROLLING_FRICTION;
	m_tires[m_tiresCount].m_tireUseConvexCastMode = castMode; 
//	m_tires[m_tiresCount].m_tireUseConvexCastMode = 1; 
	m_tires[m_tiresCount].m_tireJacobianRowIndex = -1;

	// make a convex shape to represent the tire collision
	#define TIRE_SHAPE_SIZE 12
	dVector shapePoints[TIRE_SHAPE_SIZE * 2];
	for (int i = 0; i < TIRE_SHAPE_SIZE; i ++) {
		shapePoints[i].m_x = -width * 0.5f;	
		shapePoints[i].m_y = radius * dCos (2.0f * 3.1416 * dFloat(i)/ dFloat(TIRE_SHAPE_SIZE));
		shapePoints[i].m_z = radius * dSin (2.0f * 3.1416 * dFloat(i)/ dFloat(TIRE_SHAPE_SIZE));
		shapePoints[i + TIRE_SHAPE_SIZE].m_x = -shapePoints[i].m_x;
		shapePoints[i + TIRE_SHAPE_SIZE].m_y = shapePoints[i].m_y;
		shapePoints[i + TIRE_SHAPE_SIZE].m_z = shapePoints[i].m_z;
	}
	m_tires[m_tiresCount].m_shape = NewtonCreateConvexHull (m_world, TIRE_SHAPE_SIZE * 2, &shapePoints[0].m_x, sizeof (dVector), 0.0f, NULL);

	// calculate the tire geometrical parameters
	m_tires[m_tiresCount].m_radius = radius;
	m_tires[m_tiresCount].m_radiusInv  = 1.0f / m_tires[m_tiresCount].m_radius;
	m_tires[m_tiresCount].m_mass = mass;	
	m_tires[m_tiresCount].m_massInv = 1.0f / m_tires[m_tiresCount].m_mass;	
	m_tires[m_tiresCount].m_Ixx = mass * radius * radius / 2.0f;
	m_tires[m_tiresCount].m_IxxInv = 1.0f / m_tires[m_tiresCount].m_Ixx;

	m_tiresCount ++;
}


const dMatrix& CustomRayCastCar::GetChassisMatrixLocal () const
{
	return m_localFrame;
}

dMatrix CustomRayCastCar::CalculateSuspensionMatrix (int tireIndex, dFloat distance) const
{
	const Tire& tire = m_tires[tireIndex];

	dMatrix matrix;
	// calculate the steering angle matrix for the axis of rotation
	matrix.m_front = tire.m_localAxis;
	matrix.m_up    = dVector (0.0f, 1.0f, 0.0f, 0.0f);
	matrix.m_right = dVector (-tire.m_localAxis.m_z, 0.0f, tire.m_localAxis.m_x, 0.0f);
	matrix.m_posit = tire.m_harpoint - m_localFrame.m_up.Scale (distance);
	return matrix;
}

dMatrix CustomRayCastCar::CalculateTireMatrix (int tireIndex) const
{
	const Tire& tire = m_tires[tireIndex];

	// calculate the rotation angle matrix
	dMatrix angleMatrix (dPitchMatrix(tire.m_spinAngle));

	// get the tire body matrix
	dMatrix bodyMatrix;
	NewtonBodyGetMatrix(m_body0, &bodyMatrix[0][0]);
	return angleMatrix * CalculateSuspensionMatrix (tireIndex, tire.m_posit) * m_localFrame * bodyMatrix;
}


unsigned CustomRayCastCar::ConvexCastPrefilter(const NewtonBody* body, const NewtonCollision* collision, void* userData)
{
	NewtonBody* me;

	// for now just collide with static bodies
	dFloat mass; 
	dFloat Ixx; 
	dFloat Iyy; 
	dFloat Izz; 
	NewtonBodyGetMassMatrix(body, &mass, &Ixx, &Iyy, &Izz);
	if (mass > 0.0f) {
		return 0;
	}


	me = (NewtonBody*) userData;
	// do no cast myself
	return (me != body);
}


void CustomRayCastCar::CalculateTireCollision (Tire& tire, const dMatrix& suspensionMatrixInGlobalSpace) const
{

//static int xxx;
//xxx ++;


	if (tire.m_tireUseConvexCastMode) { 
		NewtonWorldConvexCastReturnInfo info;
		dVector destination (suspensionMatrixInGlobalSpace.TransformVector(m_localFrame.m_up.Scale (-tire.m_suspensionLength)));

//if (xxx > 2741)
//xxx *= 1;

		// cast the convex shape along the suspension length
		dFloat hitParam;
		if (NewtonWorldConvexCast (m_world, &suspensionMatrixInGlobalSpace[0][0], &destination[0], tire.m_shape, &hitParam, (void*)m_body0, ConvexCastPrefilter, &info, 1)) {
			// the ray hit something
			tire.m_posit = info.m_intersectionParam * tire.m_suspensionLength;
			tire.m_contactPoint = info.m_point;
			tire.m_contactNormal = info.m_normal;
			// TO DO: get the material properties for tire frictions on different roads 

//dTrace (("%f\n", info.m_intersectionParam));

			switch (info.m_contactID)
			{
				case 0:
				{
					// normal ground friction
					tire.m_groundFriction = 1.0f;
					break;
				}

				default:
				{
					// default ground friction
					tire.m_groundFriction = 1.0f;
					break;
				}
			}

		} else {
			// if the ray do not hit anything the tire in airborne
			tire.m_posit = tire.m_suspensionLength;
			tire.m_groundFriction = 0.0f;
		}

	} else {
		// make a data structure to collect the information returned by the ray cast
		struct RayCastInfo
		{
			RayCastInfo(const NewtonBody* body)
			{
				m_param = 1.0f;
				m_me = body;
				m_hitBody = NULL;
			}


			static dFloat RayCast (const NewtonBody* body, const dFloat* normal, int collisionID, void* userData, dFloat intersetParam)
			{
				RayCastInfo& caster = *((RayCastInfo*) userData); 

				// if this body is not the vehicle, see if a close hit
				if (body != caster.m_me) {
					if (intersetParam < caster.m_param) {
						// this is a close hit, record the information. 
						caster.m_param = intersetParam;
						caster.m_hitBody = body;
						caster.m_contactID = collisionID;
						caster.m_normal = dVector (normal[0], normal[1], normal[2], 1.0f);
					}
				}
				return intersetParam;
			}

			dFloat m_param;
			dVector m_normal;
			const NewtonBody* m_me;
			const NewtonBody* m_hitBody;
			int m_contactID;
		};

		RayCastInfo info (m_body0);

		// extend the ray by the radius of the tire
		dFloat dist (tire.m_suspensionLength + tire.m_radius);
		dVector destination (suspensionMatrixInGlobalSpace.TransformVector(m_localFrame.m_up.Scale (-dist)));	

		// cast a ray to the world
		NewtonWorldRayCast(m_world, &suspensionMatrixInGlobalSpace.m_posit[0], &destination[0], RayCastInfo::RayCast, &info, ConvexCastPrefilter);

		// if the ray hit something, it means the tire has some traction
		if (info.m_hitBody) {
			dFloat intesectionDist;


			tire.m_contactPoint = suspensionMatrixInGlobalSpace.m_posit + (destination - suspensionMatrixInGlobalSpace.m_posit).Scale (info.m_param); 
			tire.m_contactNormal = info.m_normal;
			// TO DO: get the material properties for tire frictions on different roads 

			intesectionDist = dist * info.m_param - tire.m_radius;
			if (intesectionDist > tire.m_suspensionLength) {
				intesectionDist = tire.m_suspensionLength;
			} else if (intesectionDist < dFloat (0.0f)) {
				intesectionDist = dFloat (0.0f);
			}
			tire.m_posit = intesectionDist;
			switch (info.m_contactID)
			{
				case 0:
				{
					// normal ground friction
					tire.m_groundFriction = 1.0f;
					break;
				}

				default:
				{
					// default ground friction
					tire.m_groundFriction = 1.0f;
					break;
				}
			}

		} else {
			tire.m_posit = tire.m_suspensionLength;
			tire.m_groundFriction = 0.0f;
		}
	}
}



dFloat CustomRayCastCar::GetNormalizedLateralForce (dFloat sideSlipAngleInDegres) const
{
	// this function should return the normalized size slip force at the tire 
	// the expression is according to the book: Race Car Vehicle Dynamics 
	// by: Willian F. Milliken and Douglas Milliken
	// Page 26 & 27 is:
	// tireLoad / sizeForce = sideSlipCoeficient
	// 
	// there are several tire model graph that can be used to get the normalize tire for cuve value 
	// http://www.racer.nl/reference/pacejka.htm
	// http://code.eng.buffalo.edu/dat/sites/tire/tire.html
	
	// but for simplicity I will use piece wise curve zero degree curve
	const dFloat maxSideSlipAngle = 20.0f;
	if (sideSlipAngleInDegres > maxSideSlipAngle) {
		return 1.0f;
	} else {
		return sideSlipAngleInDegres / maxSideSlipAngle;
	}
}

dFloat CustomRayCastCar::GetNormalizedTractionAndBrakingForce (dFloat slipRatio) const
{
	// this function should return the normalized traction force at the tire 
	// the expression is according to the book: Race Car Vehicle Dynamics 
	// by: Willian F. Milliken and Douglas Milliken, page 38is:
	// tireLoad / sizeForce = sideSlipCoeficient
	// 
	// there are several tire model graph that can be used to get the normalize tire for cuve value 
	// http://www.racer.nl/reference/pacejka.htm
	// http://code.eng.buffalo.edu/dat/sites/tire/tire.html


	// but for simplicity I will use piece wise curve zero degree curve
	const dFloat maxSlipRatio = 0.5f;
	if (slipRatio > maxSlipRatio) {
		return 1.0f;
	} else if (slipRatio < maxSlipRatio) {
		return -1.0f;
	} else {
		return slipRatio / maxSlipRatio;
	}
}




void CustomRayCastCar::SubmitConstrainst(dFloat timestep, int threadIndex)
{
	dFloat invTimestep;
	dMatrix bodyMatrix;
	dMatrix suspensionMatrices[VEHICLE_LOCAL_ROOM];  

	// get the simulation time
	invTimestep = 1.0f / timestep ;

	// get the vehicle global matrix, and use it in several calculations
	NewtonBodyGetMatrix(m_body0, &bodyMatrix[0][0]);
	dMatrix chassisMatrix (m_localFrame * bodyMatrix);  

	// calculate all suspension matrices in global space and tire collision
	for (int i = 0; i < m_tiresCount; i ++) {
		Tire& tire = m_tires[i];
		dMatrix suspensionMatrix = suspensionMatrices[i];

		// calculate this suspension matrix and save it for future used
		suspensionMatrix = CalculateSuspensionMatrix (i, 0.0f) * chassisMatrix;

		// calculate the tire collision
		CalculateTireCollision (tire, suspensionMatrix);
	}


	// calculate all suspension forces due to spring and damper
	dVector globalVeloc;
	dVector globalOmega;
	dVector force (0.0f, 0.0f, 0.0f, 0.0f);
	dVector torque (0.0f, 0.0f, 0.0f, 0.0f);

	// get the chassis instantaneous linear and angular velocity in the local space of the chassis
	int longitidunalForceIndex;
	longitidunalForceIndex = 0;
	NewtonBodyGetVelocity(m_body0, &globalVeloc[0]);
	NewtonBodyGetOmega(m_body0, &globalOmega[0]);
	for (int i = 0; i < m_tiresCount; i ++) {
		Tire& tire = m_tires[i];

		if (tire.m_posit < tire.m_suspensionLength) {
			dFloat speed;

			// calculate the linear velocity of the tire at the ground contact
			dVector tireAxelPosit (chassisMatrix.TransformVector(tire.m_harpoint - m_localFrame.m_up.Scale (tire.m_posit)));
			dVector localAxelPosit (tireAxelPosit - chassisMatrix.m_posit);
			dVector tireAxelVeloc (globalVeloc + globalOmega * localAxelPosit); 		

			// TO DO: need to calculate the velocity if the other body at the point
			// for now assume the ground is a static body
			dVector hitBodyVeloc (0, 0, 0, 0);

			// calculate the relative velocity
			dVector relVeloc (tireAxelVeloc - hitBodyVeloc);
			speed = -(relVeloc % chassisMatrix.m_up);

			// now calculate the tire load at the contact point
			tire.m_tireLoad = - NewtonCalculateSpringDamperAcceleration (timestep, tire.m_springConst, tire.m_suspensionLength - tire.m_posit, tire.m_springDamper, speed);

			if (tire.m_tireLoad < 0.0f) {
				// since the tire is not a body with real mass it can only push the chassis.
				tire.m_tireLoad = 0.0f;
			} else {
				//this suspension is applying a normalize force to the car chassis, need to scales by the mass of the car
				tire.m_tireLoad *= m_mass;

				// apply the tire model to these wheel
				ApplyTireFrictionModel(tire, chassisMatrix, tireAxelVeloc, tireAxelPosit, timestep, invTimestep, longitidunalForceIndex);
			}

			// convert the tire load force magnitude to a torque and force.
			dVector tireForce (chassisMatrix.m_up.Scale (tire.m_tireLoad));

			// accumulate the force and torque form this suspension
			force += tireForce;
			torque += localAxelPosit * tireForce;
		} else {
			//tire is on the air  not force applied to the vehicle.
			tire.m_tireLoad = dFloat (0.0f);
			tire.m_tireJacobianRowIndex = -1;
		}
	}


	// add aerodynamic forces
	dVector verticalVeloc (chassisMatrix.m_up.Scale (chassisMatrix.m_up % globalVeloc));
	dVector horizontalVeloc (globalVeloc - verticalVeloc);
	force -= horizontalVeloc.Scale (m_aerodynamicDrag * m_mass);
	force -= chassisMatrix.m_up.Scale (m_aerodynamicDownForce * m_mass * sqrtf (horizontalVeloc % horizontalVeloc));

	// apply the net suspension force and torque to the car chassis.
	NewtonBodyAddForce(m_body0, &force[0]);
	NewtonBodyAddTorque(m_body0, &torque[0]);

	// set the current vehicle speed
	m_curSpeed = bodyMatrix[0] % globalVeloc;
}






#if 0
void CustomRayCastCar::ApplyTireFrictionModel(
	Tire& tire, 
	const dMatrix& chassisMatrix,
	const dVector& tireAxelVeloc,
	const dVector& tireAxelPosit,
	dFloat timestep,
	dFloat invTimestep)
{
	// calculate the point of side ping vector
	dVector lateralPin (chassisMatrix.RotateVector (tire.m_localAxis));
	dVector longitudinalPin (chassisMatrix.m_up * lateralPin);
	tire.m_longitudinalDir = longitudinalPin;
	tire.m_lateralDir = lateralPin; 

	// TO DO: need to subtract the velocity at the contact point of the hit body
	// for now assume the ground is a static body
	dVector hitBodyContactVeloc (0, 0, 0, 0);

	// calculate relative velocity at the tire center
	dVector tireAxelRelativeVelocity (tireAxelVeloc - hitBodyContactVeloc); 

	// now calculate relative velocity a velocity at contact point

	dVector tireAngularVelocity (lateralPin.Scale (tire.m_angularVelocity));
	dVector tireRadius (tire.m_contactPoint - tireAxelPosit);
	dVector tireContactVelocity (tireAngularVelocity * tireRadius);	
	dVector tireContactRelativeVelocity (tireAxelRelativeVelocity + tireContactVelocity); 

	tire.m_tireRadius = tireRadius;

	if (1) {
		// the tire is in coasting mode
		//		dFloat tireContactSpeed;
		//		dFloat tireRelativeSpeed;
		//		dFloat lateralForceMagnitud;

		//these tire is coasting, so the lateral friction dominates the behaviors  

		dFloat invMag2;
		dFloat frictionCircleMag;
		dFloat lateralFrictionForceMag;
		dFloat longitudinalAcceleration;
		dFloat longitudinalFrictionForceMag;

		frictionCircleMag = tire.m_tireLoad * tire.m_groundFriction;
		lateralFrictionForceMag = frictionCircleMag;
		longitudinalFrictionForceMag = tire.m_tireLoad * tire.m_fixedRollingFriction;

		invMag2 = frictionCircleMag / dSqrt (lateralFrictionForceMag * lateralFrictionForceMag + longitudinalFrictionForceMag * longitudinalFrictionForceMag);
		lateralFrictionForceMag *= invMag2;
		longitudinalFrictionForceMag = invMag2;

		//longitudinalFrictionForceMag = 1000;
		NewtonUserJointAddLinearRow (m_joint, &tireAxelPosit[0], &tireAxelPosit[0], &lateralPin[0]);
		NewtonUserJointSetRowMaximumFriction(m_joint,  lateralFrictionForceMag);
		NewtonUserJointSetRowMinimumFriction(m_joint, -lateralFrictionForceMag);

		// apply longitudinal friction force and acceleration
		longitudinalAcceleration = -(tireContactRelativeVelocity % longitudinalPin) * invTimestep;
		NewtonUserJointAddLinearRow (m_joint, &tireAxelPosit[0], &tireAxelPosit[0], &longitudinalPin[0]);
		NewtonUserJointSetRowMaximumFriction (m_joint,  longitudinalFrictionForceMag);
		NewtonUserJointSetRowMinimumFriction (m_joint, -longitudinalFrictionForceMag);
		NewtonUserJointSetRowAcceleration (m_joint, longitudinalAcceleration);

		// save the tire contact longitudinal velocity for integration after the solver
		tire.m_currentSlipVeloc = tireAxelRelativeVelocity % longitudinalPin;
	} else {
		_ASSERTE (0);
	}
}

#else 

void CustomRayCastCar::ApplyTireFrictionModel(
	Tire& tire, 
	const dMatrix& chassisMatrix,
	const dVector& tireAxelVeloc,
	const dVector& tireAxelPosit,
	dFloat timestep,
	dFloat invTimestep,
	int& longitudinalForceIndex)
{
	// calculate the point of side ping vector
	dVector lateralPin (chassisMatrix.RotateVector (tire.m_localAxis));
	dVector longitudinalPin (chassisMatrix.m_up * lateralPin);
	tire.m_longitudinalDir = longitudinalPin;
	tire.m_lateralDir = lateralPin; 

	// TO DO: need to subtract the velocity at the contact point of the hit body
	// for now assume the ground is a static body
	dVector hitBodyContactVeloc (0, 0, 0, 0);

	// calculate relative velocity at the tire center
	dVector tireAxelRelativeVelocity (tireAxelVeloc - hitBodyContactVeloc); 

	// now calculate relative velocity a velocity at contact point

	dVector tireAngularVelocity (lateralPin.Scale (tire.m_angularVelocity));
	dVector tireRadius (tire.m_contactPoint - tireAxelPosit);
	dVector tireContactVelocity (tireAngularVelocity * tireRadius);	
	dVector tireContactRelativeVelocity (tireAxelRelativeVelocity + tireContactVelocity); 
	tire.m_tireRadius = tireRadius;


	if (1) {
		// the tire is in coasting mode
		//		dFloat tireContactSpeed;
		//		dFloat tireRelativeSpeed;
		//		dFloat lateralForceMagnitud;

		//these tire is coasting, so the lateral friction dominates the behaviors  

		dFloat invMag2;
		dFloat frictionCircleMag;
		dFloat lateralFrictionForceMag;
		dFloat longitudinalAcceleration;
		dFloat longitudinalFrictionForceMag;

		frictionCircleMag = tire.m_tireLoad * tire.m_groundFriction;
		lateralFrictionForceMag = frictionCircleMag;
		longitudinalFrictionForceMag = tire.m_tireLoad * tire.m_fixedRollingFriction;

		invMag2 = frictionCircleMag / dSqrt (lateralFrictionForceMag * lateralFrictionForceMag + longitudinalFrictionForceMag * longitudinalFrictionForceMag);
		lateralFrictionForceMag *= invMag2;
		longitudinalFrictionForceMag = invMag2;

		//longitudinalFrictionForceMag = 1000;
		NewtonUserJointAddLinearRow (m_joint, &tireAxelPosit[0], &tireAxelPosit[0], &lateralPin[0]);
		NewtonUserJointSetRowMaximumFriction(m_joint,  lateralFrictionForceMag);
		NewtonUserJointSetRowMinimumFriction(m_joint, -lateralFrictionForceMag);
		longitudinalForceIndex ++; 

		// apply longitudinal friction force and acceleration
		longitudinalAcceleration = -(tireContactRelativeVelocity % longitudinalPin) * invTimestep;
		NewtonUserJointAddLinearRow (m_joint, &tireAxelPosit[0], &tireAxelPosit[0], &longitudinalPin[0]);
		NewtonUserJointSetRowMaximumFriction (m_joint,  longitudinalFrictionForceMag);
		NewtonUserJointSetRowMinimumFriction (m_joint, -longitudinalFrictionForceMag);
		NewtonUserJointSetRowAcceleration (m_joint, longitudinalAcceleration);
		tire.m_tireJacobianRowIndex = longitudinalForceIndex;
		longitudinalForceIndex ++;



		// save the tire contact longitudinal velocity for integration after the solver
		tire.m_currentSlipVeloc = tireAxelRelativeVelocity % longitudinalPin;
	} else {
		_ASSERTE (0);
	}

	/*	
	// calculate the side slip tire angle
	dFloat maxSideForce;
	dFloat sideSlipAngle; 
	dFloat sideSlipVelocity; 
	dFloat maxLongitudinalForce;
	dFloat normalizeSideSlipCoef;
	dFloat longitudinalAxelVelocity; 

	sideSlipVelocity = lateralPin % tireAxelRelativeVelocity;
	longitudinalAxelVelocity = longitudinalPin % tireAxelRelativeVelocity;


	// if the the car is moving too slow, the for the side 
	// Hack: to give some lateral stiffness behavior if the later velocity is too low, 
	// add so minimum value this will provide static friction when the car is moving at low speed.
	dFloat lateralSpeedStaticVehviorKludge = 2.0f;
	sideSlipAngle = dAtan2 (dAbs (sideSlipVelocity) + lateralSpeedStaticVehviorKludge, dAbs (longitudinalAxelVelocity));

	normalizeSideSlipCoef = GetNormalizedLateralForce (sideSlipAngle * 180.0f / 3.1416f);
	maxSideForce = tire.m_tireLoad * normalizeSideSlipCoef;


	// Now calculate the longitudinal tire Slip Ratio and force
	dFloat freeRollingVelocity; 
	const dFloat smallValue = dFloat (1.0e-2f);
	freeRollingVelocity = -tire.m_angularVelocity * tire.m_radius;
	if ((dAbs (freeRollingVelocity) < smallValue) && (dAbs (longitudinalAxelVelocity) < smallValue) ) {
	// the tire is rolling very slow the car is probable stopped or coasting
	maxLongitudinalForce = 0.1f;
	} else {
	dFloat slipRatio;
	dFloat slipRatioCoef;
	// definition of slip ration according to: Race Car Vehicle Dynamics 
	// by: Willian F. Milliken and Douglas Milliken page 39
	// tireLoad / sizeForce = sideSlipCoeficient
	// 
	slipRatio = freeRollingVelocity / longitudinalAxelVelocity - 1.0f;
	slipRatioCoef = GetNormalizedTractionAndBrakingForce (slipRatio);
	maxLongitudinalForce = tire.m_tireLoad * slipRatioCoef;
	}


	// make sure the lateral and longitudinal force are within the tire friction circle 
	dFloat forceMag2 = maxSideForce * maxSideForce + maxLongitudinalForce * maxLongitudinalForce;
	if (forceMag2 > tire.m_tireLoad * tire.m_tireLoad) {
	forceMag2 = tire.m_tireLoad / dSqrt (forceMag2);
	maxSideForce *= forceMag2;
	maxLongitudinalForce *= forceMag2;
	}
	*/
}

#endif




void CustomRayCastCar::IntegrateTires (dFloat timestep, int threadIndex)
{
	dFloat timestepInv;

	// get the simulation time
	timestepInv = 1.0f / timestep;

	dMatrix bodyMatrix;
	NewtonBodyGetMatrix(m_body0, &bodyMatrix[0][0]);
	dMatrix chassisMatrix (m_localFrame * bodyMatrix);  

	dVector globalVeloc;
	dVector globalOmega;
	NewtonBodyGetVelocity(m_body0, &globalVeloc[0]);
	NewtonBodyGetOmega(m_body0, &globalOmega[0]);

	// get the tire longitudinal to apply free force diagram for tire integration
	for (int i = 0; i < m_tiresCount; i ++) {
		Tire& tire = m_tires[i];

		if (tire.m_tireJacobianRowIndex == -1){
			dFloat torque;
			//this is free rolling tire 
			//just apply just integrate the torque and apply some angular damp
			torque = tire.m_torque - tire.m_angularVelocity * tire.m_Ixx * 0.1f;
			tire.m_angularVelocity  += torque * tire.m_IxxInv * timestep;


		} else {
			//check if any engine torque or brake torque is applied to the tire
			if (dAbs(tire.m_torque) < 1.0e-3f){
				//tire is coasting, calculate the tire zero slip angular velocity
				// this is the velocity that satisfy the constraint equation
				// V % dir + W * R % dir = 0
				// where V is the tire Axel velocity
				// W is the tire local angular velocity
				// R is the tire radius
				// dir is the longitudinal direction of of the tire.

				dVector tireAxelPosit (chassisMatrix.TransformVector(tire.m_harpoint - chassisMatrix.m_up.Scale (tire.m_posit)));
				dVector localAxelPosit (tireAxelPosit - chassisMatrix.m_posit);
				dVector tireAxelVeloc (globalVeloc + globalOmega * localAxelPosit); 		

				dFloat tireLinearSpeed;
				dFloat tireContactSpeed;
				tireLinearSpeed = tireAxelVeloc % tire.m_longitudinalDir;
				tireContactSpeed = (tire.m_lateralDir * tire.m_tireRadius) % tire.m_longitudinalDir;
				tire.m_angularVelocity = - tireLinearSpeed / tireContactSpeed ;
			} else {
				// tire is under some power, need to do the free body integration to apply the net torque
				int tireforceIndex;
				dFloat netTorque;
				dFloat tireFrictionTorque;
				tireforceIndex = tire.m_tireJacobianRowIndex;
				tireFrictionTorque = tire.m_radius * NewtonUserJointGetRowForce (m_joint, tireforceIndex);
				netTorque = tire.m_torque - tireFrictionTorque - tire.m_angularVelocity * tire.m_Ixx * 0.1f;


				tire.m_angularVelocity  += netTorque * tire.m_IxxInv * timestep;

			}
		}

		// integrate tire angular velocity and rotation
		tire.m_spinAngle = dMod (tire.m_spinAngle + tire.m_angularVelocity * timestep, 3.1416f * 2.0f); 

		// reset tire torque to zero after integration; 
		tire.m_torque = 0.0f;
	}
}




