#pragma once

#include "SYM_Core.h"
#include "Game.h"
#include "WeaponBase.h"
#include "WeaponMiniTurret.h"

class CTurretGun : public CWeaponBase
{
	public:
		//Load model and set physics type
		void OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation);

		void OnUpdate();
		void OnRender();
		void OnDestroy();

		bool Fire(int FireMode);

		//Set list of targets
		void SetTargets(std::vector<sym::CGeometry*> Targets);

	private:

		//Vector of targets
		std::vector<sym::CGeometry*> TargetList;

		//Sounds
		sym::CSound *FireSound;

		//Tracking dot
		sym::CSprite Dot;
};
