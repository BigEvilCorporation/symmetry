#include "Explosion.h"
#include "Game.h"

CExplosionField::CExplosionField()
{
	ExplodeStrength = 1000.0f;
	ExplodeDiameter = 100.0f;
}

void CExplosionField::SetDiameter(float Diameter)
{
	ExplodeDiameter = Diameter;
}

void CExplosionField::SetStrength(float Strength)
{
	ExplodeStrength = Strength;
}

void CExplosionField::OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation)
{
	//Load("Explosion", "explosion_field.sym.mesh", sym::NO_PHYSICS);

	//Create spherical trigger
	CreateSphericalTrigger(sym::CVector3(ExplodeDiameter, ExplodeDiameter, ExplodeDiameter));

	//Set position
	SetPosition(Position);

	//Create unique material
	CreateNewtonMaterial();

	//Set callback
	SetMaterialCollisionCallback(NewtonMaterialGetDefaultGroupID(sym::Engine->Physics.GetWorld()));
}

void CExplosionField::OnUpdate()
{
	Game->GetCurrentLevel()->DestroySpawnedObject(this);
}

void CExplosionField::OnRender()
{
}

void CExplosionField::MaterialCollisionCallback(CPhysicsBody *HitBody)
{
	//If body is a trigger volume, ignore
	if(NewtonCollisionIsTriggerVolume(NewtonBodyGetCollision(HitBody->GetNewtonBody()))) return;

	sym::CGeometry *Geom = (CGeometry*)HitBody;

	if(!Geom) return;

	//Make sure body is unfrozen
	NewtonBodySetFreezeState(HitBody->GetNewtonBody(), 0);

	//Calculate force
	float Magnitude;
	sym::CVector3 Force(Geom->GetPosition() - GetPosition());

	Magnitude = Force.Dot(Force);
	Magnitude = ExplodeStrength / (Magnitude * sqrt(Magnitude) + 0.1f);
	Force = Force * (Magnitude);

	//Set force
	NewtonBodyAddForce(HitBody->GetNewtonBody(), &Force.x);
}