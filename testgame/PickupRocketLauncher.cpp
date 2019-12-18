#include "PickupRocketLauncher.h"
#include "WeaponRocketLauncher.h"
#include "Game.h"

void CPickupRocketLauncher::OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation)
{
	//Load model
	Load("RocketLauncherPickup", "grapplepickup.sym.mesh", sym::NO_PHYSICS);

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

void CPickupRocketLauncher::MaterialAABBOverlapCallback(CPhysicsBody *HitBody)
{
	//Check a player hit the trigger
	CCharacterBase *Character = (CCharacterBase*)HitBody;

	if(!Character)
	{
		//Something went wrong
		return;
	}

	//Triggered by player, spawn and give a grapple gun
	CRocketLauncher *RocketLauncher = Game->GetCurrentLevel()->Spawn<CRocketLauncher>(Character->GetPosition(), Character->GetRotation());
	RocketLauncher->SetOwner(Character);
	Character->AddInventoryItem((CInventoryItemBase*)RocketLauncher);
	Character->SelectWeapon1("RocketLauncher");

	//Destroy this pickup
	Game->GetCurrentLevel()->DestroySpawnedObject(this);
}
		
