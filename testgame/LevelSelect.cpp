#include "LevelSelect.h"
#include "Game.h"
#include "Playground.h"

void CLevelSelectState::Init()
{
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

	//Load buttons
	ButtonLvl1.Load("button_lvl1.png");
	ButtonLvl2.Load("button_lvl2.png");
	ButtonLvl3.Load("button_lvl3.png");
	ButtonLvl4.Load("button_lvl4.png");
	ButtonCancel.Load("button_cancel.png");

	//Set button positions
	float ButtonsRight = (float)sym::Engine->ScreenWidth - ButtonLvl1.GetSize().x;
	ButtonLvl1.SetPosition(sym::CVector3(ButtonsRight, 220, 0.0f));
	ButtonLvl2.SetPosition(sym::CVector3(ButtonsRight, 180, 0.0f));
	ButtonLvl3.SetPosition(sym::CVector3(ButtonsRight, 140, 0.0f));
	ButtonLvl4.SetPosition(sym::CVector3(ButtonsRight, 100, 0.0f));
	ButtonCancel.SetPosition(sym::CVector3(ButtonsRight, 60, 0.0f));

	//Set button tint colour
	Tint = sym::CColour(100, 100, 100);
}

void CLevelSelectState::Shutdown()
{
}

void CLevelSelectState::Pause()
{
}

void CLevelSelectState::Resume()
{
	//Let mouse leave window
	sym::Engine->Input.ReleaseMouse();

	//Centre mouse cursor
	sym::Engine->Input.WarpMouse((float)sym::Engine->ScreenWidth / 2.0f, (float)sym::Engine->ScreenHeight / 2.0f);
}

void CLevelSelectState::HandleInput()
{
	//Get mouse input
	MouseX = sym::Engine->Input.GetMouseAbsoluteX();
	MouseY = sym::Engine->Input.GetMouseAbsoluteY();

	//Check for ESC button, shutdown state
	if(sym::Engine->Input.IsKeyPressed(SDLK_ESCAPE)) sym::Engine->StateManager.PopState();
}

void CLevelSelectState::Update()
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
}

void CLevelSelectState::Render()
{
	//Start rendering
	sym::StartRendering();

		//Start 2D rendering
		sym::glEnable2D();

			//Render buttons
			ButtonLvl1.Render();
			ButtonLvl2.Render();
			ButtonLvl3.Render();
			ButtonLvl4.Render();
			ButtonCancel.Render();

			//Render mouse cursor
			MouseCursor.Render();

			//Render console
			if(sym::Engine->Console.IsOpen) sym::Engine->Console.Render();

		sym::glDisable2D();

	sym::FinishRendering();
}

void CLevelSelectState::CheckButtons()
{
	//Reset button tints
	ButtonLvl1.SetRenderTint(White);
	ButtonLvl2.SetRenderTint(White);
	ButtonLvl3.SetRenderTint(White);
	ButtonLvl4.SetRenderTint(White);
	ButtonCancel.SetRenderTint(White);

	//Check for mouse over
	if(ButtonLvl1.CheckCollision(sym::CVector3((float)MouseX, (float)sym::Engine->ScreenHeight - MouseY, 0.0f)))
	{
		//Set render tint
		ButtonLvl1.SetRenderTint(Tint);

		//If mouse is pressed, push back game state
		if(sym::Engine->Input.IsLMBDown())
		{
			Game->ChangeLevel("level1.sym.scene");
		}
	}

	if(ButtonLvl2.CheckCollision(sym::CVector3((float)MouseX, (float)sym::Engine->ScreenHeight - MouseY, 0.0f)))
	{
		//Set render tint
		ButtonLvl2.SetRenderTint(Tint);

		//If mouse is pressed, push back game state
		if(sym::Engine->Input.IsLMBDown())
		{
			Game->ChangeLevel("level2.sym.scene");
		}
	}

	if(ButtonLvl3.CheckCollision(sym::CVector3((float)MouseX, (float)sym::Engine->ScreenHeight - MouseY, 0.0f)))
	{
		//Set render tint
		ButtonLvl3.SetRenderTint(Tint);

		//If mouse is pressed, push back game state
		if(sym::Engine->Input.IsLMBDown())
		{
			Game->ChangeLevel("level3.sym.scene");
		}
	}

	if(ButtonLvl4.CheckCollision(sym::CVector3((float)MouseX, (float)sym::Engine->ScreenHeight - MouseY, 0.0f)))
	{
		//Set render tint
		ButtonLvl4.SetRenderTint(Tint);

		//If mouse is pressed, push back game state
		if(sym::Engine->Input.IsLMBDown())
		{
			Game->ChangeLevel("level4.sym.scene");
		}
	}

	if(ButtonCancel.CheckCollision(sym::CVector3((float)MouseX, (float)sym::Engine->ScreenHeight - MouseY, 0.0f)))
	{
		//Set render tint
		ButtonCancel.SetRenderTint(Tint);

		//If mouse is pressed, shutdown state
		if(sym::Engine->Input.IsLMBDown()) sym::Engine->StateManager.PopState();
	}
}