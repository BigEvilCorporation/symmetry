#include "Spawn.h"
#include "SYM_Core.h"

class CVideoBoard : public CSpawnable
{
	public:
		//Called when the object is spawned
		void OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation);

		//Called each frame
		void OnUpdate();

		//Called on 3D render
		void OnRender();

		//Called before the object is to be destroyed
		void OnDestroy();

	private:
		//Video
		sym::CMpeg Video;
		sym::CMaterial VideoMaterial;

		//Display
		sym::CMeshSubset *Display;
};