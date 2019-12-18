#pragma once

#include "SYM_Core.h"
#include "Game.h"
#include "WeaponBase.h"
#include "ProjectileBase.h"

class CRocketTurretProjectile : public CProjectileBase
{
	public:
		CRocketTurretProjectile();

		//Load model and set physics type
		void OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation);

		//Update AI
		void OnUpdate();
		void OnRender();
		void OnDestroy();

		void SetTarget(sym::CVector3 T);

		void ApplyForceAndTorque();
		void MaterialCollisionCallback(CPhysicsBody *HitBody, sym::CVector3 HitPosition, sym::CVector3 Normal);

	protected:
		std::string ModelFilename;
		float ExplosionForce;
		float ExplosionDiameter;
		bool KillMe;

		float PrevY;
		float UpwardVelocity;
		float DownwardVelocity;
		float Acceleration;
		bool Descending;
		sym::CVector3 Target;

		sym::CSound ExplosionSound;
};

class CRocketTurret : public CWeaponBase
{
	public:
		//Load model and set physics type
		void OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation);

		void OnUpdate();
		void OnRender();
		void OnDestroy();

		void Fire(sym::CVector3 TargetPos);

	private:

		//Projectile spawn point point
		sym::CVector3 ProjectileSpawnPoint;
		sym::CQuaternion ProjectileDirection;

		//Sounds
		sym::CSound FireSound;
};

class CRocketTurretRemote : public CWeaponBase
{
	public:
		//Load model and set physics type
		void OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation);

		void OnUpdate();
		void OnRender();
		void OnDestroy();

		bool Fire(int FireMode);

	private:
		//Searches through spawn list and finds nearest rocket turret
		CRocketTurret *FindNearestTurret();

		//Sounds
		sym::CSound HumSound;
		sym::CSound BeepSound;
		sym::CSound FireSound;

		//Max laser distance
		float MaxFireDistance;

		//Max wireless range
		float MaxWirelessRange;

		//Is aiming laser on
		bool Aiming;

		//Is the laser touching any geometry
		bool HasTarget;

		//Last raycasted position (potential fire target)
		sym::CVector3 TargetPosition;

		//Message font
		sym::CFont Font;

		//Message timer
		sym::CTimer MessageTimer;

		//Current message
		std::string CurrentMessage;
};