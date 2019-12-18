#include "CharacterBase.h"

#include "Game.h"

CCharacterBase::CCharacterBase()
{
	//Set default values
	MaxHealth = 100;
	Health = 100;
	CurrentWeapons[0] = 0;
	CurrentWeapons[1] = 0;
	WeaponToggle1 = false;
	WeaponToggle2 = false;

	//New inventory
	Inventory = new CInventory;
}

CCharacterBase::~CCharacterBase()
{
	//Destroy inventory
	if(Inventory) delete Inventory;
}

void CCharacterBase::Update()
{

}

void CCharacterBase::SetAimVector(sym::CVector3 Vector)
{
	Aiming = Vector;
}

sym::CQuaternion CCharacterBase::GetAimQuaternion()
{
	//Create look-at matrix from aim vector
	sym::CMatrix4 Mat(GetPosition(), GetUp(), GetAimVector());

	//Convert to quaternion
	sym::CQuaternion Quat;
	Quat.FromMatrix(Mat);

	return Quat;
}

sym::CVector3 CCharacterBase::GetAimVector()
{
	return Aiming;
}

bool CCharacterBase::AddFootstepSound(std::string Filename)
{
	sym::CSound Sound;
	if(!Sound.Load(Filename, sym::SOUND_3D)) return false;

	FootstepSounds.push_back(Sound);

	return true;
}

bool CCharacterBase::AddJumpSound(std::string Filename)
{
	sym::CSound Sound;
	if(!Sound.Load(Filename, sym::SOUND_3D)) return false;

	JumpSounds.push_back(Sound);

	return true;
}

bool CCharacterBase::AddLandSound(std::string Filename)
{
	sym::CSound Sound;
	if(!Sound.Load(Filename, sym::SOUND_3D)) return false;

	LandingSounds.push_back(Sound);

	return true;
}

bool CCharacterBase::AddDamagedSound(std::string Filename)
{
	sym::CSound Sound;
	if(!Sound.Load(Filename, sym::SOUND_3D)) return false;

	DamageSounds.push_back(Sound);

	return true;
}

bool CCharacterBase::AddDeathSound(std::string Filename)
{
	sym::CSound Sound;
	if(!Sound.Load(Filename, sym::SOUND_3D)) return false;

	DeathSounds.push_back(Sound);
	return true;
}

void CCharacterBase::SetFootstepSoundTimeOffset(float LeftSeconds, float RightSeconds)
{
	FootstepSoundTimers[0].SetTime(LeftSeconds / 1000.0f);
	FootstepSoundTimers[1].SetTime(RightSeconds / 1000.0f);
}

bool CCharacterBase::SetWeaponBone1(std::string BoneName)
{
	//Check a mesh has been loaded
	if(Mesh)
	{
		//Get bone
		sym::CBone *Bone = Mesh->GetBoneByName(BoneName);

		//Check bone exists
		if(Bone)
		{
			//Set bone
			WeaponBones[0] = Bone;

			return true;
		}
		else
		{
			//Bone does not exist
			Game->Log.AddEntry("Error: Bone " + BoneName = " does not exist on character " + Name);
			return false;
		}
	}
	else
	{
		//Mesh does not exist
		Game->Log.AddEntry("Error: SetWeaponBone1 called before mesh has been loaded on character " + Name);
		return false;
	}
}

bool CCharacterBase::SetWeaponBone2(std::string BoneName)
{
	//Check a mesh has been loaded
	if(Mesh)
	{
		//Get bone
		sym::CBone *Bone = Mesh->GetBoneByName(BoneName);

		//Check bone exists
		if(Bone)
		{
			//Set bone
			WeaponBones[1] = Bone;

			return true;
		}
		else
		{
			//Bone does not exist
			Game->Log.AddEntry("Error: Bone " + BoneName = " does not exist on character " + Name);
			return false;
		}
	}
	else
	{
		//Mesh does not exist
		Game->Log.AddEntry("Error: SetWeaponBone2 called before mesh has been loaded on character " + Name);
		return false;
	}
}

sym::CBone *CCharacterBase::GetWeaponBone1()
{
	return WeaponBones[0];
}

sym::CBone *CCharacterBase::GetWeaponBone2()
{
	return WeaponBones[1];
}

bool CCharacterBase::AddInventoryItem(CInventoryItemBase *Item)
{
	//Check inventory exists
	if(Inventory)
	{
		//Add the item
		if(!Inventory->AddItem(Item))
		{
			//Not enough space
			Game->Log.AddEntry("Error: Could not add item " + Item->GetName() + ", inventory full");
		}
	}
	else
	{
		//Inventory doesn't exist
		Game->Log.AddEntry("Error: Inventory doesn't exist, cannot add item " + Name);
		return false;
	}

	return true;
}

bool CCharacterBase::DropInventoryItem(std::string Name)
{
	//Check inventory exists
	if(Inventory)
	{
		//Get pointer to inventory item
		CInventoryItemBase *Item = Inventory->GetItemByName(Name);

		//Check item exists
		if(!Item)
		{
			//Item not in inventory
			Game->Log.AddEntry("Error: Cannot remove inventory item " + Name + ", item doesn't exist");
			return false;
		}

		//Remove item from inventory
		if(!Inventory->RemoveItem(Item))
		{
			//Error removing item
			Game->Log.AddEntry("Error: Cannot remove inventory item " + Name);
			return false;
		}
	}
	else
	{
		//Inventory doesn't exist
		Game->Log.AddEntry("Error: Inventory doesn't exist, cannot remove item " + Name);
		return false;
	}

	return true;
}

CInventoryItemBase *CCharacterBase::GetInventoryItemByName(std::string Name)
{
	//Check inventory exists
	if(Inventory)
	{
		//Get pointer to inventory item
		CInventoryItemBase *Item = Inventory->GetItemByName(Name);

		//Check item exists
		if(!Item)
		{
			//Item not in inventory
			Game->Log.AddEntry("Error: Cannot get inventory item " + Name + ", item doesn't exist");
			return 0;
		}

		//Return item
		return Item;
	}
	else
	{
		//Inventory doesn't exist
		Game->Log.AddEntry("Error: Inventory doesn't exist, cannot get item " + Name);
		return 0;
	}
}

bool CCharacterBase::SelectWeapon1(std::string Name)
{
	//Get weapon from inventory
	CInventoryItemBase *Weapon = GetInventoryItemByName(Name);

	//If weapons doesn't exist, return false
	if(!Weapon) return false;

	//Set current weapon
	CurrentWeapons[0] = (CWeaponBase*)Weapon;

	//Set weapon owner
	CurrentWeapons[0]->SetOwner(this);

	return true;
}

bool CCharacterBase::SelectWeapon2(std::string Name)
{
	//Get weapon from inventory
	CInventoryItemBase *Weapon = GetInventoryItemByName(Name);

	//If weapons doesn't exist, return false
	if(!Weapon) return false;

	//Set current weapon
	CurrentWeapons[1] = (CWeaponBase*)Weapon;

	//Set weapon owner
	CurrentWeapons[1]->SetOwner(this);

	return true;
}

void CCharacterBase::ShootWeapon1(int FireMode)
{
	if(CurrentWeapons[0]) CurrentWeapons[0]->Fire(FireMode);
}

void CCharacterBase::ShootWeapon2(int FireMode)
{
	if(CurrentWeapons[1]) CurrentWeapons[1]->Fire(FireMode);
}

void CCharacterBase::AddHealth(int Amount)
{
	Health += Amount;
	if(Health > MaxHealth) Health = MaxHealth;
}

void CCharacterBase::SetMaxHealth(int Amount)
{
	MaxHealth = Amount;
}

void CCharacterBase::TakeDamage(int Amount, CCharacterBase *DamagedBy, int DamageType, sym::CVector3 Normal)
{
	//Decrease health
	Health -= Amount;

	//Play random damage sound
	if((int)DamageSounds.size() > 0)
	{
		int Sound = rand() % (int)DamageSounds.size() - 1;
		DamageSounds[Sound].Play();
	}

	//If player is dead
	if(Health <= 0)
	{
		//Kill
		Health = 0;
		Die(DamageType);
	}
}

void CCharacterBase::Die(int Damagetype)
{
	//Play random damage sound
	if((int)DamageSounds.size() > 0)
	{
		int Sound = rand() % (int)DamageSounds.size() - 1;
		DamageSounds[Sound].Play();
	}
}