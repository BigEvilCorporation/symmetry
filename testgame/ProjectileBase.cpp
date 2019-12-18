#include "ProjectileBase.h"
#include "Explosion.h"
#include "Game.h"

CProjectileBase::CProjectileBase()
{
	//Set defaults
	ModelFilename = "rocket.sym.mesh";
	Speed = 50.0f;
	ExplosionForce = 1000.0f;
	ExplosionDiameter = 1000.0f;
	KillMe = false;
};

void CProjectileBase::OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation)
{
	//Load model
	Load("Projectile", ModelFilename, sym::PHYSICS_OBJ_BOX);

	//Set position and orientation
	SetPosition(Position);
	SetRotation(Rotation);

	//Load audio
	ExplosionSound = sym::Engine->ResourceManager.Audio.Get("sound_wpn_rocket_expl01.wav");

	//Create unique physics material
	CreateNewtonMaterial();

	//Set collision callback
	SetMaterialCollisionCallback(NewtonMaterialGetDefaultGroupID(sym::Engine->Physics.GetWorld()));

	//SetForceTorqueCallback();

	NewtonBodySetUserData(GetNewtonBody(), this);

	Direction = GetForward();
}

void CProjectileBase::OnUpdate()
{
	if(KillMe) Game->GetCurrentLevel()->DestroySpawnedObject(this);
	else SetVelocity(Direction * Speed);
}

void CProjectileBase::OnRender()
{
}

void CProjectileBase::OnDestroy()
{
}

void CProjectileBase::ApplyForceAndTorque()
{
	Force = Direction * Speed;
	NewtonBodyAddForce(GetNewtonBody(), &Force.x);
}

void CProjectileBase::MaterialCollisionCallback(CPhysicsBody *HitBody, sym::CVector3 HitPosition, sym::CVector3 Normal)
{
	//Spawn explosion
	CExplosionField *Explosion= Game->GetCurrentLevel()->Spawn<CExplosionField>(GetPosition(), sym::CQuaternion());
	Explosion->SetStrength(ExplosionForce);
	Explosion->SetDiameter(ExplosionDiameter);

	//Play explosion audio
	if(ExplosionSound) ExplosionSound->Play();

	//Destroy this projectile
	KillMe = true;
}
