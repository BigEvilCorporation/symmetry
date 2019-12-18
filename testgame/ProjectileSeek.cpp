#include "ProjectileSeek.h"
#include "Explosion.h"
#include "Game.h"

CProjectileSeek::CProjectileSeek()
{
	//Set defaults
	ModelFilename = "projectile.sym.mesh";
	Speed = 10.0f;
	SteerSpeed = 1.0f;
	ExplosionForce = 1000.0f;
	ExplosionDiameter = 1000.0f;
	KillMe = false;
};

void CProjectileSeek::OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation)
{
	//Load model
	Load("Projectile", ModelFilename, sym::PHYSICS_OBJ_BOX);

	//Load audio
	ExplosionSound = sym::Engine->ResourceManager.Audio.Get("sound_wpn_rocket_expl01.wav");

	//Set position and orientation
	SetPosition(Position);
	SetRotation(Rotation);

	std::stringstream Stream;
	Stream << "Projec forward: " << GetForward().x << "\t" << GetForward().y << "\t" << GetForward().z;
	Game->Log.AddEntry(Stream.str());

	//Create unique physics material
	CreateNewtonMaterial();

	//Set collision callback
	SetMaterialCollisionCallback(NewtonMaterialGetDefaultGroupID(sym::Engine->Physics.GetWorld()));

	SetForceTorqueCallback();

	NewtonBodySetUserData(GetNewtonBody(), this);
}

void CProjectileSeek::OnUpdate()
{
	if(KillMe) Game->GetCurrentLevel()->DestroySpawnedObject(this);
}

void CProjectileSeek::OnRender()
{
}

void CProjectileSeek::OnDestroy()
{
}

void CProjectileSeek::ApplyForceAndTorque()
{
	//Target rotation
	sym::CQuaternion TargetRotation;
	TargetRotation.FromLookAt(GetForward(), GetPosition(), Target, SteerSpeed * sym::Engine->GetDeltaTime());

	sym::CVector3 Force = GetForward() * Speed;
	sym::CVector3 Torque = TargetRotation.ToEuler() * Speed;

	//AddTorque(TargetRotation.ToEuler());
	//AddForce(ForwardForce);

	//NewtonBodyAddTorque(GetNewtonBody(), &Torque.x);
	NewtonBodyAddForce(GetNewtonBody(), &Force.x);

	//Force = GetForward() * Speed;
	//NewtonBodyAddForce(GetNewtonBody(), &Force.x);

	/*
	Ogre::Vector3 goalHeading = me->mDesiredHeading;
	goalHeading.y = 0.0f;
	goalHeading.normalise();

	Ogre::Vector3 curHeading = bodyorient * Ogre::Vector3::UNIT_Z;

	// angle between these vectors.
	Ogre::Degree delta_o = Math::ACos( curHeading.dotProduct( goalHeading ) );
	Ogre::Vector3 rot_axis = curHeading.crossProduct( goalHeading ).normalisedCopy();
	body->addTorque( (rot_axis * delta_o.valueDegrees() * 40.0f) - (body->getOmega()*100.0f) );
	*/
}

void CProjectileSeek::MaterialCollisionCallback(CPhysicsBody *HitBody, sym::CVector3 HitPosition, sym::CVector3 Normal)
{
	//Spawn explosion
	//CExplosionField *Explosion= Game->GetCurrentLevel()->Spawn<CExplosionField>(GetPosition(), sym::CQuaternion());
	//Explosion->SetStrength(ExplosionForce);
	//Explosion->SetDiameter(ExplosionDiameter);

	//Playe explosion audio
	if(ExplosionSound) ExplosionSound->Play();

	//Destroy this projectile
	KillMe = true;
}
