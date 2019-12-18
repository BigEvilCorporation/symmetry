#pragma once

#include "CharacterBase.h"

class CCharacterPlayer : public CCharacterBase
{
	public:
		CCharacterPlayer();
		~CCharacterPlayer();

		bool Init();

		sym::CSkeletalAnimation WalkAnim;
		sym::CSkeletalAnimation WalkBackAnim;
		sym::CSkeletalAnimation StepLeftAnim;
		sym::CSkeletalAnimation StepRightAnim;
		sym::CSkeletalAnimation IdleAnim;
		sym::CSkeletalAnimation JumpAnim;
		bool Walking;

	private:
		
};