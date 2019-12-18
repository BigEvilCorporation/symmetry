#include "Game.h"

//Define global game object
CGame *Game = new CGame();

CGame::CGame()
{
	//Set default values
	LogFilename = "project.log";
	CurrentLevelState = 0;
	Debug = false;
}

CGame::~CGame()
{
	//Destroy global game object
	delete Game;
}

bool CGame::Init(std::vector<std::string> Args)
{
	//Set engine media paths
	sym::Engine->SystemPath = "..//system//";
	sym::Engine->ScenePath = "..//scenes//";
	sym::Engine->ScriptsPath = "..//scripts//";
	sym::Engine->ModelsPath = "..//models//";
	sym::Engine->MaterialsPath = "..//materials//";
	sym::Engine->TexturesPath = "..//textures//";
	sym::Engine->HeightmapsPath = "..//textures//heightmaps//";
	sym::Engine->FontsPath = "..//fonts//";
	sym::Engine->VideosPath = "..//videos//";
	sym::Engine->ShadersPath = "..//shaders//";
	sym::Engine->AnimationsPath = "..//animations//";
	sym::Engine->AudioPath = "..//audio//";

	//Open game log
	Log.Init(LogFilename);

	//Register script functions
	RegisterFunctions();

	//Default screen settings
	sym::Engine->ScreenWidth = 1024;
	sym::Engine->ScreenHeight = 768;
	sym::Engine->Fullscreen = false;

	//Scene file to open
	std::string Scene;

	//Audio volume
	float Volume = 1.0f;

	//Process command line args
	for(int i = 0; i < Args.size(); i++)
	{
		//Open map
		if(Args[i] == "-open")
		{
			if(Args.size() >= (i + 1))
			{
				//Set scene file
				Scene = Args[i + 1];

				//Skip next arg
				i++;
			}
			else sym::Engine->Logger.AddEntry("Error: -open takes one parameter");
		}

		//Set screen resolution
		if(Args[i] == "-resolution")
		{
			if(Args.size() >= (i + 2))
			{
				//Set resolution
				sym::Engine->ScreenWidth = atoi(Args[i + 1].c_str());
				sym::Engine->ScreenHeight = atoi(Args[i + 2].c_str());

				//Skip next two args
				i += 2;
			}
			else sym::Engine->Logger.AddEntry("Error: -resolution takes two parameters");
		}

		//Fullscreen
		if(Args[i] == "-fullscreen") sym::Engine->Fullscreen = true;

		//No audio
		if(Args[i] == "-noaudio") Volume = 0.0f;
	}

	//Fire up Symmetry Engine
	if (!sym::Engine->Init()) return false;

	//Set audio volume
	sym::Engine->Audio.SetMasterVolume(Volume);

	//If scene was requested in command line args
	if(Scene.size() > 0)
	{
		//Push back level state
		CLevelState *State = new CLevelState();
		ChangeLevel(State);
		State->LoadScene(Scene);
	}
	else
	{
		//Push back main menu state
		sym::Engine->StateManager.PushState(&MainMenuState);
	}

	return true;
}

void CGame::Shutdown()
{
	//Shutdown engine
	sym::Engine->ShutDown();

	//Close log
	Log.Close();
}

void CGame::SetDebugMode(bool On)
{
	Debug = On;
}

bool CGame::GetDebugMode()
{
	return Debug;
}

bool CGame::ChangeLevel(std::string Filename)
{
	CLevelState *State = new CLevelState();

	ChangeLevel(State);

	State->LoadScene(Filename);

	return true;
}

void CGame::ChangeLevel(CLevelState *LevelState)
{
	if(CurrentLevelState)
	{
		CurrentLevelState->Shutdown();
		delete CurrentLevelState;
	}

	//Set level state
	CurrentLevelState = LevelState;

	//Add to engine state manager
	sym::Engine->StateManager.PushState(CurrentLevelState);
}

void CGame::CloseLevel()
{
	//Check level is the top state
	if(CurrentLevelState)
	{
		if(sym::Engine->StateManager.GetCurrentState() == CurrentLevelState)
		{
			//Shutdown state
			CurrentLevelState->Shutdown();

			//Pop back state
			sym::Engine->StateManager.PopState();

			//Delete level
			//delete CurrentLevelState;
			//CurrentLevelState = 0;
		}
	}
}

CLevelState* CGame::GetCurrentLevel()
{
	return CurrentLevelState;
}