#include "PickupBase.h"
#include "Game.h"

CPickupBase::CPickupBase()
{
	TriggerHeight = 20.0f;
	TriggerRadius = 10.0f;
	RotateSpeed = 0.1f;
}

CPickupBase::~CPickupBase()
{

}

void CPickupBase::OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation)
{
	//Zero mass
	SetMass(0.0f);
}

void CPickupBase::OnUpdate()
{
	//Update model position
	//Model->SetNewtonMatrix(GetMatrix());

	//Rotate
	Yaw(sym::Engine->GetDeltaTime() * RotateSpeed);
}

void CPickupBase::OnDestroy()
{
}

int CPickupBase::TriggerCallback(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1, int threadIndex)
{
	return 0;
}