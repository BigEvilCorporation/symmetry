#include "WeaponTurretGun.h"

void CTurretGun::OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation)
{
	CInventoryItemBase::SetName("TurretGun");
	AmmoType = AMMO_TOGGLE;

	/*
	//Load base model with fixed level physics
	if(!Load("RocketLauncher", "grapplegun.sym.mesh", sym::NO_PHYSICS))
	{
		Game->Log.AddEntry("Error: Could not load rocket launcher mesh");
	}

	//Position on floor (TODO: Find floor with raycast)
	SetPosition(sym::CVector3(Position.x, GetLocalAABB().Size.y / 2, Position.z));
	*/

	//
	FireSound = sym::Engine->ResourceManager.Audio.Get("sound_wpn_rocket_fire01.wav");

	Dot.Load("rocket_laser.png");
	Dot.SetDoubleSided(true);

	//Timer is reset manually
	RefireTimer.SetLooping(false);
}

void CTurretGun::OnUpdate()
{
	if(Owner)
	{
		//Raycast for tracking dot
		sym::CVector3 Start = Owner->GetPosition() + (Owner->GetForward() * Owner->GetLocalAABB().Size.z);
		sym::CVector3 End = Owner->GetPosition() + (Owner->GetAimVector() * 800.0f);

		//Do raycast
		sym::RaycastData HitData = sym::Engine->Physics.Raycast(Start, End);

		//Sprite position, lerped back slightly
		sym::CVector3 SpritePosition = HitData.HitPosition.Lerp(Owner->GetPosition(), 0.01f);

		//Create lookat quaternion
		sym::CQuaternion LookAtQuat;
		LookAtQuat.FromLookAt(sym::CVector3(0.0f, 0.0f, -1.0f), SpritePosition, SpritePosition + HitData.Normal, 1.0f);

		Dot.SetPosition(SpritePosition);
		Dot.SetRotation(LookAtQuat);
	}
}

void CTurretGun::OnRender()
{
	//Dot.Render();
}

void CTurretGun::OnDestroy()
{
}

void CTurretGun::SetTargets(std::vector<sym::CGeometry*> Targets)
{
	TargetList = Targets;
}

bool CTurretGun::Fire(int FireMode)
{
	if(Owner && FireMode == FIRE_PRIMARY)
	{
		//Spawn projectile
		sym::CVector3 Start = Owner->GetPosition() + (Owner->GetForward() * ((Owner->GetLocalAABB().Size.y / 2.0f) + 50.0f));
		CMiniTurret *Projectile = Game->GetCurrentLevel()->Spawn<CMiniTurret>(Start, Owner->GetRotation());
		Projectile->SetTargets(TargetList);
		Projectile->SetOwner(Owner);
		Projectile->SetMovementSpeed(100.0f);

		//Play audio
		FireSound->Play();

		//Reser refire timer
		RefireTimer.Reset();
	}

	return true;
}