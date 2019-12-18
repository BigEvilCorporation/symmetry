#pragma once

#include "SYM_Core.h"
#include "Spawn.h"

class CPickupBase : public CSpawnable
{
	public:
		CPickupBase();
		~CPickupBase();

		//Load model and set physics type
		void OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation);

		void OnUpdate();

		void OnDestroy();

	protected:
		//Trigger height and radius
		float TriggerHeight;
		float TriggerRadius;

		//Rotate speed
		float RotateSpeed;

		//Trigger callback
		static int TriggerCallback(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1, int threadIndex);
};