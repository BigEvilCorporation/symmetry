#pragma once

#include "SYM_Core.h"
#include "Spawn.h"

class CProjectileSeek : public CSpawnable
{
	public:
		CProjectileSeek();

		//Load model and set physics type
		void OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation);

		//Update AI
		void OnUpdate();
		void OnRender();
		void OnDestroy();

		void ApplyForceAndTorque();
		void MaterialCollisionCallback(CPhysicsBody *HitBody, sym::CVector3 HitPosition, sym::CVector3 Normal);

		void SetSpeed(float S) { Speed = S; }

	protected:
		std::string ModelFilename;
		sym::CVector3 Target;
		float Speed;
		float SteerSpeed;
		float ExplosionForce;
		float ExplosionDiameter;
		bool KillMe;

		sym::CSound *ExplosionSound;
};