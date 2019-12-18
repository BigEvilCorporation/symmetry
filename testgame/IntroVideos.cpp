#include "IntroVideos.h"

void CIntroVideoState::Init()
{
	//Load MPEG files
	sym::CMpeg *SymmetryIntro = new sym::CMpeg;
	SymmetryIntro->Load("symmetry_intro.mpg");
	//SymmetryIntro->Load("bizkit.mpg");
	Videos.push_back(SymmetryIntro);

	sym::CMpeg *BeartrapIntro = new sym::CMpeg;
	BeartrapIntro->Load("beartrap_intro.mpg");
	Videos.push_back(BeartrapIntro);

	//Get and start playing first video
	CurrentVideo = Videos.begin();
	(*CurrentVideo)->Play();
}

void CIntroVideoState::Shutdown()
{
	//Close and release all videos
	for(CurrentVideo = Videos.begin(); CurrentVideo != Videos.end(); CurrentVideo++)
	{
		(*CurrentVideo)->Close();
		delete (*CurrentVideo);
	}

	//Clear the screen
	sym::StartRendering();
	sym::FinishRendering();
}

void CIntroVideoState::Pause()
{
	//Do nothing, function not supported here
}

void CIntroVideoState::Resume()
{
	//Do nothing, function not supported here
}

void CIntroVideoState::HandleInput()
{
	//Check if ESC or SPACE keys are pressed, skip current video
	if(sym::Engine->Input.IsKeyPressed(SDLK_ESCAPE) || sym::Engine->Input.IsKeyPressed(SDLK_SPACE))
	{
		//Check if end of videos has been reached
		if(CurrentVideo + 1 != Videos.end())
		{
			//Get next video and play it
			(*CurrentVideo)->Stop();
			CurrentVideo++;
			(*CurrentVideo)->Play();
		}
		//End of videos, pop this state
		else sym::Engine->StateManager.PopState();
	}
}

void CIntroVideoState::Update()
{
	//Get next frame of current video
	if(!(*CurrentVideo)->ReadFrame())
	{
		//Check if end of videos has been reached
		if(CurrentVideo + 1 != Videos.end())
		{
			//Get next video and play it
			CurrentVideo++;
			(*CurrentVideo)->Play();
		}
		//End of videos, pop this state
		else sym::Engine->StateManager.PopState();
	}
}

void CIntroVideoState::Render()
{
	
	sym::StartRendering();
		sym::glEnable2D();

			//Render Mpeg stream onto a quad
			(*CurrentVideo)->RenderFrame();

			//Render console
			if(sym::Engine->Console.IsOpen) sym::Engine->Console.Render();

		sym::glDisable2D();
	sym::FinishRendering();
}