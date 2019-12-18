#pragma once

#include "SYM_Core.h"

#include "WeaponBase.h"
#include "Inventory.h"

//Damage types
const int DAMAGE_GUNFIRE	= 0;
const int DAMAGE_IMPACT		= 1;
const int DAMAGE_BURNING	= 2;
const int DAMAGE_DROWNING	= 3;
const int DAMAGE_SUICIDE	= 4;
const int DAMAGE_CRUSHING	= 5;
const int DAMAGE_MELEE		= 6;

class CCharacterBase : public sym::CCharacterController
{
	public:
		CCharacterBase();
		~CCharacterBase();

		//Sound functions
		bool AddFootstepSound(std::string Filename);
		bool AddJumpSound(std::string Filename);
		bool AddLandSound(std::string Filename);
		bool AddDamagedSound(std::string Filename);
		bool AddDeathSound(std::string Filename);

		void SetFootstepSoundTimeOffset(float LeftSeconds, float RightSeconds);
		
		//Update character
		void Update();

		/*
		//Update think-tank
		virtual void UpdateAI();

		//Movement functions
		virtual void Walk();
		virtual void Run();
		virtual void Stepback();
		virtual void StepLeft();
		virtual void StepRight();
		virtual void Turn(float Degrees);
		virtual void Jump();
		*/

		//Action functions
		void ShootWeapon1(int FireMode);
		void ShootWeapon2(int FireMode);

		//Weapon attachment
		bool SetWeaponBone1(std::string BoneName);
		bool SetWeaponBone2(std::string BoneName);
		sym::CBone *GetWeaponBone1();
		sym::CBone *GetWeaponBone2();

		//Inventory functions
		bool AddInventoryItem(CInventoryItemBase *Item);
		bool DropInventoryItem(std::string Name);
		CInventoryItemBase *GetInventoryItemByName(std::string Name);
		bool SelectWeapon1(std::string Name);
		bool SelectWeapon2(std::string Name);

		//AI functions
		//virtual void WalkTo(sym::CVector3 Position);

		//Health functions
		void AddHealth(int Amount);
		void SetMaxHealth(int Amount);
		void TakeDamage(int Amount, CCharacterBase *DamagedBy, int DamageType, sym::CVector3 Normal);
		void Die(int Damagetype);

		int GetHealth();
		int GetMaxHealth();

		//Get aiming vector (unit length)
		void SetAimVector(sym::CVector3 Vector);
		sym::CVector3 GetAimVector();
		sym::CQuaternion GetAimQuaternion();

	protected:
		//CInventory
		CInventory *Inventory;

		//Current inventory item
		CInventoryItemBase *CurrentItem;

		//Current weapons (one per hand)
		CWeaponBase *CurrentWeapons[2];

		//Attachment bones
		sym::CBone *WeaponBones[2];

		//Toggle-able weapon bool
		bool WeaponToggle1;
		bool WeaponToggle2;

		//Health
		int Health;
		int MaxHealth;

		//Aiming direction vector
		sym::CVector3 Aiming;

		//Weapon aim bone
		sym::CBone *AimBone;

		//Sounds
		std::vector<sym::CSound> FootstepSounds;
		std::vector<sym::CSound> JumpSounds;
		std::vector<sym::CSound> LandingSounds;
		std::vector<sym::CSound> DamageSounds;
		std::vector<sym::CSound> DeathSounds;

		//Timers
		sym::CTimer FootstepSoundTimers[2];

		//Animations
		sym::CSkeletalAnimation *RunAnimation;
		sym::CSkeletalAnimation *TurnAnimation;
		sym::CSkeletalAnimation *SidestepAnimation;
		sym::CSkeletalAnimation *FireAnimation;
};