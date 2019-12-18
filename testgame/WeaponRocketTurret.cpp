#include "WeaponRocketTurret.h"
#include "Explosion.h"

/////////////////////////////////////////////////////////////////////////////////////////
// ROCKET TURRET LAUNCHER
/////////////////////////////////////////////////////////////////////////////////////////
void CRocketTurret::OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation)
{
	//Load base model with fixed level physics
	if(!Load("RocketTurret", "remote_rocket_launcher.sym.mesh", sym::LEVEL_COLLISION))
	{
		Game->Log.AddEntry("Error: Could not load remote rocket launcher mesh");
	}

	//Position on floor (TODO: Find floor with raycast)
	SetPosition(sym::CVector3(Position.x, GetLocalAABB().Size.y / 2, Position.z));
	SetRotation(Rotation);

	ProjectileSpawnPoint = GetPosition() + sym::CVector3(0.0f, GetLocalAABB().Size.y / 2, 0.0f);
	ProjectileDirection = GetRotation() + sym::CQuaternion(0.0f, 90.0f, 0.0f, 0.0f);

	FireSound.Load("rocketturret_fire.wav", false, false);
}

void CRocketTurret::OnUpdate()
{
}

void CRocketTurret::OnRender()
{
}

void CRocketTurret::OnDestroy()
{
}

void CRocketTurret::Fire(sym::CVector3 TargetPos)
{
	CRocketTurretProjectile *Projectile = Game->GetCurrentLevel()->Spawn<CRocketTurretProjectile>(ProjectileSpawnPoint, ProjectileDirection);
	Projectile->SetTarget(TargetPos);
	FireSound.Play();
}

/////////////////////////////////////////////////////////////////////////////////////////
// ROCKET TURRET REMOTE CONTROL
/////////////////////////////////////////////////////////////////////////////////////////
void CRocketTurretRemote::OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation)
{
	CInventoryItemBase::SetName("RocketTurretRemote");
	AmmoType = AMMO_TOGGLE;
	Aiming = false;
	HasTarget = false;
	MaxFireDistance = 500.0f;
	MaxWirelessRange = 500.0f;
	MessageTimer.SetTime(3000.0f);

	//Load base model with fixed level physics
	if(!Load("RocketTurretRemote", "grapplegun.sym.mesh", sym::NO_PHYSICS))
	{
		Game->Log.AddEntry("Error: Could not load remote rocket launcher mesh");
	}

	//Position on floor (TODO: Find floor with raycast)
	SetPosition(sym::CVector3(Position.x, GetLocalAABB().Size.y / 2, Position.z));

	BeepSound.Load("rocketremote_beep.wav", false, false);
	FireSound.Load("rocketremote_fire.wav", false, false);

	Font.LoadTTFFont("arial.ttf", 12);
}

void CRocketTurretRemote::OnUpdate()
{
	//If laser is on
	if(Aiming && Owner)
	{
		//Raycast
		sym::CVector3 Start = Owner->GetPosition() + (Owner->GetAimVector() * (Owner->GetLocalAABB().Size.z));
		sym::CVector3 End = Owner->GetPosition() + (Owner->GetAimVector() * MaxFireDistance);
		sym::RaycastData Data = sym::Engine->Physics.Raycast(Start, End);

		if(Data.Geometry)
		{
			TargetPosition = Data.HitPosition;
			HasTarget = true;
		}
		else
		{
			TargetPosition = End;
			HasTarget = false;
		}
	}
}

void CRocketTurretRemote::OnRender()
{
	if(Aiming && Owner)
	{
		sym::CVector3 Start = Owner->GetPosition() + (Owner->GetAimVector() * (Owner->GetLocalAABB().Size.z));
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_LINES);
		glVertex3fv(&Start.x);
		glVertex3fv(&TargetPosition.x);
		glEnd();
	}

	//If the message timer is running and hasn't elapsed
	if(MessageTimer.IsRunning() && !MessageTimer.HasTriggered())
	{
		sym::glEnable2D();
		Font.RenderText(CurrentMessage, 20, 20);
		sym::glDisable2D();
	}
}

void CRocketTurretRemote::OnDestroy()
{
}

bool CRocketTurretRemote::Fire(int FireMode)
{
	if(Owner && FireMode == FIRE_PRIMARY)
	{
		//If not aiming
		if(!Aiming)
		{
			//Turn on laser/aiming
			Aiming = true;
		}
		else
		{
			//Find nearest turret
			CRocketTurret* Turret = FindNearestTurret();

			//If a turret was found in range
			if(Turret)
			{
				//Does the laser have a target
				if(HasTarget)
				{
					//Fire this turret
					Turret->Fire(TargetPosition);
					CurrentMessage = "Rocket on its way!";
					FireSound.Play();
				}
				else
				{
					//No target
					CurrentMessage = "No target";
					BeepSound.Play();
				}
			}
			else
			{
				//No turret withing range
				CurrentMessage = "No rocket launcher within wireless range";
				BeepSound.Play();
			}

			//Turn off laser/aiming
			Aiming = false;

			//Start message timer
			MessageTimer.Reset();
			MessageTimer.Start();
		}
	}

	return true;
}

CRocketTurret *CRocketTurretRemote::FindNearestTurret()
{
	//Get list of spawned objects
	std::vector<CSpawnable*> *Objects = Game->GetCurrentLevel()->GetSpawnedObjects();

	CRocketTurret *Turret = 0;
	float Distance = MaxWirelessRange;

	//Loop through spawned objects
	for(std::vector<CSpawnable*>::iterator i = Objects->begin(); i != Objects->end(); i++)
	{
		//If objects is a Rocket Turret
		CRocketTurret *CurrentTurret = dynamic_cast<CRocketTurret*>(*i);
		if(CurrentTurret)
		{
			//Get distance between player and turret
			float CurrentDistance = Game->GetCurrentLevel()->GetPlayer()->GetPosition().GetDistance(CurrentTurret->GetPosition());

			//If distance is closest
			if(CurrentDistance < Distance)
			{
				Turret = CurrentTurret;
				Distance = CurrentDistance;
			}
		}
	}

	//Return turret
	return Turret;
}

/////////////////////////////////////////////////////////////////////////////////////////
// ROCKET TURRET PROJECTILE
/////////////////////////////////////////////////////////////////////////////////////////
CRocketTurretProjectile::CRocketTurretProjectile()
{
	//Set defaults
	ModelFilename = "box10.sym.mesh";
	UpwardVelocity = 200.0f;
	DownwardVelocity = 5.0f;
	Acceleration = 0.5f;
	ExplosionForce = 1000.0f;
	ExplosionDiameter = 1000.0f;
	KillMe = false;
	Descending = false;
	PrevY = 0.0f;
};

void CRocketTurretProjectile::SetTarget(sym::CVector3 T)
{
	Target = T;
}

void CRocketTurretProjectile::OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation)
{
	//Load model
	Load("Projectile", ModelFilename, sym::PHYSICS_OBJ_BOX);

	//Set position and orientation
	SetPosition(Position);
	SetRotation(Rotation);

	//Create unique physics material
	CreateNewtonMaterial();

	//Disable collision with other projectiles
	//NewtonMaterialSetCollidable(sym::Engine->Physics.GetWorld(), GetNewtonMaterial(), GetNewtonMaterial(), 0);

	//Set collision callback
	SetMaterialCollisionCallback(NewtonMaterialGetDefaultGroupID(sym::Engine->Physics.GetWorld()));

	//Set velocity directly upward
	SetVelocity(sym::CVector3(0.0f, UpwardVelocity, 0.0f));

	ExplosionSound.Load("miniturret_explode.wav", false, false);

	NewtonBodySetUserData(GetNewtonBody(), this);
}

void CRocketTurretProjectile::OnUpdate()
{
	if(KillMe)
	{
		Game->GetCurrentLevel()->DestroySpawnedObject(this);
		return;
	}

	//Has the projectile started its descent
	if(!Descending && GetPosition().y < PrevY)
	{
		//LookAt(GetForward(), Target, 1000.0f * sym::Engine->GetDeltaTime());
		SetForceTorqueCallback();
		Descending = true;
	}
	else PrevY = GetPosition().y;

	if(Descending)
	{
		//Point towards target
		
		LookAt(GetForward(), Target, sym::Engine->GetDeltaTime() * 0.01f);
		DownwardVelocity += Acceleration;
	}
}

void CRocketTurretProjectile::OnRender()
{
}

void CRocketTurretProjectile::OnDestroy()
{
}

void CRocketTurretProjectile::ApplyForceAndTorque()
{
	Force = GetForward() * DownwardVelocity;
	NewtonBodyAddForce(GetNewtonBody(), &Force.x);
}

void CRocketTurretProjectile::MaterialCollisionCallback(CPhysicsBody *HitBody, sym::CVector3 HitPosition, sym::CVector3 Normal)
{
	//Spawn explosion
	CExplosionField *Explosion= Game->GetCurrentLevel()->Spawn<CExplosionField>(GetPosition(), sym::CQuaternion());
	Explosion->SetStrength(ExplosionForce);
	Explosion->SetDiameter(ExplosionDiameter);

	ExplosionSound.Play();

	//Destroy this projectile
	KillMe = true;
}