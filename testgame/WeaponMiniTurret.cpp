#include "WeaponMiniTurret.h"
#include "Game.h"

CMiniTurret::CMiniTurret()
{
	Head = Barrel = 0;
	Joint = 0;
	Attached = false;
	MovementSpeed = 0.001f;
	Speed = 100.0f;
	MaxFireDistance = 200.0f;
	RefireTimer.SetTime(200.0f);
	RefireTimer.SetLooping(true);
	SuicideTimer.SetTime(1000.0f * 20.0f);

	CurrentTarget = 0;
}

CMiniTurret::~CMiniTurret()
{

}

void CMiniTurret::OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation)
{
	//Load model with box physics
	if(!Load("MiniTurret", "miniturret.sym.mesh", sym::PHYSICS_OBJ_BOX))
	{
		Game->Log.AddEntry("Error: Could not load miniturret mesh");
	}

	//Create unique mesh
	sym::CMesh *UniqueMesh = new sym::CMesh;
	UniqueMesh->Subsets = Mesh->Subsets;
	Mesh = UniqueMesh;

	//Set head subset
	if(!SetHeadSubset("head"))
	{
		Game->Log.AddEntry("Error: Could not set head subset for miniturret");
	}

	//Set barrel subset
	if(!SetBarrelSubset("barrel"))
	{
		Game->Log.AddEntry("Error: Could not set barrel subset for miniturret");
	}

	//Set target (default is current player)
	//SetTarget(Game->GetCurrentLevel()->GetPlayer());

	//Set pivot points
	HeadPivotPoint = sym::CVector3(0.0f, 0.0f, 2.5f);
	BarrelPivotPoint = sym::CVector3(0.0f, 0.0f, 2.5f);

	//Set position and rotation
	SetPosition(Position + sym::CVector3(0.0f, GetLocalAABB().Size.y / 2, 0.0f));
	SetRotation(Rotation);
	Pitch(90.0f);

	//Create unique physics material
	CreateNewtonMaterial();

	//Set collision callback
	SetMaterialCollisionCallback(NewtonMaterialGetDefaultGroupID(sym::Engine->Physics.GetWorld()));

	SetVelocity(GetUp() * -Speed);

	NewtonBodySetUserData(GetNewtonBody(), this);

	SetMass(0.5f);

	//Load sounds
	//AttachSound = sym::Engine->ResourceManager.Audio.Get("miniturret_arm.wav");
	//FireSound = sym::Engine->ResourceManager.Audio.Get("miniturret_fire.wav");
	ExplodeSound = sym::Engine->ResourceManager.Audio.Get("sound_wpn_rocket_det01.wav");
}

void CMiniTurret::OnUpdate()
{
	//If Suicide timer has triggered, destroy
	if(SuicideTimer.HasTriggered())
	{
		ExplodeSound->Play();
		ExplodeSound->SetPosition(GetPosition());
		Game->GetCurrentLevel()->DestroySpawnedObject(this);
		return;
	}

	//If no current target
	if(!CurrentTarget)
	{
		float Distance = MaxFireDistance;

		//Find closest target
		for(int i = 0; i < (int)TargetList.size(); i++)
		{
			if(TargetList[i]->GetPosition().GetDistance(GetPosition()) < Distance) CurrentTarget = TargetList[i];
		}
	}

	//If subsets have been set
	if(Head && Barrel && CurrentTarget)
	{
		//Point head and barrel at target
		sym::CPlaceable RotObject;
		RotObject.SetPosition(GetPosition() + HeadPivotPoint);
		RotObject.SetRotation(Barrel->GetRotation());
		RotObject.LookAt(sym::CVector3(0.0f, 0.0f, 1.0f), CurrentTarget->GetPosition(), MovementSpeed * sym::Engine->GetDeltaTime());

		sym::CQuaternion Quat = RotObject.GetRotation();

		Quat.Z = 0.0f;
		Barrel->SetRotation(Quat);

		Quat.X = 0.0f;
		Head->SetRotation(Quat);

		//Head->SetRotation(sym::CQuaternion(0.0f, RotObject.GetQuaternion().ToEuler().y, 0.0f));
		//Barrel->SetRotation(sym::CVector3(RotObject.GetQuaternion().ToEuler().x, RotObject.GetQuaternion().ToEuler().y, 0.0f));

		//If refire timer has triggered and target is in view, fire
		if(RefireTimer.HasTriggered() && CanSeeTarget()) Fire(FIRE_PRIMARY);
	}

	if(Joint)
	{
		sym::CVector3 Force;
		NewtonBallGetJointForce(Joint, &Force.x);
		if(Force.GetLength() > 5000.0f)
		{
			NewtonDestroyJoint(sym::Engine->Physics.GetWorld(), Joint);
			Joint = 0;
			Attached = false;
		}
	}
}

bool CMiniTurret::Fire(int FireMode)
{
	//'Flick' barrel up to simulate recoil
	//Barrel->RotateAxis(10.0f, Barrel->GetRight());

	//FireSound->Play();
	//FireSound->SetPosition(GetPosition());

	//Deal damage
	//CurrentTarget->Impulse(Barrel->GetForward() * -20.0f, HitPoint);
	
	return true;
}

bool CMiniTurret::CanSeeTarget()
{
	//Raycast from barrel
	sym::CVector3 Start = GetPosition();
	sym::CVector3 End = GetPosition() - (Barrel->GetForward() * MaxFireDistance);
	sym::RaycastData RayData = sym::Engine->Physics.Raycast(Start, End);

	//Test if the hit geometry is the target
	if(RayData.Geometry == CurrentTarget)
	{
		HitPoint = RayData.HitPosition;
		return true;
	}

	return false;
}

void CMiniTurret::OnRender()
{
	if(Head && Barrel && CurrentTarget)
	{
		sym::CVector3 Start = GetPosition();
		sym::CVector3 End = GetPosition() - (Barrel->GetForward() * MaxFireDistance);
		sym::RaycastData Data = sym::Engine->Physics.Raycast(Start, End);

		if(Data.Geometry) End = Data.HitPosition;

		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_LINES);
		glVertex3fv(&Start.x);
		glVertex3fv(&End.x);
		glEnd();
	}
}

void CMiniTurret::OnDestroy()
{
	//Explode
}

bool CMiniTurret::SetHeadSubset(std::string SubsetName)
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
			Game->Log.AddEntry("Error: Subset " + SubsetName = " does not exist on miniturret " + CPlaceable::Name);
			return false;
		}
	}
	else
	{
		//Mesh does not exist
		Game->Log.AddEntry("Error: SetHeadSubset called before mesh has been loaded on miniturret " + CPlaceable::Name);
		return false;
	}
}

bool CMiniTurret::SetBarrelSubset(std::string SubsetName)
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
			Barrel = Subset;
			return true;
		}
		else
		{
			//Subset does not exist
			Game->Log.AddEntry("Error: Subset " + SubsetName = " does not exist on miniturret " + CPlaceable::Name);
			return false;
		}
	}
	else
	{
		//Mesh does not exist
		Game->Log.AddEntry("Error: SetBarrelSubset called before mesh has been loaded on miniturret " + CPlaceable::Name);
		return false;
	}
}

void CMiniTurret::SetTarget(sym::CGeometry *Target)
{
	CurrentTarget = Target;
}

void CMiniTurret::SetTargets(std::vector<sym::CGeometry*> Targets)
{
	TargetList = Targets;
}

void CMiniTurret::SetBarrelPitchLimit(float Limit)
{
	BarrelPitchLimit = Limit;
}

void CMiniTurret::SetMovementSpeed(float Speed)
{
	MovementSpeed = Speed;
}

void CMiniTurret::SetMaxFireDistance(float Distance)
{
	MaxFireDistance = Distance;
}

void CMiniTurret::MaterialCollisionCallback(CPhysicsBody *HitBody, sym::CVector3 HitPosition, sym::CVector3 Normal)
{
	if(!IsGrappled())
	{
		//Attach
		sym::CGeometry *Geom = (sym::CGeometry*) HitBody;
		if(!Attached) Attach(Geom, HitPosition, Normal);
	}
}

void CMiniTurret::Attach(sym::CGeometry *Geom, sym::CVector3 Position, sym::CVector3 Normal)
{
	if(Geom)
	{
		LookAt(sym::CVector3(0.0f, 1.0f, 0.0f), GetPosition() + Normal, 1.0f);

		NewtonBody *Body0 = Geom->GetNewtonBody();
        NewtonBody *Body1 = GetNewtonBody();

		sym::CVector3 Pin = GetRight();
		Joint = NewtonConstraintCreateBall(sym::Engine->Physics.GetWorld(), &Position.x, Body1, Body0);
		NewtonBallSetConeLimits(Joint, &Pin.x, 1  * sym::Maths::PI_DIV_180, 1 * sym::Maths::PI_DIV_180);

		//Start timers
		RefireTimer.Start();
		SuicideTimer.Start();
		Attached = true;

		//AttachSound->Play();
		//AttachSound->SetPosition(GetPosition());
	}
}