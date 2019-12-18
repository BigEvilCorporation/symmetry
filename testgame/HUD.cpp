#include "HUD.h"

void CHUD::Init()
{
	Open = false;
	CurrentWeapon = 0;
	CurrentHealth = 0;

	WeaponSelectedMinigun.Load("MiniGunHighlighted.png");
	WeaponUnSelectedMinigun.Load("MiniGunUnselected.png");
	WeaponSelectedLaser.Load("LasersHighlighted.png");
	WeaponUnselectedLaser.Load("LasersUnSelected.png");
	WeaponSelectedTurret.Load("TurretLauncherHighlighted.png");
	WeaponUnselectedTurret.Load("TurretLauncherUnSelected.png");
	WeaponSelectedRocket.Load("RocketHighlighted.png");
	WeaponUnselectedRocket.Load("RocketUnSelected.png");

	float CentreX = (float)sym::Engine->ScreenWidth / 2.0f;
	float CentreY = (float)sym::Engine->ScreenHeight / 2.0f;

	float TexWidth = WeaponSelectedMinigun.GetSize().x / 2.0f;
	float TexHeight = WeaponSelectedMinigun.GetSize().y / 2.0f;

	float XOffset = 64.0f;
	float YOffset = 64.0f;

	//Bottom
	WeaponSelectedMinigun.SetPosition(sym::CVector3(CentreX, CentreY - TexHeight - YOffset, 0.0f));
	WeaponUnSelectedMinigun.SetPosition(sym::CVector3(CentreX, CentreY - TexHeight - YOffset, 0.0f));

	//Right
	WeaponSelectedLaser.SetPosition(sym::CVector3(CentreX + TexWidth + XOffset, CentreY, 0.0f));
	WeaponUnselectedLaser.SetPosition(sym::CVector3(CentreX + TexWidth + XOffset, CentreY, 0.0f));

	//Top
	WeaponSelectedTurret.SetPosition(sym::CVector3(CentreX, CentreY + TexHeight + YOffset, 0.0f));
	WeaponUnselectedTurret.SetPosition(sym::CVector3(CentreX, CentreY + TexHeight + YOffset, 0.0f));

	//Left
	WeaponSelectedRocket.SetPosition(sym::CVector3(CentreX - TexWidth - XOffset, CentreY, 0.0f));
	WeaponUnselectedRocket.SetPosition(sym::CVector3(CentreX - TexWidth - XOffset, CentreY, 0.0f));
}

void CHUD::Update()
{
}

void CHUD::Render()
{
	if(Open)
	{
		if(CurrentWeapon == 0) WeaponSelectedMinigun.Render();
		else WeaponUnSelectedMinigun.Render();

		if(CurrentWeapon == 1) WeaponSelectedLaser.Render();
		else WeaponUnselectedLaser.Render();

		if(CurrentWeapon == 2) WeaponSelectedTurret.Render();
		else WeaponUnselectedTurret.Render();

		if(CurrentWeapon == 3) WeaponSelectedRocket.Render();
		else WeaponUnselectedRocket.Render();
	}
}

void CHUD::SelectWeapon(int Weapon)
{
	CurrentWeapon = Weapon;
	Open = true;
}

void CHUD::CloseWeaponSelector()
{
	Open = false;
}

bool CHUD::IsOpen()
{
	return Open;
}

void CHUD::SetHealth(int Health)
{
}