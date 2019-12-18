#include "HoverGame.h"

struct RayCastData
{
	NewtonBody* Me;
	float Parameter;

	RayCastData (NewtonBody* Body)
	{
		Me = Body;
		Parameter = 1.2f;
	}
};

bool CHovercraft::Create(std::string ModelFilename)
{
	if(!Load("HCraft", ModelFilename, sym::PHYSICS_OBJ_CUSTOM)) return false;

	CurrentSpeed = 0.0f;
	Steering = 0.0;
	Damping = 0.1f;
	Gravity = 9.8;

	SetMass(20.0f);
	SetHoverHeight(10.0f);

	sym::CVector3 Size = Mesh->AABB.Size;

	Thrusters[0] = sym::CVector3( Size.x * 0.6, -Size.y * 0.5,  Size.z * 0.6);
	Thrusters[1] = sym::CVector3(-Size.x * 0.6, -Size.y * 0.5,  Size.z * 0.6);
	Thrusters[2] = sym::CVector3( Size.x * 0.6, -Size.y * 0.5,  0.0);
	Thrusters[3] = sym::CVector3(-Size.x * 0.6, -Size.y * 0.5,  0.0);
	Thrusters[4] = sym::CVector3( Size.x * 0.6, -Size.y * 0.5, -Size.z * 0.6);
	Thrusters[5] = sym::CVector3(-Size.x * 0.6, -Size.y * 0.5, -Size.z * 0.6);

	//Set force/torque callback
	OverrideCallbackForceAndTorque(OnApplyForceAndTorque);
	SetAutoFreeze(false);

	//Set up vector
	sym::CVector3 UpDirection (0.0f, 0.1f, 0.0f);
	//UpVectorJoint = NewtonConstraintCreateUpVector(sym::Engine->Physics.GetWorld(), &UpDirection.x, GetNewtonBody());

	return true;
}

void CHovercraft::SetSpeed(float Speed)
{
	CurrentSpeed = Speed;
}

void CHovercraft::SetSteerTorque(float Torque)
{
	Steering = Torque;
}

void CHovercraft::SetHoverHeight(float Height)
{
	HoverHeight = Height;
	ThrusterForce = (Mass * Gravity / 6) / HoverHeight;
	MaxHoverHeight = 1.25 * HoverHeight + HoverHeight;
}

void CHovercraft::SetMass(float M)
{
	CModel::SetMass(M);
	GravityForce = sym::CVector3(0.0f, -M * Gravity, 0.0f);
}

void CHovercraft::OnApplyForceAndTorque(const NewtonBody* Body, dFloat timestep, int threadIndex)
{
	CHovercraft *ThisCraft = (CHovercraft*) NewtonBodyGetUserData(Body);

	//Get matrix
	sym::CMatrix4 Matrix = ThisCraft->GetMatrix();

	//Get omega
	sym::CVector3 Omega = ThisCraft->GetOmega();

	//Get force direction in global space
	sym::CVector3 ThrusterDir = Matrix.UnrotateVector(sym::CVector3(0, 1, 0));

	sym::CVector3 TotalForce = ThisCraft->GravityForce;
	sym::CVector3 TotalTorque(0, 0, 0);

	//Loop through thrusters
	for(int i = 0; i < 6; i++)
	{
		//Get thruster position in global space
		sym::CVector3 ThrusterPos = Matrix.TransformVector(ThisCraft->Thrusters[i]);

		//Get distance to ground
		float HoverDistance = ThisCraft->FindFloor(ThrusterPos, ThisCraft->MaxHoverHeight);

		//Get magnitude of thruster force
		float ForceMag = 0.0f;
		if(HoverDistance < 1.0f) ForceMag = ThisCraft->ThrusterForce * ThisCraft->HoverHeight * (1.0f - HoverDistance) * 20.0f;

		//Get thruster relative position
		sym::CVector3 ThrusterRelPos = Matrix.RotateVector(ThisCraft->Thrusters[i]);

		//Get thruster velocity
		sym::CVector3 ThrusterVel = Omega.Cross(ThrusterRelPos);

		//Get thruster damp force
		float DampForce = -ThrusterVel.Dot(ThrusterDir) * ThisCraft->Mass * ThisCraft->Damping;

		//Calculate force/torque
		sym::CVector3 ThisForce = ThrusterDir * (ForceMag + DampForce);
		sym::CVector3 ThisTorque = ThrusterRelPos.Cross(ThisForce);

		//Add to total force/torque
		TotalForce += ThisForce;
		TotalTorque += ThisTorque;
	}

	//Scale force by current speed/direction
	TotalForce += ThisCraft->GetForward() * ThisCraft->CurrentSpeed * ThisCraft->Mass;

	//Add steering torque
	TotalTorque += sym::CVector3(0, ThisCraft->Steering * ThisCraft->Mass, 0);

	//Set force and torque
	ThisCraft->SetForce(TotalForce);
	ThisCraft->SetTorque(TotalTorque);

	sym::CVector3 Pin (0.0f, 0.1f, 0.0f);
	//NewtonUpVectorSetPin(ThisCraft->UpVectorJoint, &Pin.x);

	//Call original callback
	sym::CPhysicsBody::PhysicsApplyForceAndTorque(Body, timestep, threadIndex);
}



float CHovercraft::OnRayCastHit(const NewtonBody* Body, const float* Normal, int CollisionID, void* UserData, float IntersetParam)
{
	float paramPtr;
	RayCastData& data = *((RayCastData*) UserData);

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

float CHovercraft::FindFloor(sym::CVector3 Centre, float MaxDist)
{
	RayCastData Data (GetNewtonBody());
	
	sym::CVector3 p1(Centre);
	p1.y -= MaxDist;

	NewtonWorldRayCast (sym::Engine->Physics.GetWorld(), &Centre.x, &p1.x, OnRayCastHit, &Data, NULL);

	return Data.Parameter;
}