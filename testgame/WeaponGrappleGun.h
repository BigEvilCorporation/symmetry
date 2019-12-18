#pragma once

#include "SYM_Rope.h"

#include "WeaponBase.h"
#include "ProjectileBase.h"

class CGrappleProjectile;
class CGrappleObject;

class CGrappleGun : public CWeaponBase
{
	public:
		//Load model and set physics type
		void OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation);
		void OnUpdate();
		void OnRender();
		void OnDestroy();

		void SetMaxLength(float Length);

		bool Fire(int FireMode);

		void ProjectileHit(sym::CGeometry *Geom, sym::CVector3 Pos);

		void SetGrappleObjectList(std::vector<sym::CGeometry*> List);

	private:
		//Newton joint
		NewtonJoint *Joint;
		bool Grappling;
		float HangDistance, PullStrength;
		sym::CVector3 AttachPointObject;
		sym::CGeometry *AttachGeom;

		float PrevLinearDamp;
		sym::CVector3 PrevAngularDamp;

		//The rope
		sym::CRope *Rope;

		//Rope material
		sym::CMaterial *RopeMaterial;

		//Max length
		float MaxRopeLength;

		//Width
		float RopeWidth;

		//Has the projectile hit anything yet
		bool ProjectileHasHit;

		//Grapple object attached
		CGrappleObject *GrappledObject;

		//Current projectile
		CGrappleProjectile *Projectile;

		//List of grapple-able scene objects
		std::vector<sym::CGeometry*> SceneGrappleObjects;

		//Message font
		sym::CFont Font;

		//Message timer
		sym::CTimer MessageTimer;

		//Current message
		std::string CurrentMessage;

		sym::CVector3 CastStart, CastEnd;
};

class CGrappleObject
{
	public:
		CGrappleObject()
		{
			Grappled = false;
		}

		virtual ~CGrappleObject() {};

		bool IsGrappled() { return Grappled; }
		bool Grappled;
};

class CGrappleProjectile : public CProjectileBase
{
	public:
		//Load model and set physics type
		void OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation);

		void SetOwner(CGrappleGun *GrappleGun);

		//Update AI
		void OnUpdate();

		void ApplyForceAndTorque();

		//void ApplyForceAndTorque();
		void MaterialCollisionCallback(CPhysicsBody *HitBody);

	private:
		CGrappleGun *Owner;
		bool Moving;
};