#include "MainMenu.h"
#include "Game.h"
#include "Playground.h"
#include "LevelSelect.h"

void CMainMenuState::Init()
{
	//If this is the first time the menu has been loaded
	if(FirstRun)
	{
		//Add intro video state
		sym::Engine->StateManager.PushState(&VideoState);

		//No longer the first run
		FirstRun = false;
	}

	//Load font file
	Font.LoadTTFFont("arial.ttf", 14);

	//Load mouse cursor
	MouseCursor.Load("mouse_cursor.png");

	//Set initial mouse coords
	MouseX = sym::Engine->ScreenWidth / 2;
	MouseY = sym::Engine->ScreenHeight / 2;

	//Set mouse rotation speed;
	MouseRotation = 0.0f;
	MouseRotateSpeed = 0.05f;

	//Load logo
	Logo.Load("game_logo_256.png");
	Logo.SetPosition(sym::CVector3((float)sym::Engine->ScreenWidth / 2, (float)sym::Engine->ScreenHeight / 2, 0.0f));

	//Load buttons
	ButtonStart.Load("button_newgame.png");
	ButtonLoad.Load("button_loadgame.png");
	ButtonSettings.Load("button_settings.png");
	ButtonCredits.Load("button_credits.png");
	ButtonQuit.Load("button_quit.png");

	//Set button positions
	float ButtonsRight = (float)sym::Engine->ScreenWidth - ButtonStart.GetSize().x;
	ButtonStart.SetPosition(sym::CVector3(ButtonsRight, 220, 0.0f));
	ButtonLoad.SetPosition(sym::CVector3(ButtonsRight, 180, 0.0f));
	ButtonSettings.SetPosition(sym::CVector3(ButtonsRight, 140, 0.0f));
	ButtonCredits.SetPosition(sym::CVector3(ButtonsRight, 100, 0.0f));
	ButtonQuit.SetPosition(sym::CVector3(ButtonsRight, 60, 0.0f));

	//Set button tint colour
	Tint = sym::CColour(100, 100, 100);

	//Load title track
	TitleTrack.Load("title_theme.ogg", sym::SOUND_2D, true);

	//Load video
	Video.Load("menu_background.mpg");

	//Set video canvas
	VideoCanvas.Load(Video.Texture);
	VideoCanvas.SetSize(sym::CVector3((float)sym::Engine->ScreenWidth / 1.5f, (float)sym::Engine->ScreenHeight / 1.5f, 0.0f));
	VideoCanvas.SetPosition(sym::CVector3((float)sym::Engine->ScreenWidth / 2.0f, (float)sym::Engine->ScreenHeight / 2.0f, 0.0f));

	//Start playing
	Video.Play();

	//Centre mouse cursor
	sym::Engine->Input.WarpMouse((float)sym::Engine->ScreenWidth / 2.0f, (float)sym::Engine->ScreenHeight / 2.0f);

	//sym::Engine->Audio.SetMasterVolume(0.0f);
}

void CMainMenuState::Shutdown()
{
	//Stop title track
	TitleTrack.Stop();
	Video.Close();
}

void CMainMenuState::Pause()
{
	//Stop title track
	TitleTrack.Stop();
}

void CMainMenuState::Resume()
{
	//Play title track
	TitleTrack.Play();

	//Let mouse leave window
	sym::Engine->Input.ReleaseMouse();
}

void CMainMenuState::HandleInput()
{
	//Get mouse input
	MouseX = sym::Engine->Input.GetMouseAbsoluteX();
	MouseY = sym::Engine->Input.GetMouseAbsoluteY();

	//Check for ESC button, shutdown game
	if(sym::Engine->Input.IsKeyPressed(SDLK_ESCAPE)) Game->Shutdown();
}

void CMainMenuState::Update()
{
	//Check mouse is within screen bounds
	if(MouseX < 0) MouseX = 0;
	if(MouseY < 0) MouseY = 0;
	if(MouseX > sym::Engine->ScreenWidth) MouseX = sym::Engine->ScreenWidth;
	if(MouseY > sym::Engine->ScreenHeight) MouseY = sym::Engine->ScreenHeight;

	//Rotate mouse using delta time
	MouseRotation += MouseRotateSpeed * sym::Engine->GetDeltaTime();
	if(MouseRotation > 360.0f) MouseRotation = 0.0f;
	MouseCursor.SetRotation(sym::CQuaternion(-MouseRotation, sym::CVector3(0.0f, 0.0f, 1.0f)));

	//Set mouse position
	MouseCursor.SetPosition(sym::CVector3((float)MouseX, (float)sym::Engine->ScreenHeight - (float)MouseY, 0.0f));

	//Update buttons, check for presses
	CheckButtons();

	//Get next video frame
	if(!Video.ReadFrame())
	{
		Video.SeekBackward(0);
		Video.Play();
	}
}

void CMainMenuState::Render()
{
	//Start rendering
	sym::StartRendering();

		//Start 2D rendering
		sym::glEnable2D();

			//Render background
			//VideoCanvas.Render();
			Logo.Render();
				
			//Render text
			Font.RenderText("Beartrap Interactive :: SASHA", 20, 40);

			//Render buttons
			ButtonStart.Render();
			ButtonLoad.Render();
			ButtonSettings.Render();
			ButtonCredits.Render();
			ButtonQuit.Render();

			//Render mouse cursor
			MouseCursor.Render();

			//Render console
			if(sym::Engine->Console.IsOpen) sym::Engine->Console.Render();

		sym::glDisable2D();

	sym::FinishRendering();
}

void CMainMenuState::CheckButtons()
{
	//Reset button tints
	ButtonStart.SetRenderTint(White);
	ButtonLoad.SetRenderTint(White);
	ButtonSettings.SetRenderTint(White);
	ButtonCredits.SetRenderTint(White);
	ButtonQuit.SetRenderTint(White);

	//Check for mouse over
	if(ButtonStart.CheckCollision(sym::CVector3((float)MouseX, (float)sym::Engine->ScreenHeight - MouseY, 0.0f)))
	{
		//Set render tint
		ButtonStart.SetRenderTint(Tint);

		//If mouse is pressed, push back game state
		if(sym::Engine->Input.IsLMBDown())
		{
			CLevelSelectState *State = new CLevelSelectState();

			sym::Engine->StateManager.PushState(State);
		}
	}

	if(ButtonLoad.CheckCollision(sym::CVector3((float)MouseX, (float)sym::Engine->ScreenHeight - MouseY, 0.0f)))
	{
		//Set render tint
		ButtonLoad.SetRenderTint(Tint);

		//If mouse is pressed, push back game state
		//if(sym::Engine->Input.IsLMBDown())
	}

	if(ButtonSettings.CheckCollision(sym::CVector3((float)MouseX, (float)sym::Engine->ScreenHeight - MouseY, 0.0f)))
	{
		//Set render tint
		ButtonSettings.SetRenderTint(Tint);

		//If mouse is pressed, push back game state
		//if(sym::Engine->Input.IsLMBDown())
	}

	if(ButtonCredits.CheckCollision(sym::CVector3((float)MouseX, (float)sym::Engine->ScreenHeight - MouseY, 0.0f)))
	{
		//Set render tint
		ButtonCredits.SetRenderTint(Tint);

		//If mouse is pressed, push back game state
		//if(sym::Engine->Input.IsLMBDown())
	}

	if(ButtonQuit.CheckCollision(sym::CVector3((float)MouseX, (float)sym::Engine->ScreenHeight - MouseY, 0.0f)))
	{
		//Set render tint
		ButtonQuit.SetRenderTint(Tint);

		//If mouse is pressed, shutdown game
		if(sym::Engine->Input.IsLMBDown()) Game->Shutdown();
	}
}