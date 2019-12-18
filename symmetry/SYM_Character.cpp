#include <sstream>
#include <math.h>

#include "SYM_Character.h"
#include "SYM_Engine.h"

namespace sym
{
	bool CCharacterController::Create(std::string ModelFilename)
	{
		//Initialise variables
		RunSpeed = 10.0f;
		TurnSpeed = 5.0f;
		UpVectorJoint = 0;
		TargetHeading = 0.0f;
		UseInverseDynamics = true;

		//Load model
		if(!Load("", ModelFilename, sym::PHYSICS_OBJ_BOX)) return false;
		CreateSphere(CVector3(GetLocalAABB().Size.y / 3.0f, GetLocalAABB().Size.y, GetLocalAABB().Size.y / 3.0f));

		//Disable auto freeze
		SetAutoFreeze(false);

		

		
		//Set force/torque callback
		OverrideCallbackForceAndTorque(OnApplyForceAndTorque);
		//OverrideCallbackSetTransform(OnSetTransform);

		SetUpVector(CVector3(0, 1, 0));

		float damp[3];
		damp[0] = 0.0f;
		damp[1] = 0.0f;
		damp[2] = 0.0f;
		// set the viscous damping the the minimum
		NewtonBodySetLinearDamping (GetNewtonBody(), 0.0f);
		NewtonBodySetAngularDamping (GetNewtonBody(), damp);

		//Create material ID
		CreateNewtonMaterial();
		NewtonMaterialSetDefaultFriction(Engine->Physics.GetWorld(), NewtonMaterialGetDefaultGroupID(Engine->Physics.GetWorld()), MaterialID, 0.0f, 0.0f);
		NewtonMaterialSetDefaultElasticity(Engine->Physics.GetWorld(), NewtonMaterialGetDefaultGroupID(Engine->Physics.GetWorld()), MaterialID, 0.0f);
		NewtonMaterialSetDefaultSoftness(Engine->Physics.GetWorld(), NewtonMaterialGetDefaultGroupID(Engine->Physics.GetWorld()), MaterialID, 0.1f);

		//NewtonMaterialSetCollisionCallback(Engine->Physics.GetWorld(), NewtonMaterialGetDefaultGroupID(Engine->Physics.GetWorld()), MaterialID, 0, 0, OnLevelCollision);
		
		

		//ApplyControlJoint();

		SetMass(30.0f);

		NewtonBodySetUserData(GetNewtonBody(), this);

		return true;
	}

	void CCharacterController::ApplyControlJoint()
	{
		ControlJoint = new NewtonPlayerController(dVector(0.0f, 1.0f, 0.0f), GetNewtonBody(), 0.1f);
		ControlJoint->SetMaxSlope(45.0f * sym::Maths::DEGREES_TO_RADIANS);
	}

	void CCharacterController::ReleaseControlJoint()
	{
		//NewtonDestroyJoint(Engine->Physics.GetWorld(), ControlJoint);
	}

	void CCharacterController::ApplyInverseDynamics()
	{
		UseInverseDynamics = true;
	}
	
	void CCharacterController::ReleaseInverseDynamics()
	{
		UseInverseDynamics = false;
	}

	void CCharacterController::Jump(float Height)
	{
		Impulse(CVector3(0.0f, Height, 0.0f), GetPosition());
	}

	void CCharacterController::SetUpVector(CVector3 UpVector)
	{
		//Set up vector
		CVector3 UpDirection (0.0f, 1.0f, 0.0f);
		UpVectorJoint = NewtonConstraintCreateUpVector(Engine->Physics.GetWorld(), &UpDirection.x, GetNewtonBody());
	}

	void CCharacterController::ReleaseUpVector()
	{
		if(UpVectorJoint) NewtonDestroyJoint(Engine->Physics.GetWorld(), UpVectorJoint);
		UpVectorJoint = 0;
	}

	void CCharacterController::SetMovement(float Forward, float Side, float Heading)
	{
		Heading = Heading - (270.0f * Maths::DEGREES_TO_RADIANS);
		if(ControlJoint) ControlJoint->SetVelocity(Forward, Side, Heading);
	}

	void CCharacterController::SetMovementVector(CVector3 MovVec)
	{
		Movement = MovVec;
	}

	void CCharacterController::SetHeading(float Degrees)
	{
		TargetHeading = Degrees * Maths::DEGREES_TO_RADIANS;
	}

	void CCharacterController::OnApplyForceAndTorque(const NewtonBody* Body, float timestep, int threadIndex)
	{
		//Invert timestep
		float TimestepInv = 1.0f / timestep;

		//Get newton body
		CCharacterController *Character = (CCharacterController*)NewtonBodyGetUserData(Body);
		if(!Character) return;

		//Get mass
		float Mass, Ix, Iy, Iz;
		NewtonBodyGetMassMatrix(Body, &Mass, &Ix, &Iy, &Iz);

		//Add gravity to force
		Character->Force += CVector3(0.0f, -Mass * 18.16f, 0.0f);

		//Normalise movement vector
		Character->Movement.Normalise();

		//Get current velocity
		CVector3 Velocity;
		NewtonBodyGetVelocity(Body, &Velocity.x);

		//Get target velocity
		CVector3 TargetVelocity = Character->Movement * Character->RunSpeed * Character->Mass;

		if(Character->UseInverseDynamics)
		{
			//Calculate inverse dynamics (required force to reach target velocity)
			CVector3 DynamicsForce;
			NewtonBodyCalculateInverseDynamicsForce(Body, timestep, &TargetVelocity.x, &DynamicsForce.x);

			//Zero Y force
			DynamicsForce.y = 0.0f;

			//Add force
			Character->Force += DynamicsForce;
		}

		//Add any Y velocity
		//Force.y += Velocity.y;

		//Calculate omega from target heading
		CVector3 heading (cos(Character->TargetHeading), 0.0f, sin(Character->TargetHeading));

		float turnAngle = Character->GetForward().Cross(heading).y;
		turnAngle = min(max(turnAngle, -1.0f), 1.0f);
		turnAngle = asin (turnAngle); 

		float TurnOmega = turnAngle * TimestepInv;
		CVector3 Omega(0.0f, TurnOmega, 0.0f);

		//Apply final force and torque
		NewtonBodySetForce(Body, &Character->Force.x);
		NewtonBodySetOmega(Body, &Omega.x);
		//NewtonBodySetTorque(Body, &HeadingTorque.x);

		//Reset force and torque
		Character->Force = CVector3(0.0f, 0.0f, 0.0f);
		Character->Torque = CVector3(0.0f, 0.0f, 0.0f);
		Character->Movement = CVector3(0.0f, 0.0f, 0.0f);
	}

	void CCharacterController::OnSetTransform(const NewtonBody* body, const float* matrix, int threadIndex)
	{
		CCharacterController* Character = (CCharacterController*)NewtonBodyGetUserData(body);
		CMatrix4 Mat((float*)matrix);
		Character->SetPosition(Mat.GetTranslation());
	}

	/*
	void CCharacterController::OnApplyForceAndTorque(const NewtonBody* Body, float timestep, int threadIndex)
	{
		CCharacterController *ThisCharacter = (CCharacterController*) NewtonBodyGetUserData(Body);

		float mass;
		float Ixx;
		float Iyy;
		float Izz;
		float dist;
		float floor;
		float accelY;
		float deltaHeight;
		float steerAngle;
		//float timestep;
		float timestepInv;
		CVector3 omega;
		CVector3 alpha;
		CVector3 heading;
		CVector3 velocity;
		CMatrix4 matrix;

		// Get the current world timestep
		timestepInv = 1.0f / timestep;

		// get the character mass
		NewtonBodyGetMassMatrix (Body, &mass, &Ixx, &Iyy, &Izz);

		// apply the gravity force, cheat a little with the character gravity
		ThisCharacter->Force += CVector3 (0.0f, -mass * 9.8, 0.0f);

		// Get the velocity vector
		NewtonBodyGetVelocity(Body, &velocity.x);

		// determine if the character have to be snap to the ground
		NewtonBodyGetMatrix(Body, matrix.GetAsFloatArray());

		// if the floor is with in reach then the character must be snap to the ground
		// the max allow distance for snapping i 0.25 of a meter
		if (ThisCharacter->IsAirborne && ThisCharacter->JumpTimer == 0.0)
		{ 
			floor = ThisCharacter->FindFloor (matrix.GetTranslation(), ThisCharacter->Mesh->AABB.Size.y + 0.25f);
			deltaHeight = (matrix.GetTranslation().y - ThisCharacter->Mesh->AABB.Size.y) - floor;

			if ((deltaHeight < (0.25f - 0.001f)) && (deltaHeight > 0.01f))
			{
				// snap to floor only if the floor is lower than the character feet		
				accelY = - (deltaHeight * timestepInv + velocity.y) * timestepInv;
				ThisCharacter->Force.y = mass * accelY;
			}
		}
		else if (ThisCharacter->JumpTimer == 4)
		{
			CVector3 veloc (0.0f, ThisCharacter->JumpHeight, 0.0f);
			CVector3 point = matrix.GetTranslation();
			NewtonBodyAddImpulse(Body, &veloc.x, &point.x);
		}

		ThisCharacter->JumpTimer = ThisCharacter->JumpTimer ? ThisCharacter->JumpTimer - 1 : 0;
		
		
		// estimate the final horizontal translation for to next force and velocity
		CVector3 step ((ThisCharacter->Force * (timestep  / mass) + velocity) * (timestep));

		step = ThisCharacter->GetMatrix().UnrotateVector (step);
		CMatrix4 collisionPaddingMatrix;

		step.y = 0.0f;

		dist = step.Dot(step);
		if (dist > ThisCharacter->MaxTranslation * ThisCharacter->MaxTranslation)
		{
			// when the velocity is high enough that can miss collision we will enlarge the collision 
			// long the vector velocity
			dist = sqrt (dist);
			step = step * (1.0f / dist);

			//make a rotation matrix that will align the velocity vector with the front vector
			collisionPaddingMatrix.set(0, 0, step.x);
			collisionPaddingMatrix.set(0, 2, -step.y);
			collisionPaddingMatrix.set(2, 0, step.y);
			collisionPaddingMatrix.set(2, 2, step.x);

			// get the transpose of the matrix
			CMatrix4 transp (collisionPaddingMatrix);
			transp = transp.GetTransposeMatrix();
			transp.set(0, 0, transp.get(0, 0) * (dist / ThisCharacter->MaxTranslation));

			// calculate and oblique scale matrix by using a similar transformation matrix of the for, R'* S * R
			collisionPaddingMatrix = collisionPaddingMatrix * transp;
		}

		// set the collision modifierMatrix;
		NewtonConvexHullModifierSetMatrix (NewtonBodyGetCollision(Body), collisionPaddingMatrix.GetAsFloatArray());

		// assume the character is on the air. this variable will be set to false if the contact detect 
		//the character is landed 
		ThisCharacter->IsAirborne = true;
		ThisCharacter->StepContact = CVector3 (0.0f, -ThisCharacter->Mesh->AABB.Size.y, 0.0f);  

		CVector3 pin (0.0f, 1.0f, 0.0f);
		NewtonUpVectorSetPin(ThisCharacter->UpVectorJoint, &pin.x);

		//Call original callback
		NewtonBodySetForce(Body, &ThisCharacter->Force.x);
		NewtonBodySetTorque(Body, &ThisCharacter->Torque.x);

		ThisCharacter->Force = CVector3(0.0f, 0.0f, 0.0f);
		ThisCharacter->Torque = CVector3(0.0f, 0.0f, 0.0f);
	}
	*/

	void CCharacterController::CharLevelCollision(void* contact, NewtonMaterial *material)
	{
		/*
		CVector3 point;
		CVector3 normal;
		CVector3 velocity;

		// Get the collision and normal
		NewtonMaterialGetContactPositionAndNormal(material, &point.x, &normal.x);

		CVector3 localPoint (GetMatrix().GetInverse().TransformVector(point));

		// if a contact is below the max need consider the character is on the ground
		if (localPoint.y < MaxStepHeight)
		{
			IsAirborne = false;
				
			NewtonBodyGetVelocity(GetNewtonBody(), &velocity.x);

			// calculate ball velocity perpendicular to the contact normal
			CVector3 tangentVelocity (velocity - normal.Multiply (normal.Cross(velocity)));

			// align the tangent at the contact point with the tangent velocity vector of the ball
			NewtonMaterialContactRotateTangentDirections (material, &tangentVelocity.x);

			// we do do want bound back we hitting the floor
			NewtonMaterialSetContactElasticity(material, 0.3f);
		
			// save the elevation of the highest step to take
			if (localPoint.y > StepContact.y)
			{
				if (abs(normal.y) < 0.8f)
				{
					StepContact = localPoint;   
				}
			}

			// if the player want to move set disable friction else set high ground friction so it can stop on slopes
			if (IsStopped)
			{
				NewtonMaterialSetContactFrictionCoef(material, 2.0f, 2.0f, 0);
				NewtonMaterialSetContactFrictionCoef(material, 2.0f, 2.0f, 1);

			}
			else
			{
				NewtonMaterialSetContactFrictionState (material, 0, 0);
				NewtonMaterialSetContactFrictionState (material, 0, 1);
			}

		}
		else
		{
			//set contact above the max step to be friction less
			// disable fiction calculation for sphere collision
			NewtonMaterialSetContactFrictionState (material, 0, 0);
			NewtonMaterialSetContactFrictionState (material, 0, 1);
		}
		*/
	}

	void CCharacterController::OnLevelCollision(const NewtonJoint* contactJoint, dFloat timestep, int threadIndex)
	{
		/*
		const NewtonBody *body0 = NewtonJointGetBody0(contactJoint);
		const NewtonBody *body1 = NewtonJointGetBody1(contactJoint);

		const NewtonBody *body = body0;

		float mass, Ixx, Iyy, Izz;
		NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);
		if (mass == 0.0f)
		{
			body = body1;
		}

		CCharacterController *Character = (CCharacterController*) NewtonBodyGetUserData(body);

		for (void* contact = NewtonContactJointGetFirstContact(contactJoint); contact; contact = NewtonContactJointGetNextContact(contactJoint, contact))
		{
			NewtonMaterial* material = NewtonContactGetMaterial (contact);
			NewtonMaterialSetContactElasticity(material, 0.3f);

			Character->CharLevelCollision(contact, material);
		}
		*/
	}

	struct CharacterRayCastData
	{
		NewtonBody* Me;
		float Parameter;

		CharacterRayCastData (NewtonBody* Body)
		{
			Me = Body;
			Parameter = 1.2f;
		}
	};

	float CCharacterController::FindFloor(CVector3 Centre, float MaxDist)
	{
		CharacterRayCastData Data (GetNewtonBody());
		
		// shot a vertical ray from a high altitude and collected the intersection parameter.
		CVector3 p1 (Centre); 
		p1.y -= MaxDist;

		NewtonWorldRayCast (Engine->Physics.GetWorld(), &Centre.x, &p1.x, OnRayCastHit, &Data, NULL);
	//	_ASSERTE (data.m_parameter < 1.0f);

		// the intersection is the interpolated value
		return Centre.y - MaxDist * Data.Parameter;
	}

	float CCharacterController::OnRayCastHit(const NewtonBody* Body, const float* Normal, int CollisionID, void* UserData, float IntersetParam)
	{
		float paramPtr;
		CharacterRayCastData& data = *((CharacterRayCastData*) UserData);

		// get the graphic object form the rigid body
		paramPtr = 1.2f;
		// any body can be a floor
		if (data.Me != Body)
		{
			if (IntersetParam < data.Parameter)
			{
				data.Parameter = IntersetParam;
				paramPtr = IntersetParam;
			}
		}
		// else continue the search
		return paramPtr;
	}

}