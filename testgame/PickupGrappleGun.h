#pragma once

#include "PickupBase.h"

class CPickupGrappleGun : public CPickupBase
{
	public:
		//Load model and set physics type
		void OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation);

	private:
		//Trigger callback
		void MaterialAABBOverlapCallback(CPhysicsBody *HitBody);
};