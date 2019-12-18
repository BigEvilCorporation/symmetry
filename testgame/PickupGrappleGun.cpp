#include "PickupGrappleGun.h"
#include "WeaponGrappleGun.h"
#include "Game.h"

void CPickupGrappleGun::OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation)
{
	//Load model
	Load("GrapplePickup", "grapplepickup.sym.mesh", sym::NO_PHYSICS);

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

void CPickupGrappleGun::MaterialAABBOverlapCallback(CPhysicsBody *HitBody)
{
	//Check a player hit the trigger
	CCharacterBase *Character = (CCharacterBase*)HitBody;

	if(!Character)
	{
		//Something went wrong
		return;
	}

	//Triggered by player, spawn and give a grapple gun
	CGrappleGun *GrappleGun = Game->GetCurrentLevel()->Spawn<CGrappleGun>(Character->GetPosition(), Character->GetRotation());
	GrappleGun->SetOwner(Character);
	Character->AddInventoryItem((CInventoryItemBase*)GrappleGun);
	Character->SelectWeapon1("GrappleGun");

	//Destroy this pickup
	Game->GetCurrentLevel()->DestroySpawnedObject(this);
}
		
