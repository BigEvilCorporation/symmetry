#pragma once

#include "SYM_Core.h"

class CLevelSelectState : public sym::CState
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

		//Menu font
		sym::CFont Font;

		//Button sprites
		sym::CSprite ButtonLvl1, ButtonLvl2, ButtonLvl3, ButtonLvl4, ButtonCancel;

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