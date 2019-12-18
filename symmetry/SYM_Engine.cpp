#include "SYM_Engine.h"

namespace sym
{
	//Engine constructor
	CEngine::CEngine()
	{
		//Some default values
		ScreenWidth = 1024;
		ScreenHeight = 768;
		Fullscreen = true;
		Title = "Symmetry Engine";

		SystemPath = "system//";
		ScenePath = "scenes//";
		ScriptsPath = "scripts//";
		ModelsPath = "models//";
		MaterialsPath = "materials//";
		TexturesPath = "textures//";
		HeightmapsPath = "textures//heightmaps//";
		FontsPath = "fonts//";
		VideosPath = "videos//";
		ShadersPath = "shaders//";
		AnimationsPath = "animations//";
		AudioPath = "audio//";

		CurrTime = 0;
		StartTime = 0;
		OldTime = 0;
		ElapsedTime = 0;
		DeltaTime = 0;
	}

	bool CEngine::Init()
	{
		IsRunning = true;
		CleanupDone = false;

		//Greetings
		std::cout << "\n\nSYMMETRY ENGINE **DEVELOPMENT** v0.5\n\n";
		std::cout << "(C) Matt Phillips 2006, 2007, 2008\n";
		std::cout << "http://www.hysteriagaming.com\n\n";

		//Initialise logger
		std::cout << "Initialising Logger..." << std::endl;
		if (!Logger.Init("symmetry.log")) return false;

		//Initialise SDL
		Logger.AddEntry("Initialising SDL");
		if (!SDL_GL_InitSDL(ScreenWidth, ScreenHeight, Fullscreen, Title)) return false;

		//Seed random number generator
		srand((unsigned) SDL_GetTicks());

		//Initialise OpenGL
		Logger.AddEntry("Initialising OpenGL");
		if (!SDL_GL_InitGL(ScreenWidth, ScreenHeight)) return false;

		//Initialise OpenGL extensions
		Logger.AddEntry("Initialising OpenGL extensions");
		SDL_GL_InitGLExt();

		//Initialise SDL font library
		Logger.AddEntry("Initialising TTF");
		if (TTF_Init() == 1) return false;

		/*
		//Initialise MPEG
		Logger.AddEntry("Initialising MPEG");
		if (!MPEGPlayback.Init()) return false;
		*/

		//Initialise input subsystem
		Logger.AddEntry("Initialising Input");
		if (!Input.Init()) return false;

		//initialise scripting subsystem
		Logger.AddEntry("Initialising Scripting");
		if (!ScriptManager.Init()) return false;

		//Initialise console
		Logger.AddEntry("Initialising Console");
		if (!Console.Init()) return false;

		//Initialise shader manager
		Logger.AddEntry("Initialising Shaders");
		if (!ShaderManager.Init()) return false;

		//Initialise physics
		Logger.AddEntry("Initialising Physics");
		if (!Physics.Init()) return false;

		//Initialise audio
		Logger.AddEntry("Initialising Audio");
		if (!Audio.Init()) return false;

		//Get start time
		StartTime = SDL_GetTicks();

		//Get OpenGL info
		char *Vendor = (char*)glGetString(GL_VENDOR);
		char *Renderer = (char*)glGetString(GL_RENDERER);
		char *Version = (char*)glGetString(GL_VERSION);
		char *Extensions = (char*)glGetString(GL_EXTENSIONS);

		Logger.AddEntry("OpenGL Vendor: " + (std::string)Vendor);
		Logger.AddEntry("OpenGL Renderer: " + (std::string)Renderer);
		Logger.AddEntry("OpenGL Version: " + (std::string)Version);

		//Load the startup script
		Logger.AddEntry("Loading startup script 'symmetry.lua'");
		if(!ScriptManager.LoadScript("_symmetry", "symmetry.lua"))
		{
			Logger.AddEntry("Error: Could not load startup script 'symmetry.lua', check configuration");
			Console.PrintLine("> Error: Could not load startup script 'symmetry.lua', check configuration");
		}

		//Run the startup script
		ScriptManager.RunScript("_symmetry");

		return true;
	}

	void CEngine::ShutDown()
	{
		IsRunning = false;
	}

	void CEngine::Cleanup()
	{
		if(!CleanupDone)
		{
			//Console out
			Logger.AddEntry("Engine shutting down...");

			//Shutdown state manager
			Logger.AddEntry("Shutting down State Manager");
			StateManager.Shutdown();

			//Shutdown physics
			Logger.AddEntry("Shutting down Physics");
			Physics.Shutdown();

			//Shutdown scripting
			Logger.AddEntry("Shutting down Scripting");
			ScriptManager.Shutdown();

			//Shutdown SDL
			Logger.AddEntry("Shutting down SDL");
			SDL_Quit();

			//Shutdown logger
			Logger.AddEntry("Shutting down Logger");
			Logger.Close();
		}

		CleanupDone = true;
	}

	void CEngine::UpdateGlobalTimer()
	{
		//Update global timer
		CurrTime = SDL_GetTicks();
		ElapsedTime = CurrTime + StartTime;
		DeltaTime = CurrTime - OldTime;
		OldTime = CurrTime;
		if (DeltaTime < 0) DeltaTime = 0;

		//Calculate FPS
		CalculateFPS();
	}

	bool CEngine::Update()
	{
		if(!IsRunning)
		{
			Cleanup();
			return false;
		}

		UpdateGlobalTimer();

		//Update input subsystem
		if(!Input.UpdateInput()) ShutDown();
		StateManager.HandleInput();

		//Tick all scripts
		ScriptManager.TickScripts();

		//Update state
		StateManager.Update();

		//Update physics
		Physics.Update();

		//Update audio system
		Audio.Update();

		//Render state
		StateManager.Render();

		return true;
	}

	int CEngine::GetElapsedTime()
	{
		return ElapsedTime;
	}

	float CEngine::GetDeltaTime()
	{
		return static_cast<float> (DeltaTime);
	}

	CEngine* Engine = new CEngine();

} //Namespace

