#include "WeaponLaser.h"
#include "EnemyBase.h"

void CLaser::OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation)
{
	CInventoryItemBase::SetName("Laser");
	AmmoType = AMMO_RAYCAST;

	//FireSound = sym::Engine->ResourceManager.Audio.Get("sound_wpn_rocket_fire01.wav");

	//Timer is reset manually
	RefireTimer.SetLooping(false);

	MaxLength = 600.0f;
	FadeOutSpeed = 0.001f;
	CurrentAlpha = 0.0f;
}

void CLaser::OnUpdate()
{
	//Update alpha
	CurrentAlpha -= FadeOutSpeed * sym::Engine->GetDeltaTime();

	if(CurrentAlpha < 0.0f) CurrentAlpha = 0.0f;
	else
	{
		//Update positions
		StartPos = Owner->GetPosition() + (Owner->GetForward() * ((Owner->GetLocalAABB().Size.y / 2.0f)));
		EndPos = StartPos + (Owner->GetForward() * MaxLength);
	}
}

void CLaser::OnRender()
{
	if(CurrentAlpha > 0.0f)
	{
		//Set beam alpha
		glColor4f(1.0f, 0.0f, 0.0f, CurrentAlpha);

		//Render beam
		glBegin(GL_LINES);
		glVertex3f(StartPos.x, StartPos.y, StartPos.z);
		glVertex3f(EndPos.x, EndPos.y, EndPos.z);
		glEnd();
	}

	CurrentAlpha -= sym::Engine->GetDeltaTime() * FadeOutSpeed;
}

void CLaser::OnDestroy()
{
}

bool CLaser::Fire(int FireMode)
{
	if(Owner && FireMode == FIRE_PRIMARY)
	{
		//Set alpha
		CurrentAlpha = 1.0f;

		//Set positions
		StartPos = Owner->GetPosition() + (Owner->GetForward() * ((Owner->GetLocalAABB().Size.y / 2.0f)));
		EndPos = StartPos + (Owner->GetAimVector() * MaxLength);

		//Raycast
		sym::RaycastData Data = sym::Engine->Physics.Raycast(StartPos, EndPos);

		//Check if any geometry was hit
		if(Data.Geometry)
		{
			//Cast to enemy
			CEnemyBase *Enemy = dynamic_cast<CEnemyBase*>(Data.Geometry);

			//Check if it is an enemy
			if(Enemy)
			{
				//Deal damage
			}
		}

		//Play audio
		//FireSound->Play();

		//Reser refire timer
		RefireTimer.Reset();
	}

	return true;
}