#pragma once

#include "SYM_Core.h"

#include "Spawn.h"
#include "Inventory.h"

//Fire modes
const int FIRE_PRIMARY = 0;
const int FIRE_SECONDARY = 1;

//Ammo types
const int AMMO_RAYCAST		= 0;
const int AMMO_PROJECTILE	= 1;
const int AMMO_TOGGLE		= 2;

class CCharacterBase;

class CWeaponBase : public CSpawnable, public CInventoryItemBase
{
	public:
		CWeaponBase();
		~CWeaponBase();

		//Fire 
		virtual bool Fire(int FireMode) { return false; }

		//Sound functions
		bool AddFireSound(std::string Filename);
		bool AddEmptyFireSound(std::string Filename);
		bool AddReloadSound(std::string Filename);

		//Set timing
		void SetRefireRate(float TimeSeconds);
		void SetReloadSpeed(float Seconds);

		//Set max ammo (0 for unlimited)
		void SetMaxAmmo(int Ammo);

		//Add ammunition
		void AddAmmo(int Size);

		//Get ammunition type
		int GetAmmoType();

		//Reload
		void Reload();

		//Owner
		void SetOwner(CCharacterBase *Character);
		CCharacterBase *GetOwner();

	protected:
		//Max ammo
		int MaxAmmo;

		//Current Ammo
		int CurrentAmmo;

		//Ammo fire type
		int AmmoType;

		//Timers
		sym::CTimer RefireTimer;
		sym::CTimer ReloadTimer;

		//Owner
		CCharacterBase *Owner;

		//Sounds
		std::vector<sym::CSound> FireSounds;
		std::vector<sym::CSound> EmptyFireSounds;
		std::vector<sym::CSound> ReloadSounds;
};