#pragma once

#include "Spawn.h"
#include "WeaponBase.h"
#include "WeaponMiniTurret.h"

class CTurretLauncher : public CWeaponBase
{
	public:
		//Load model and set physics type
		void OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation);

		//Update AI
		void OnUpdate();
		void OnRender();
		void OnDestroy();

		//Set head mesh subset
		bool SetHeadSubset(std::string SubsetName);

		//Set target
		void SetTarget(sym::CPlaceable *Target);

		//Set movement speed
		void SetMovementSpeed(float Speed);

	private:
		//Head subset
		sym::CMeshSubset *Head;

		//Power supply mesh
		sym::CModel PowerSupply;

		//Fan subset
		sym::CMeshSubset *Fan;

		//Power cable
		sym::CRope Cable;

		//Pivot point
		sym::CVector3 PivotPoint;

		//Current target
		sym::CPlaceable *CurrentTarget;

		//Limits
		float VertLimit, HorizLimit;

		//Shoot range
		float MaxFireDistance;

		//Movement speed
		float MovementSpeed;

		//Fan speed and deceleration
		float FanSpeed;
		float FanDeceleration;

		//Index of middle cable joint
		int MiddleJoint;

		//Is the turrent launcher enabled
		bool Enabled;

		//Check if the turret can see the target
		bool CanSeeTarget();

		sym::CSound FireSound, LoopSound, PowerDownSound, SparkSound;

		sym::CBrush FirePoint;

		sym::CFont Font;
};