#pragma once

#include "SYM_Core.h"
#include "Game.h"
#include "WeaponBase.h"
#include "ProjectileBase.h"
#include "ProjectileSeek.h"

class CRocketLauncher : public CWeaponBase
{
	public:
		//Load model and set physics type
		void OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation);

		void OnUpdate();
		void OnRender();
		void OnDestroy();

		bool Fire(int FireMode);

	private:

		//Sounds
		sym::CSound *FireSound;

		//Tracking dot
		sym::CSprite Dot;
};
