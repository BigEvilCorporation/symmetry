#include "../SYM_Engine.h"
#include "../SYM_SDL_GL_Utils.h"
#include "../SYM_Sprite.h"
#include "../SYM_Font.h"

class CInGameMenu : public sym::CState
{
	public:
		void Init()
		{
			Layer1Rot = 0.0f;
			Layer2Rot = 0.0f;

			BkLayer1.Load("menu_bk_layer1.png");
			BkLayer2.Load("menu_bk_layer2.png");
			MouseCursor.Load("mousecursor.png");
			ResumeButton.Load("start_128.png");
			QuitButton.Load("exit_128.png");

			ResumeButton.SetPosition(sym::CVector3(sym::Engine->ScreenWidth / 3, 100.0f, 0.0f));
			QuitButton.SetPosition(sym::CVector3((sym::Engine->ScreenWidth / 3) * 2, 100.0f, 0.0f));
			BkLayer1.SetPosition(sym::CVector3(0, sym::Engine->ScreenHeight, 0));
			BkLayer2.SetPosition(sym::CVector3(0, sym::Engine->ScreenHeight, 0));
			BkLayer1.SetScale(sym::CVector3(8, 8, 0));
			BkLayer2.SetScale(sym::CVector3(8, 8, 0));

			Font.LoadTTFFont("arial.ttf", 24);
		}

		void Shutdown()
		{
		}

		void Pause()
		{
		}

		void Resume()
		{
		}

		void HandleInput()
		{
			//Get mouse absolute coords
			MouseX = (float)sym::Engine->Input.GetMouseAbsoluteX();
			MouseY = (float)sym::Engine->Input.GetMouseAbsoluteY();

			//Escape key - pop this state
			if(sym::Engine->Input.IsKeyPressed(SDLK_ESCAPE)) sym::Engine->StateManager.PopState();

			//Check if quit button is clicked
			if(sym::Engine->Input.IsLMBDown())
				if(QuitButton.CheckCollision(sym::CVector3(MouseX, sym::Engine->ScreenHeight - MouseY + MouseCursor.GetSize().y, 0)))
					sym::Engine->ShutDown();

			//Check if resume button is clicked
			if(sym::Engine->Input.IsLMBDown())
				if(ResumeButton.CheckCollision(sym::CVector3(MouseX, sym::Engine->ScreenHeight - MouseY + MouseCursor.GetSize().y, 0)))
					sym::Engine->StateManager.PopState();
		}

		void Update()
		{
			//Set mouse cursor position
			MouseCursor.SetPosition(sym::CVector3(MouseX, sym::Engine->ScreenHeight - MouseY, 0.0f));

			Layer1Rot += sym::Engine->GetDeltaTime() * 0.01f;
			Layer2Rot -= sym::Engine->GetDeltaTime() * 0.01f;
			if(Layer1Rot > 360) Layer1Rot = 0;
			if(Layer2Rot < 0) Layer2Rot = 360;

			BkLayer1.SetRotation(sym::CVector3(0, 0, Layer1Rot));
			BkLayer2.SetRotation(sym::CVector3(0, 0, Layer2Rot));
		}

		void Render()
		{
			//Clear screen
			glClear(GL_COLOR_BUFFER_BIT);

			//Enable 2D rendering
			sym::glEnable2D();

				//Render sprites
				BkLayer1.Render();
				BkLayer2.Render();
				ResumeButton.Render();
				QuitButton.Render();

				//Render text
				Font.RenderText("[Insert in-game menu here]", 400, 400);

				//Render mouse cursor
				MouseCursor.Render();

			//Disable 2D rendering
			sym::glDisable2D();

			//Flip buffers
			SDL_GL_SwapBuffers();
		}

	private:

		sym::CSprite BkLayer1;
		sym::CSprite BkLayer2;
		sym::CSprite ResumeButton;
		sym::CSprite QuitButton;
		sym::CSprite MouseCursor;

		sym::CFont Font;

		float MouseX, MouseY;
		float Layer1Rot;
		float Layer2Rot;
};