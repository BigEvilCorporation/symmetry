#include "WeaponTurretLauncher.h"
#include "Game.h"

void CTurretLauncher::OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation)
{
	//Set defaults
	MovementSpeed = 0.01f;
	FanSpeed = 0.4f;
	FanDeceleration = 0.00005f;
	MaxFireDistance = 1000.0f;
	RefireTimer.SetTime(5000.0f);
	RefireTimer.SetLooping(true);

	//Load base model with fixed level physics
	if(!Load("Weapon", "turretlauncher_main.sym.mesh", sym::LEVEL_COLLISION))
	{
		Game->Log.AddEntry("Error: Could not load turret mesh");
	}

	//Position on floor (TODO: Find floor with raycast)
	SetPosition(sym::CVector3(Position.x, GetLocalAABB().Size.y / 2, Position.z));

	//Get head subset
	if(!SetHeadSubset("head"))
	{
		Game->Log.AddEntry("Error: Could not set head subset for turret");
	}

	//Load power supply mesh
	if(!PowerSupply.Load("TurretPSU", "turretlauncher_powersupply.sym.mesh", sym::PHYSICS_OBJ_CUSTOM))
	{
		Game->Log.AddEntry("Error: Could not load power supply mesh for turret launcher");
	}

	//Set PSU position
	PowerSupply.SetPosition(sym::CVector3(Position.x - 100.0f, PowerSupply.GetLocalAABB().Size.y / 2, Position.z));

	//Set mass
	PowerSupply.SetMass(5.0f);

	//Get fan subset
	Fan = PowerSupply.Mesh->GetSubsetByName("fan");
	if(!Fan)
	{
		Game->Log.AddEntry("Error: Could not get power supply fan subset for turret launcher");
	}

	sym::CVector3 Start = GetPosition() - sym::CVector3(GetLocalAABB().Size.x / 2, 10.0f, 0.0f);
	sym::CVector3 End = PowerSupply.GetPosition() + sym::CVector3(PowerSupply.GetLocalAABB().Size.x / 2, -5.0f, 0.0f);

	//Create cable
	Cable.Create(Start, End, 3.0f);

	//Attach cable
	Cable.AttachStart(this, Start);
	Cable.AttachEnd(&PowerSupply, End);

	//Get middle joint
	MiddleJoint = (int)Cable.GetNumLinks() / 2;

	//Set pivot point
	PivotPoint = sym::CVector3(-6.0f, 24.0f, -19.0f);

	//Set initial target (current player)
	SetTarget(Game->GetCurrentLevel()->GetPlayer());

	//Turret is enabled
	Enabled = true;

	FireSound.Load("turretlauncher_fire.wav", false, false);
	LoopSound.Load("turretlauncher_loop.wav", false, true);
	PowerDownSound.Load("turretlauncher_powerdown.wav", false, false);
	SparkSound.Load("turretlauncher_spark.wav", false, false);
	
	LoopSound.Play();
	LoopSound.SetPosition(PowerSupply.GetPosition());

	//Start refire timer
	RefireTimer.Start();

	Font.LoadTTFFont("arial.ttf", 12);
	FirePoint.Create("FirePoint", sym::SYM_BOX, sym::NO_PHYSICS, sym::CVector3(4.0f, 4.0f, 4.0f));
}

void CTurretLauncher::OnRender()
{
	//Render PSU
	PowerSupply.Render();

	//Render cable
	Cable.Render();

	if(Game->GetDebugMode())
	{
		if(CurrentTarget && Head)
		{
			sym::CVector3 Start = GetPosition();

			//Create a lookat object, look at the player, then get the Yaw from the head
			sym::CPlaceable End;
			End.SetPosition(GetPosition());
			End.LookAt(sym::CVector3(0.0f, 0.0f, 1.0f), CurrentTarget->GetPosition(), 1.0f);
			End.SetRotation(sym::CQuaternion(End.GetRotation().W, End.GetRotation().X, Head->GetRotation().Y, Head->GetRotation().Z));
			End.Move(-MaxFireDistance);

			sym::CVector3 EndV = End.GetPosition();

			glColor3f(1.0f, 0.0f, 0.0f);
			glBegin(GL_LINES);
			glVertex3fv(&Start.x);
			glVertex3fv(&EndV.x);
			glEnd();
		}

		FirePoint.Render();

		if(CanSeeTarget())
		{
			sym::glEnable2D();
				Font.RenderText("Turret Launcher can see you", 20, 40);
			sym::glDisable2D();
		}
	}
}

void CTurretLauncher::OnUpdate()
{
	//If turret launcher is enabled
	if(Enabled)
	{
		//If head subset has been set
		if(Head)
		{
			//If there is a target
			if(CurrentTarget)
			{
				sym::CPlaceable RotObject;
				RotObject.SetPosition(GetPosition() + PivotPoint);
				RotObject.SetRotation(Head->GetRotation());
				RotObject.LookAt(sym::CVector3(0.0f, 0.0f, 1.0f), CurrentTarget->GetPosition(), 1.0f);//MovementSpeed * sym::Engine->GetDeltaTime());

				sym::CQuaternion Quat = RotObject.GetRotation();
				Quat.X = 0.0f;
				Quat.Z = 0.0f;
				Head->SetRotation(Quat);

				//If refire timer has triggered, and target is in view
				if(RefireTimer.HasTriggered() && CanSeeTarget())
				{
					//Get distance to target
					float Distance = GetPosition().GetDistance(CurrentTarget->GetPosition());

					//End of turret barrel
					sym::CVector3 BarrelEnd = GetPosition() + PivotPoint + (Head->GetForward().GetNormalised() * -50.0f);

					//Spawn a miniturret
					CMiniTurret *Turret = Game->GetCurrentLevel()->Spawn<CMiniTurret>(BarrelEnd, Head->GetRotation());

					//Set velocity based on target distance
					Turret->SetVelocity(Head->GetForward() * (-Distance * 0.4f));

					FireSound.Play();
					FireSound.SetPosition(GetPosition() + PivotPoint);
				}

			}
		}
	
		//If too much force is applied to middle joint
		if(Cable.GetJointForce(MiddleJoint) > 800.0f)
		{
			//Snap cable
			Cable.BreakJoint(MiddleJoint);

			//Disable turret
			Enabled = false;

			//Stop looping sound
			LoopSound.Stop();

			//Play power down sound
			PowerDownSound.Play();
			SparkSound.Play();
			PowerDownSound.SetPosition(PowerSupply.GetPosition());
			SparkSound.SetPosition(PowerSupply.GetPosition());
		}
	}
	else
	{
		//Decelerate fan
		FanSpeed -= FanDeceleration * sym::Engine->GetDeltaTime();

		//Stop fan
		if(FanSpeed < 0.0f) FanSpeed = 0.0f;
	}

	//Rotate fan
	Fan->Yaw(FanSpeed * sym::Engine->GetDeltaTime());

	FirePoint.SetPosition(GetPosition() + PivotPoint + (Head->GetForward().GetNormalised() * -50.0f));
}

void CTurretLauncher::OnDestroy()
{
}

bool CTurretLauncher::SetHeadSubset(std::string SubsetName)
{
	//Check a mesh has been loaded
	if(Mesh)
	{
		//Get subset
		sym::CMeshSubset *Subset = Mesh->GetSubsetByName(SubsetName);

		//Check subset exists
		if(Subset)
		{
			//Set subset
			Head = Subset;
			return true;
		}
		else
		{
			//Subset does not exist
			Game->Log.AddEntry("Error: Subset " + SubsetName = " does not exist on turret " + CPlaceable::Name);
			return false;
		}
	}
	else
	{
		//Mesh does not exist
		Game->Log.AddEntry("Error: SetHeadSubset called before mesh has been loaded on turret " + CPlaceable::Name);
		return false;
	}
}

void CTurretLauncher::SetTarget(sym::CPlaceable *Target)
{
	CurrentTarget = Target;
}

void CTurretLauncher::SetMovementSpeed(float Speed)
{
	MovementSpeed = Speed;
}

bool CTurretLauncher::CanSeeTarget()
{
	if(CurrentTarget && Head)
	{
		sym::CVector3 Start = GetPosition();

		//Create a lookat object, look at the player, then get the Yaw from the head
		sym::CPlaceable End;
		End.SetPosition(GetPosition());
		End.LookAt(sym::CVector3(0.0f, 0.0f, 1.0f), CurrentTarget->GetPosition(), 1.0f);
		End.SetRotation(sym::CQuaternion(End.GetRotation().W, End.GetRotation().X, Head->GetRotation().Y, Head->GetRotation().Z));
		End.Move(-MaxFireDistance);

		//Raycast
		sym::RaycastData Data = sym::Engine->Physics.Raycast(Start, End.GetPosition());

		//If target == raycast hit geometry, player is in view
		if(Data.Geometry == CurrentTarget) return true;
	}

	return false;
}

