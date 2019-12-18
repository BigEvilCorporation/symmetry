#ifndef __SYM_INPUT
#define __SYM_INPUT

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include <SDL/SDL.h>

namespace sym
{
	class CInput
	{
		public:
			//Initialise input subsystem
			bool Init();
			void Shutdown();

			//Update keyboard/mouse/input input, as well as system messages. Returns false if window receives QUIT message
			bool UpdateInput();

			//Reset all input states
			void Reset();

			//Get mouse coords, relative
			int GetMouseRelativeX();
			int GetMouseRelativeY();

			//Get mouse coords, absolute
			int GetMouseAbsoluteX();
			int GetMouseAbsoluteY();

			//Mouse resolution
			void SetMouseResolution(float Res);
			float GetMouseResolution();

			//Joystick sensitivity
			void SetJoystickSensitivity(float Sens);
			float GetJoystickSensitivity();

			//Warp pointer
			void WarpMouse(float x, float y);

			//Restrict mouse from leaving window
			void StealMouse();
			void ReleaseMouse();

			//Check if key is down
			bool IsKeyDown(int Key);

			//Check if key was pressed since last SDL_KEYUP
			bool IsKeyPressed(int Key);

			//Check for mouse buttons
			bool IsLMBDown();
			bool IsMMBDown();
			bool IsRMBDown();

			//Joystick functions
			bool JoystickAvailable();
			int GetNumJoystickAxis();
			int GetNumJoystickHats();
			int GetNumJoystickButtons();
			int GetJoystickAxis(int AxisNum);
			int GetJoystickHat(int HatNum);
			int GetJoystickButton(int ButtonNum);

		private:
			//SDL input event
			SDL_Event Event;

			//Joystick interface
			SDL_Joystick *Joystick;

			//Mouse button state
			Uint8 MouseState;

			//Mouse coords
			int MouseAbsoluteX;
			int MouseAbsoluteY;
			int MouseRelativeX;
			int MouseRelativeY;

			//Mouse resolution
			float MouseResolution;

			//Joystick sensitivity
			float JoystickSensitivity;

			//Array of keys being held down
			bool KeysDown[323];

			//Array of keys that have been pressed
			bool KeysPressed[323];
	};
} //Namespace

#endif
