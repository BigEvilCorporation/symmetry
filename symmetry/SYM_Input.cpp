#include "SYM_Input.h"
#include "SYM_Engine.h"

namespace sym
{
	bool CInput::Init()
	{
		MouseAbsoluteX = 0;
		MouseAbsoluteY = 0;
		MouseRelativeX = 0;
		MouseRelativeY = 0;

		MouseResolution = 0.1f;
		JoystickSensitivity = 0.0001f;

		for (int i = 0; i < 323; i++)
		{
			KeysDown[i] = false;
			KeysPressed[i] = false;
		}

		//Set to unicode input (for capital letter support)
		SDL_EnableUNICODE(1);

		//If joysticks available
		if(SDL_NumJoysticks() > 0) Joystick = SDL_JoystickOpen(0);
		else Joystick = 0;

		return true;
	}

	void CInput::StealMouse()
	{
		//Confine mouse to window
		SDL_WM_GrabInput(SDL_GRAB_ON);
	}

	void CInput::ReleaseMouse()
	{
		//Let mouse out of window
		SDL_WM_GrabInput(SDL_GRAB_OFF);
	}

	void CInput::Shutdown()
	{
		if(SDL_JoystickOpened(0) && Joystick) SDL_JoystickClose(Joystick);
	}

	void CInput::Reset()
	{
		//Reset all keys
		for (int i = 0; i < 323; i++)
		{
			KeysDown[i] = false;
			KeysPressed[i] = false;
		}

		//Reset mouse button state
		MouseState = 0;

		//Warp cursor
		WarpMouse(0.0f, 0.0f);

		//Reset mouse movement
		MouseAbsoluteX = MouseAbsoluteY = MouseRelativeX = MouseRelativeY = 0;
	}

	bool CInput::JoystickAvailable()
	{
		if(SDL_JoystickOpened(0) && Joystick) return true;
		else return false;
	}

	int CInput::GetNumJoystickAxis()
	{
		if(Joystick) return SDL_JoystickNumAxes(Joystick);
		else return 0;
	}

	int CInput::GetNumJoystickHats()
	{
		if(Joystick) return SDL_JoystickNumHats(Joystick);
		else return 0;
	}

	int CInput::GetNumJoystickButtons()
	{
		if(Joystick) return SDL_JoystickNumButtons(Joystick);
		else return 0;
	}

	int CInput::GetJoystickAxis(int AxisNum)
	{
		if(Joystick) return SDL_JoystickGetAxis(Joystick, AxisNum);
		else return 0;
	}

	int CInput::GetJoystickHat(int HatNum)
	{
		if(Joystick) return SDL_JoystickGetHat(Joystick, HatNum);
		else return 0;
	}

	int CInput::GetJoystickButton(int ButtonNum)
	{
		if(Joystick) return SDL_JoystickGetButton(Joystick, ButtonNum);
		else return 0;
	}

	bool CInput::UpdateInput()
	{
		//Update joystick
		SDL_JoystickUpdate();

		//Zero relative mouse
		MouseRelativeX = 0;
		MouseRelativeY = 0;

		//Poll for SDL events, loop until buffer is empty
		while(SDL_PollEvent(&Event))
		{
			switch(Event.type)
			{
				case SDL_QUIT:
				{
					return false;
				}

				case SDL_KEYDOWN:
				{
					//If console is open
					if(Engine->Console.IsOpen)
					{
						//If key is '`' or '~' or ESC, close console, else pass key presses to the console's key buffer
						if(Event.key.keysym.sym == SDLK_ESCAPE || Event.key.keysym.sym == '`' || Event.key.keysym.sym == '~') Engine->Console.Close();
						else Engine->Console.KeyboardInput(Event.key.keysym.sym, Event.key.keysym.unicode);
					}
					else
					{
						//Update key array
						KeysDown[Event.key.keysym.sym] = true;
						KeysPressed[Event.key.keysym.sym] = true;
					}

					break;
				}
				
				case SDL_KEYUP:
				{
					//If console is not open
					if(!Engine->Console.IsOpen)
					{
						//Update key array
						KeysDown[Event.key.keysym.sym] = false;
						KeysPressed[Event.key.keysym.sym] = false;
					}

					break;
				}
				
				case SDL_MOUSEMOTION:
				{
					//Save mouse state
					MouseState = Event.motion.state;

					//Set relative coords
					MouseRelativeX = Event.motion.xrel;
					MouseRelativeY = Event.motion.yrel;

					//Set absolute coords
					MouseAbsoluteX = Event.motion.x;
					MouseAbsoluteY = Event.motion.y;

					break;
				}
			}
		}

		return true;
	}

	int CInput::GetMouseAbsoluteX()
	{
		return MouseAbsoluteX;
	}

	int CInput::GetMouseAbsoluteY()
	{
		return MouseAbsoluteY;
	}

	int CInput::GetMouseRelativeX()
	{
		return MouseRelativeX;
	}

	int CInput::GetMouseRelativeY()
	{
		return MouseRelativeY;
	}

	float CInput::GetMouseResolution()
	{
		return MouseResolution;
	}

	void CInput::SetMouseResolution(float Res)
	{
		MouseResolution = Res;
	}

	void CInput::SetJoystickSensitivity(float Sens)
	{
		JoystickSensitivity = Sens;
	}

	float CInput::GetJoystickSensitivity()
	{
		return JoystickSensitivity;
	}

	bool CInput::IsKeyDown(int Key)
	{
        //Check key is in range
	    if (Key > 0 && Key < 324)
	    {
            //Check the key array
            if (KeysDown[Key]) return true;
	    }
        else Engine->Logger.AddEntry("Error: Requested key value out of range");

	    return false;

	}

	bool CInput::IsKeyPressed(int Key)
	{
	    //Check key is in range
	    if (Key > 0 && Key < 324)
	    {
            //Check the key array
            if (KeysPressed[Key])
            {
                //Set the key not pressed
                KeysPressed[Key] = false;
                return true;
            }
	    }
	    else Engine->Logger.AddEntry("Error: Requested key value out of range");

	    return false;
	}

	void CInput::WarpMouse(float x, float y)
	{
	    SDL_WarpMouse((int)x, (int)y);
	}

	bool CInput::IsLMBDown()
	{
        return SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1);
	}

	bool CInput::IsMMBDown()
	{
        return SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(2);
	}

	bool CInput::IsRMBDown()
	{
        return SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(3);
	}

} //Namespace

