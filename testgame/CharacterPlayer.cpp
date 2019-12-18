#include "CharacterPlayer.h"
#include "Game.h"

CCharacterPlayer::CCharacterPlayer()
{
	Walking = false;
	AimBone = 0;
}

CCharacterPlayer::~CCharacterPlayer()
{
}

bool CCharacterPlayer::Init()
{
	if(!Create("olaf2.sym.mesh"))
	{
		sym::Engine->Logger.AddEntry("Error: Could not load character mesh");
		return false;
	}

	if(!WalkAnim.Load("olaf2_run.sym.animation"))
	{
		sym::Engine->Logger.AddEntry("Error: Could not load walk animation");
		return false;
	}

	if(!IdleAnim.Load("olaf2_idle.sym.animation"))
	{
		sym::Engine->Logger.AddEntry("Error: Could not load idle animation");
		return false;
	}

	if(!JumpAnim.Load("olaf2_jump.sym.animation"))
	{
		sym::Engine->Logger.AddEntry("Error: Could not load jump animation");
		return false;
	}

	if(!WalkBackAnim.Load("olaf2_run_backward.sym.animation"))
	{
		sym::Engine->Logger.AddEntry("Error: Could not load run backwards animation");
		return false;
	}

	if(!StepRightAnim.Load("olaf2_step_right.sym.animation"))
	{
		sym::Engine->Logger.AddEntry("Error: Could not load step right animation");
		return false;
	}

	if(!StepLeftAnim.Load("olaf2_step_left.sym.animation"))
	{
		sym::Engine->Logger.AddEntry("Error: Could not load step left animation");
		return false;
	}

	Mesh->SetAnimSpeed(0.001f);
	Mesh->BindAnimation(&IdleAnim);
	Mesh->StartAnimation();

	//Set mass
	SetMass(10.0f);

	//Find aim bone
	AimBone = Mesh->GetBoneByName("Bip01 R Forearm");
	if(!AimBone) Game->Log.AddEntry("Error: Could not find aim bone (Bip01 R Forearm) on character mesh (olaf.sym.mesh)");

	//Set weapon bones and offsets

	//Set sounds and footstep time offsets

	return true;
}