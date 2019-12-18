#include "Spawn.h"

class CExplosionField : public CSpawnable
{
	public:
		CExplosionField();
		void OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation);
		void OnUpdate();
		void OnRender();

		void SetDiameter(float Diameter);
		void SetStrength(float Strength);

		void MaterialCollisionCallback(CPhysicsBody *HitBody);

	protected:
		float ExplodeStrength;
		float ExplodeDiameter;
};