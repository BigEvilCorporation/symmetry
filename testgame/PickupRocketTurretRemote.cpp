#include "PickupRocketTurretRemote.h"
#include "WeaponRocketTurret.h"
#include "Game.h"

void CPickupRocketTurretRemote::OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation)
{
	//Load model
	Load("RocketTurretRemotePickup", "grapplepickup.sym.mesh", sym::NO_PHYSICS);

	//Set position and orientation
	SetPosition(Position);
	SetRotation(Rotation);

	//Create trigger volume
	CreateCylinderTrigger(sym::CVector3(TriggerRadius, TriggerHeight, 0.0f));

	//Override material collision callback
	SetMaterialAABBOverlapCallback(Game->GetCurrentLevel()->GetPlayer()->GetNewtonMaterial());

	NewtonBodySetUserData(GetNewtonBody(), this);

	//Call original spawn
	CPickupBase::OnSpawn(Position, Rotation);
}

void CPickupRocketTurretRemote::MaterialAABBOverlapCallback(CPhysicsBody *HitBody)
{
	//Check a player hit the trigger
	CCharacterBase *Character = (CCharacterBase*)HitBody;

	if(!Character)
	{
		//Something went wrong
		return;
	}

	//Triggered by player, spawn and give a grapple gun
	CRocketTurretRemote *Remote = Game->GetCurrentLevel()->Spawn<CRocketTurretRemote>(Character->GetPosition(), Character->GetRotation());
	Remote->SetOwner(Character);
	Character->AddInventoryItem((CInventoryItemBase*)Remote);
	Character->SelectWeapon1("RocketTurretRemote");

	//Destroy this pickup
	Game->GetCurrentLevel()->DestroySpawnedObject(this);
}
		
