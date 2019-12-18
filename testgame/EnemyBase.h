#pragma once

#include "CharacterBase.h"

class CEnemyBase : public CCharacterBase
{
	public:
		CEnemyBase();
		~CEnemyBase();

		bool Init(CCharacterBase *Player);

		void Update();

	private:
		sym::CSkeletalAnimation WalkAnim;
		sym::CSkeletalAnimation IdleAnim;

		CCharacterBase *Target;

		float AlertRadius, TurnSpeed, RunSpeed;
};