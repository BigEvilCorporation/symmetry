#include "SYM_Core.h"

#include "Level.h"

int LuaLoadPlayground(lua_State *LuaState);

class CPlaygroundState : public CLevelState
{
	public:
		CPlaygroundState();

		void Init();
		void Shutdown();
		void Pause();
		void Resume();
		void HandleInput();
		void Update();

		void OnRender();
		void OnRenderOverlay();

	private:

		sym::CSprite Crosshair;

		sym::RaycastData AimRaycastData;

		sym::CModel Test;
		sym::CSkeletalAnimation TestAnim;
};
