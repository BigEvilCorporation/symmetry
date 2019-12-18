#pragma once

#include "SYM_Core.h"

#include "Logger.h"
#include "Scripting.h"
#include "MainMenu.h"
#include "Level.h"

class CGame
{
	public:
		CGame();
		~CGame();

		//Initialise engine, create log file, etc
		bool Init(std::vector<std::string> Args);

		//Shutdown engine, close log file, etc
		void Shutdown();

		//Change level
		bool ChangeLevel(std::string Filename);
		void ChangeLevel(CLevelState *LevelState);

		//Get current level
		CLevelState* GetCurrentLevel();

		//Close current level
		void CloseLevel();

		//Log file
		sym::CLogger Log;

		//Set debug mode
		void SetDebugMode(bool On);
		bool GetDebugMode();

	private:
		//Log filename
		std::string LogFilename;

		//Main menu state
		CMainMenuState MainMenuState;

		//Current level state
		CLevelState *CurrentLevelState;

		//Debug mode
		bool Debug;
};

//Global game object
extern CGame *Game;