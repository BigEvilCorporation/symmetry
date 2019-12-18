#pragma once

#include "SYM_Core.h"

#include "IntroVideos.h"

//Videos play only once
static bool FirstRun = true;

class CMainMenuState : public sym::CState
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
		//Intro videos state
		CIntroVideoState VideoState;

		//Menu font
		sym::CFont Font;

		//Title music
		sym::CSound TitleTrack;

		//Background video
		sym::CMpeg Video;
		sym::CSprite VideoCanvas;

		//Game logo
		sym::CSprite Logo;

		//Button sprites
		sym::CSprite ButtonStart;
		sym::CSprite ButtonLoad;
		sym::CSprite ButtonSettings;
		sym::CSprite ButtonCredits;
		sym::CSprite ButtonQuit;

		//Button colours
		sym::CColour White;
		sym::CColour Tint;

		//Mouse cursor
		sym::CSprite MouseCursor;

		//Mouse cursor rotation
		float MouseRotation;
		float MouseRotateSpeed;

		//Mouse coords
		int MouseX, MouseY;

		//Check for button presses
		void CheckButtons();
};