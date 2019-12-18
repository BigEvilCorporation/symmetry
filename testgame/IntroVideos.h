#pragma once

#include <vector>

#include "SYM_Engine.h"
#include "SYM_State.h"
#include "SYM_MPEG.h"

class CIntroVideoState : public sym::CState
{
	public:
		void Init();
		void Shutdown();
		void Pause();
		void Resume();
		void HandleInput();
		void Update();
		void Render();

	private:
		//Vector of MPEG videos
		std::vector<sym::CMpeg*> Videos;

		//Current video being played
		std::vector<sym::CMpeg*>::iterator CurrentVideo;
};