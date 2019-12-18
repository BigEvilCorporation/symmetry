#pragma once

#include "SYM_Core.h"
#include "Game.h"
#include "WeaponBase.h"

class CLaser : public CWeaponBase
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

		//Fade out speed
		float FadeOutSpeed;
		float CurrentAlpha;

		//Max fire length
		float MaxLength;

		//Laser positions
		sym::CVector3 StartPos, EndPos;
};
