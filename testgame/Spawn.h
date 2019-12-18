#pragma once

#include "SYM_Core.h"

class CSpawnable : public sym::CModel
{
	public:
		//Called when the object is spawned
		virtual void OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation) { }

		//Called each frame
		virtual void OnUpdate() { }

		//Called on 3D render
		virtual void OnRender() { }

		//Called before the object is to be destroyed
		virtual void OnDestroy() { }

	protected:

};