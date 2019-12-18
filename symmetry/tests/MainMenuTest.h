#include "../SYM_Engine.h"
#include "../SYM_SDL_GL_Utils.h"
#include "../SYM_Sprite.h"
#include "../SYM_Font.h"

#include "CharacterTest.h"
#include "HoverGame.h"
#include "RopeDemo.h"
#include "VisionTest.h"

class CMainMenu : public sym::CState
{
	public:
		void Init()
		{
			//Load font for loading screen
			Font.LoadTTFFont("arial.ttf", 24);

			//Start drawing loading screen
			sym::StartRendering();

				//Enable 2D rendering
				sym::glEnable2D();

					//Render loading text
					Font.RenderText("[ Loading ]", 
									sym::Engine->ScreenWidth / 2 - Font.GetTextWidth("[ Loading ]") / 2, 
									sym::Engine->ScreenHeight / 2);

				//Disable 2D rendering
				sym::glDisable2D();

			//Finish drawing loading screen
			sym::FinishRendering();

			MouseOverOptions = false;
			MouseOverQuit = false;
			MouseOverCharacter = false;
			MouseOverVision = false;
			MouseOverRope = false;
			MouseOverShooting = false;
			MouseOverHovercraft = false;

			Layer1Rot = 0.0f;
			Layer2Rot = 0.0f;

			OptionsButton.Load("buttons//btn_options.png");
			QuitButton.Load("buttons//btn_quit.png");

			CharacterButton.Load("buttons//btn_character_demo.png");
			VisionButton.Load("buttons//btn_vision_demo.png");
			RopeButton.Load("buttons//btn_rope_demo.png");
			ShootingButton.Load("buttons//btn_shooting_demo.png");
			HovercraftButton.Load("buttons//btn_hovercraft_demo.png");

			CharacterButton.SetPosition(sym::CVector3(sym::Engine->ScreenWidth / 3, 250.0f, 0.0f));
			VisionButton.SetPosition(sym::CVector3((sym::Engine->ScreenWidth / 3) * 2, 250.0f, 0.0f));

			RopeButton.SetPosition(sym::CVector3(sym::Engine->ScreenWidth / 3, 400.0f, 0.0f));
			ShootingButton.SetPosition(sym::CVector3((sym::Engine->ScreenWidth / 3) * 2, 400.0f, 0.0f));

			HovercraftButton.SetPosition(sym::CVector3(sym::Engine->ScreenWidth / 2, 550.0f, 0.0f));

			OptionsButton.SetPosition(sym::CVector3(sym::Engine->ScreenWidth / 3, 100.0f, 0.0f));
			QuitButton.SetPosition(sym::CVector3((sym::Engine->ScreenWidth / 3) * 2, 100.0f, 0.0f));

			MouseCursor.Load("MouseCursor.png");

			Music.Load("robotnik.mp3", sym::SOUND_2D);
			//Music.Play(sym::SOUND_ONCE);
			//Sound_Hover.Load("menu_hover.ogg", sym::SOUND_2D);
			//Sound_Select.Load("menu_select.ogg", sym::SOUND_2D);
			Sound_Hover.SetVolume(0.2f);
			Sound_Select.SetVolume(0.2f);
		}

		void Shutdown()
		{
		}

		void Pause()
		{
			//Music.Stop();
		}

		void Resume()
		{
			//Music.Play(sym::SOUND_ONCE);
		}

		void HandleInput()
		{
			//Get mouse absolute coords
			MouseX = (float)sym::Engine->Input.GetMouseAbsoluteX();
			MouseY = (float)sym::Engine->Input.GetMouseAbsoluteY();

			//Escape key - pop this state
			if(sym::Engine->Input.IsKeyPressed(SDLK_ESCAPE)) sym::Engine->StateManager.PopState();

			//Check if demo buttons are hovered over/clicked
			if(HovercraftButton.CheckCollision(sym::CVector3(MouseX, sym::Engine->ScreenHeight - MouseY + MouseCursor.GetSize().y, 0)))
			{
				if(!MouseOverHovercraft)
				{
					Sound_Hover.Play(sym::SOUND_ONCE);
					HovercraftButton.SetRenderTint(sym::CColour(150, 150, 150));
				}
				MouseOverHovercraft = true;

				if(sym::Engine->Input.IsLMBDown())
				{
					Sound_Select.Play(sym::SOUND_ONCE);
					sym::Engine->StateManager.PushState(&HovercraftDemoState);
				}
			}
			else
			{
				MouseOverHovercraft = false;
				HovercraftButton.SetRenderTint(sym::CColour(255, 255, 255));
			}

			if(RopeButton.CheckCollision(sym::CVector3(MouseX, sym::Engine->ScreenHeight - MouseY + MouseCursor.GetSize().y, 0)))
			{
				if(!MouseOverRope)
				{
					Sound_Hover.Play(sym::SOUND_ONCE);
					RopeButton.SetRenderTint(sym::CColour(150, 150, 150));
				}
				MouseOverRope = true;

				if(sym::Engine->Input.IsLMBDown())
				{
					Sound_Select.Play(sym::SOUND_ONCE);
					sym::Engine->StateManager.PushState(&RopeDemoState);
				}
			}
			else
			{
				MouseOverRope = false;
				RopeButton.SetRenderTint(sym::CColour(255, 255, 255));
			}
		
			if(CharacterButton.CheckCollision(sym::CVector3(MouseX, sym::Engine->ScreenHeight - MouseY + MouseCursor.GetSize().y, 0)))
			{
				if(!MouseOverCharacter)
				{
					Sound_Hover.Play(sym::SOUND_ONCE);
					CharacterButton.SetRenderTint(sym::CColour(150, 150, 150));
				}
				MouseOverCharacter = true;

				if(sym::Engine->Input.IsLMBDown())
				{
					Sound_Select.Play(sym::SOUND_ONCE);
					sym::Engine->StateManager.PushState(&CharacterDemoState);
				}
			}
			else
			{
				MouseOverCharacter = false;
				CharacterButton.SetRenderTint(sym::CColour(255, 255, 255));
			}

			if(VisionButton.CheckCollision(sym::CVector3(MouseX, sym::Engine->ScreenHeight - MouseY + MouseCursor.GetSize().y, 0)))
			{
				if(!MouseOverVision)
				{
					Sound_Hover.Play(sym::SOUND_ONCE);
					VisionButton.SetRenderTint(sym::CColour(150, 150, 150));
				}
				MouseOverVision = true;

				if(sym::Engine->Input.IsLMBDown())
				{
					Sound_Select.Play(sym::SOUND_ONCE);
					sym::Engine->StateManager.PushState(&VisionDemoState);
				}
			}
			else
			{
				MouseOverVision = false;
				VisionButton.SetRenderTint(sym::CColour(255, 255, 255));
			}

			if(ShootingButton.CheckCollision(sym::CVector3(MouseX, sym::Engine->ScreenHeight - MouseY + MouseCursor.GetSize().y, 0)))
			{
				if(!MouseOverShooting)
				{
					Sound_Hover.Play(sym::SOUND_ONCE);
					ShootingButton.SetRenderTint(sym::CColour(150, 150, 150));
				}
				MouseOverShooting = true;

				if(sym::Engine->Input.IsLMBDown())
				{
					Sound_Select.Play(sym::SOUND_ONCE);
					//sym::Engine->StateManager.PushState(&CharacterDemoState);
				}
			}
			else
			{
				MouseOverShooting = false;
				ShootingButton.SetRenderTint(sym::CColour(255, 255, 255));
			}

			//Check if options button is clicked
			if(OptionsButton.CheckCollision(sym::CVector3(MouseX, sym::Engine->ScreenHeight - MouseY + MouseCursor.GetSize().y, 0)))
			{
				if(!MouseOverOptions)
				{
					Sound_Hover.Play(sym::SOUND_ONCE);
					OptionsButton.SetRenderTint(sym::CColour(150, 150, 150));
				}
				MouseOverOptions = true;

				if(sym::Engine->Input.IsLMBDown())
				{
					Sound_Select.Play(sym::SOUND_ONCE);
					sym::Engine->StateManager.PopState();
				}
			}
			else
			{
				MouseOverOptions = false;
				OptionsButton.SetRenderTint(sym::CColour(255, 255, 255));
			}

			//Check if quit button is clicked
			if(QuitButton.CheckCollision(sym::CVector3(MouseX, sym::Engine->ScreenHeight - MouseY + MouseCursor.GetSize().y, 0)))
			{
				if(!MouseOverQuit)
				{
					Sound_Hover.Play(sym::SOUND_ONCE);
					QuitButton.SetRenderTint(sym::CColour(150, 150, 150));
				}
				MouseOverQuit = true;

				if(sym::Engine->Input.IsLMBDown())
				{
					sym::Engine->StateManager.PopState();
				}
			}
			else
			{
				MouseOverQuit = false;
				QuitButton.SetRenderTint(sym::CColour(255, 255, 255));
			}
		}

		void Update()
		{
			//Set mouse cursor position
			MouseCursor.SetPosition(sym::CVector3(MouseX, sym::Engine->ScreenHeight - MouseY, 0.0f));
		}

		void Render()
		{
			//Clear screen
			glClear(GL_COLOR_BUFFER_BIT);

			//Enable 2D rendering
			sym::glEnable2D();

				//Render sprites
				OptionsButton.Render();
				QuitButton.Render();

				CharacterButton.Render();
				VisionButton.Render();
				RopeButton.Render();
				ShootingButton.Render();
				HovercraftButton.Render();

				Font.RenderText("Symmetry Engine Beta :: Demo menu", 10, 700);

				//Render mouse cursor
				MouseCursor.Render();

				if(sym::Engine->Console.IsOpen) sym::Engine->Console.Render();

			//Disable 2D rendering
			sym::glDisable2D();

			//Flip buffers
			SDL_GL_SwapBuffers();
		}

	private:
		sym::CSprite Logo;
		sym::CSprite OptionsButton;
		sym::CSprite QuitButton;
		sym::CSprite CharacterButton;
		sym::CSprite VisionButton;
		sym::CSprite RopeButton;
		sym::CSprite ShootingButton;
		sym::CSprite HovercraftButton;
		sym::CSprite MouseCursor;

		sym::CFont Font;

		sym::CSound Music;
		sym::CSound Sound_Hover;
		sym::CSound Sound_Select;

		float MouseX, MouseY;
		float Layer1Rot;
		float Layer2Rot;

		bool MouseOverOptions;
		bool MouseOverQuit;
		bool MouseOverCharacter;
		bool MouseOverVision;
		bool MouseOverRope;
		bool MouseOverShooting;
		bool MouseOverHovercraft;

		CRopeDemoState RopeDemoState;
		CHoverGameState HovercraftDemoState;
		CCharTestState CharacterDemoState;
		CVisionTestState VisionDemoState;
};