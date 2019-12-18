#include "WeaponGrappleGun.h"
#include "CharacterBase.h"
#include "Game.h"

void CGrappleGun::OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation)
{
	//Load mesh
	Load("GrappleGun", "grapplegun.sym.mesh", sym::NO_PHYSICS);

	//Load rope material
	RopeMaterial = sym::Engine->ResourceManager.Materials.Get("rope.sym.material");

	//Set default parameters
	CInventoryItemBase::SetName("GrappleGun");
	AmmoType = AMMO_TOGGLE;
	Rope = 0;
	MaxRopeLength = 200.0f;
	RopeWidth = 2.0f;
	RefireTimer.SetTime(10.0f);
	RefireTimer.Start();
	ProjectileHasHit = false;
	Projectile = 0;
	GrappledObject = 0;

	Joint = 0;
	Grappling = false;
	HangDistance = 50.0f;
	PullStrength = 200.0f;

	MessageTimer.SetTime(3000.0f);

	//Load message font
	Font.LoadTTFFont("arial.ttf", 12);
}

void CGrappleGun::OnUpdate()
{
	if(Grappling)
	{
		if(!Joint)
		{
			//Get distance from player to grapple point
			float Distance = Owner->GetPosition().GetDistance(AttachPointObject);

			//Pull owner towards target - the closer to the point, the greater the force
			Owner->AddForce((AttachPointObject - Owner->GetPosition()).GetNormalised() * PullStrength * Owner->Mass * (MaxRopeLength / Distance));

			//If target is within a certain range, create hanging joint
			if(Distance  < HangDistance)
			{
				//Kill player velocity
				Owner->SetVelocity(sym::CVector3(0.0f, 0.0f, 0.0f));

				//Re-apply control joint
				//Owner->ApplyControlJoint();
				Owner->ApplyInverseDynamics();

				/*

				//Create hanging joint
				sym::CVector3 Pivot = AttachPointObject;
				Joint = NewtonConstraintCreateBall(sym::Engine->Physics.GetWorld(), &Pivot.x, AttachGeom->GetNewtonBody(), Owner->GetNewtonBody());
				NewtonJointSetStiffness(Joint, 0.9f);

				//Store damping
				PrevLinearDamp = NewtonBodyGetLinearDamping(Owner->GetNewtonBody());
				NewtonBodyGetAngularDamping(Owner->GetNewtonBody(), &PrevAngularDamp.x);

				//Set linear and angular damping
				sym::CVector3 AngularDamp (0.1f, 0.1f, 0.1f);
				NewtonBodySetLinearDamping (Owner->GetNewtonBody(), 0.6f);
                NewtonBodySetAngularDamping (Owner->GetNewtonBody(), &AngularDamp.x);

				//Set cone restriction
                sym::CVector3 Pin(0.0f, 1.0f, 0.0f);
                NewtonBallSetConeLimits(Joint, &Pin.x, 45.0f  * sym::Maths::PI_DIV_180, 45.0f * sym::Maths::PI_DIV_180);
				*/

				Grappling = false;
			}
		}
	}
}

void CGrappleGun::OnDestroy()
{
}

void CGrappleGun::OnRender()
{
	if(Rope) Rope->Render();

	if(Grappling)
	{
		sym::CVector3 AttachPointPlayer = Owner->GetPosition() + (Owner->GetForward() * Owner->GetLocalAABB().Size.z);

		glBegin(GL_LINES);
		glVertex3f(AttachPointPlayer.x, AttachPointPlayer.y, AttachPointPlayer.z);
		glVertex3f(AttachPointObject.x, AttachPointObject.y, AttachPointObject.z);
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

void CGrappleGun::SetGrappleObjectList(std::vector<sym::CGeometry*> List)
{
	SceneGrappleObjects = List;
}

void CGrappleGun::SetMaxLength(float Length)
{
	if(Length > RopeWidth) MaxRopeLength = Length;
	else Game->Log.AddEntry("Error: Rope max length must be greater than its width");
}

void CGrappleGun::ProjectileHit(sym::CGeometry *Geom, sym::CVector3 Pos)
{
	if(!ProjectileHasHit)
	{
		sym::CVector3 Start = Owner->GetPosition() + (Owner->GetForward() * (Owner->GetLocalAABB().Size.z));

		//Create new rope
		Rope = new sym::CRope;
		Rope->Create(Start, Pos, RopeWidth);

		//Attach rope
		Rope->AttachStart(Owner, Owner->GetPosition());
		Rope->AttachEnd(Geom, Pos);

		//Apply rope material
		Rope->SetMaterial(RopeMaterial);

		//Disable collision with owner
		Rope->DisableCollision(Owner);

		ProjectileHasHit = true;
	}
}

/*
bool CGrappleGun::Fire(int FireMode)
{
	if(FireMode == FIRE_PRIMARY && Owner)
	{
		if(Projectile)
		{
			Game->GetCurrentLevel()->DestroySpawnedObject(Projectile);
			Projectile = 0;
		}

		//If rope exists, delete it
		if(Rope)
		{
			delete Rope;
			Rope = 0;
		}
		else
		{
			//Spawn projectile
			ProjectileHasHit = false;
			Projectile = Game->GetCurrentLevel()->Spawn<CGrappleProjectile>(Owner->GetPosition() + (Owner->GetForward() * (Owner->GetLocalAABB().Size.z)), GetRotation());
			Projectile->SetOwner(this);
		}
	}

	return true;
}
*/

bool CGrappleGun::Fire(int FireMode)
{
	if(FireMode == FIRE_PRIMARY && Owner)
	{
		//If already grappling
		if(Grappling)
		{
			if(Joint)
			{
				NewtonDestroyJoint(sym::Engine->Physics.GetWorld(), Joint);
				Joint = 0;

				//Re-apply control joint
				//Owner->ApplyControlJoint();
			}

			if(GrappledObject)
			{
				GrappledObject->Grappled = false;
				GrappledObject = 0;
			}

			Grappling = false;

			Owner->ApplyInverseDynamics();

			//Restore damping
			NewtonBodySetLinearDamping(Owner->GetNewtonBody(), PrevLinearDamp);
			NewtonBodySetAngularDamping(Owner->GetNewtonBody(), &PrevAngularDamp.x);
		}
		else
		{
			if(SceneGrappleObjects.size() > 0)
			{
				//Get player position
				sym::CVector3 PlayerPos = Owner->GetPosition();

				//Copy the grapple point list
				std::vector<sym::CGeometry*> GrappleList = SceneGrappleObjects;

				//Select first grapple point
				sym::CGeometry *ClosestGrappleGeom = GrappleList[0];

				//Loop through grapple points
				for(int i = 1; i < (int)GrappleList.size(); i++)
				{
					//If this grapple point is closer
					if(PlayerPos.GetDistance(GrappleList[i]->GetPosition()) < PlayerPos.GetDistance(ClosestGrappleGeom->GetPosition()))
					{
						//Set this as closest
						ClosestGrappleGeom = GrappleList[i];
					}
				}

				//Raycast from character front to grapple object
				sym::CVector3 Start = Owner->GetPosition() + (Owner->GetForward() * Owner->GetLocalAABB().Size.z);
				sym::CVector3 Direction = ClosestGrappleGeom->GetPosition() - Start;
				Direction.Normalise();
				sym::CVector3 End = Start + (Direction * MaxRopeLength);
				sym::RaycastData HitData = sym::Engine->Physics.Raycast(Start, End);

				CastStart = Start;
				CastEnd = End;

				//Check geom is visible (if hit geometry is still the closest grapple point), and distance is valid
				if((HitData.Geometry == ClosestGrappleGeom) && (HitData.Distance > 0.0f && HitData.Distance < 1.0f))
				{
					//Set attachment position
					AttachPointObject = HitData.HitPosition;
					
					//Set attachment geometry
					AttachGeom = HitData.Geometry;

					//Turn on grappling
					Grappling = true;

					//Kill control joint
					//Owner->ReleaseControlJoint();
					Owner->ReleaseInverseDynamics();
				}
				else
				{
					CurrentMessage = "No grapple point accessible";
					MessageTimer.Reset();
					MessageTimer.Start();
				}
			}
			else
			{
				CurrentMessage = "No grapple points in scene";
				MessageTimer.Reset();
				MessageTimer.Start();
			}
		}

		//Reset refire timer
		RefireTimer.Reset();
	}

	return true;
}

void CGrappleProjectile::OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation)
{
	//Load model
	Load("Projectile", ModelFilename, sym::PHYSICS_OBJ_BOX);

	//Set position and orientation
	SetPosition(Position);
	SetRotation(Rotation);

	//Create unique physics material
	CreateNewtonMaterial();

	//Set collision callback
	SetMaterialCollisionCallback(NewtonMaterialGetDefaultGroupID(sym::Engine->Physics.GetWorld()));

	SetForceTorqueCallback();

	NewtonBodySetUserData(GetNewtonBody(), this);

	Moving = true;
	Speed = 1000.0f;
}

void CGrappleProjectile::MaterialCollisionCallback(CPhysicsBody *HitBody)
{
	//Notify owner
	Owner->ProjectileHit((sym::CGeometry*)HitBody, GetPosition());

	//Stop moving
	Moving = false;
}

void CGrappleProjectile::ApplyForceAndTorque()
{
	if(Moving)
	{
		Force = GetForward() * Speed;
		NewtonBodyAddForce(GetNewtonBody(), &Force.x);
	}
}

void CGrappleProjectile::SetOwner(CGrappleGun *GrappleGun)
{
	Owner = GrappleGun;
}

void CGrappleProjectile::OnUpdate()
{
	if(!Moving)
	{
		DestroyNewtonBody();
		PhysicsType = sym::NO_PHYSICS;
	}
	else
	{
		Yaw(0.01f * sym::Engine->GetDeltaTime());
	}
}