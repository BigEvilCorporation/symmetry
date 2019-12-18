#pragma once

#include "WeaponBase.h"
#include "ProjectileBase.h"
#include "WeaponGrappleGun.h"

class CMiniTurret : public CWeaponBase, CGrappleObject
{
	public:
		CMiniTurret();
		~CMiniTurret();

		//Load model and set physics type
		void OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation);

		//Update AI
		void OnUpdate();
		void OnRender();
		void OnDestroy();

		//Set mesh subsets
		bool SetHeadSubset(std::string SubsetName);
		bool SetBarrelSubset(std::string SubsetName);

		//Set target
		void SetTarget(sym::CGeometry *Target);

		//Set target list
		void SetTargets(std::vector<sym::CGeometry*> Targets);

		//Set pivot limits
		void SetBarrelPitchLimit(float Limit);

		//Set movement speed
		void SetMovementSpeed(float Speed);

		//Set shooting range
		void SetMaxFireDistance(float Distance);

		bool Fire(int FireMode);

		//Test if target is in view of the barrel
		bool CanSeeTarget();

	private:
		//Head and barrel mesh subsets
		sym::CMeshSubset *Head, *Barrel;

		//Current target
		sym::CGeometry *CurrentTarget;

		//Vector of targets
		std::vector<sym::CGeometry*> TargetList;

		//Pivot point
		sym::CVector3 HeadPivotPoint, BarrelPivotPoint;

		//Limits
		float BarrelPitchLimit;

		//Shoot range
		float MaxFireDistance;

		//Movement speed
		float MovementSpeed;

		//Fireing speed
		float Speed;

		//Hit point
		sym::CVector3 HitPoint;

		//Self-detonation timer
		sym::CTimer SuicideTimer;

		sym::CFont Font;

		void MaterialCollisionCallback(CPhysicsBody *HitBody, sym::CVector3 HitPosition, sym::CVector3 Normal);

		//Attach to object
		void Attach(sym::CGeometry *Geom, sym::CVector3 Position, sym::CVector3 Normal);

		bool Attached;

		//Newton joint for attaching
		NewtonJoint *Joint;

		//Sounds
		sym::CSound *AttachSound, *FireSound, *ExplodeSound;
};