#include "EnemyBase.h"

CEnemyBase::CEnemyBase()
{
	Target = 0;
	AlertRadius = 300.0f;
	TurnSpeed = 0.1f;
	RunSpeed = 100.0f;
}

CEnemyBase::~CEnemyBase()
{
}

bool CEnemyBase::Init(CCharacterBase *Player)
{
	Target = Player;

	if(!Create("olaf.sym.mesh"))
	{
		sym::Engine->Logger.AddEntry("Error: Could not load character mesh");
		return false;
	}

	if(!WalkAnim.Load("olaf_run.sym.animation"))
	{
		sym::Engine->Logger.AddEntry("Error: Could not load walk animation");
		return false;
	}

	if(!IdleAnim.Load("olaf_idle.sym.animation"))
	{
		sym::Engine->Logger.AddEntry("Error: Could not load idle animation");
		return false;
	}

	Mesh->SetAnimSpeed(0.001f);
	Mesh->BindAnimation(&IdleAnim);
	Mesh->StartAnimation();

	//Set mass
	SetMass(10.0f);

	return true;
}

void CEnemyBase::Update()
{
	//Check if player is in radius of enemy
	if(Target->GetPosition().GetDistance(GetPosition()) < AlertRadius)
	{
		//Start walk anim
		//Mesh->BindAnimation(&WalkAnim);

		//Move towards player
		//LookAt(GetForward(), Target->GetPosition(), TurnSpeed * sym::Engine->GetDeltaTime());

		sym::CQuaternion Quat;
		Quat.FromLookAt(GetForward(), GetPosition(), Target->GetPosition(), 0.8f);
		float Heading = Quat.ToEuler().y;

		Quat.FromEuler(sym::CVector3(0.0f, Heading, 0.0f));
		SetRotation(Quat);

		//SetMovementVector(GetForward() * RunSpeed);
	}
	else
	{
		Mesh->BindAnimation(&IdleAnim);
	}
}