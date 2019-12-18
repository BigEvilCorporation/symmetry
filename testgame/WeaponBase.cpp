#include "WeaponBase.h"

CWeaponBase::CWeaponBase()
{
	//Set default values
	Owner = 0;
	MaxAmmo = 100;
	CurrentAmmo = 50;
	AmmoType = AMMO_RAYCAST;
	RefireTimer.SetTime(500.0f);
	ReloadTimer.SetTime(2000.0f);
}

CWeaponBase::~CWeaponBase()
{
	
}

bool CWeaponBase::AddFireSound(std::string Filename)
{
	sym::CSound Sound;
	if(!Sound.Load(Filename, sym::SOUND_3D)) return false;

	FireSounds.push_back(Sound);

	return true;
}

bool CWeaponBase::AddEmptyFireSound(std::string Filename)
{
	sym::CSound Sound;
	if(!Sound.Load(Filename, sym::SOUND_3D)) return false;

	EmptyFireSounds.push_back(Sound);

	return true;
}

bool CWeaponBase::AddReloadSound(std::string Filename)
{
	sym::CSound Sound;
	if(!Sound.Load(Filename, sym::SOUND_3D)) return false;

	ReloadSounds.push_back(Sound);

	return true;
}

void CWeaponBase::SetRefireRate(float TimeSeconds)
{
	RefireTimer.SetTime(TimeSeconds / 1000.0f);
}

void CWeaponBase::SetReloadSpeed(float Seconds)
{
	ReloadTimer.SetTime(Seconds / 1000.0f);
}

void CWeaponBase::Reload()
{
	//If reload timer has expired, play reload sound, start reload timer
}

void CWeaponBase::SetMaxAmmo(int Ammo)
{
	MaxAmmo = Ammo;
}

void CWeaponBase::AddAmmo(int Size)
{
	//Add ammo
	CurrentAmmo += Size;

	//Restrict size
	if(CurrentAmmo > MaxAmmo) CurrentAmmo = MaxAmmo;
}

int CWeaponBase::GetAmmoType()
{
	return AmmoType;
}

void CWeaponBase::SetOwner(CCharacterBase *Character)
{
	Owner = Character;
}

CCharacterBase *CWeaponBase::GetOwner()
{
	return Owner;
}