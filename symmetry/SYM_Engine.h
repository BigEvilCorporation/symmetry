#ifndef __SYM_ENGINE
#define __SYM_ENGINE

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include <SDL/SDL.h>
#include <string>

#include <GL/gl.h>
#include <GL/glext.h>

#include "SYM_Animation.h"
#include "SYM_Audio.h"
#include "SYM_Console.h"
#include "SYM_Font.h"
#include "SYM_Input.h"
#include "SYM_Logger.h"
#include "SYM_Material.h"
#include "SYM_Maths.h"
#include "SYM_Mesh.h"
#include "SYM_MPEG.h"
#include "SYM_Physics.h"
#include "SYM_Resource.h"
#include "SYM_Scene.h"
#include "SYM_Sprite.h"
#include "SYM_State.h"
#include "SYM_Script.h"
#include "SYM_SDL_GL_Utils.h"
#include "SYM_Shader.h"
#include "SYM_Texture.h"
#include "SYM_Timer.h"

namespace sym
{
	//Resource manager collection to keep things tidy
	struct SYM_MANAGERS
	{
		CResourceManager<CMesh> Meshes;
		CResourceManager<CSkeletalAnimation> Animations;
		CResourceManager<CMaterial> Materials;
		CResourceManager<CTexture> Textures;
		CResourceManager<CShader> Shaders;
		CResourceManager<CMpeg> Videos;
		CResourceManager<CTerrain> Terrains;
		CResourceManager<CSound> Audio;
	};

	class CEngine
	{
 		public:
			CEngine();

			//Init and shutdown the engine
			bool Init();
			void ShutDown();
			void Cleanup();

			//Update engine subsystems - keyboard/mouse, timing, etc
			bool Update();

			void UpdateGlobalTimer();

			//Get time passed since engine init
			int GetElapsedTime();

			//Get time passed since last call
			float GetDeltaTime();

			//Resource managers
			SYM_MANAGERS ResourceManager;

			//Resource managers for scripting and object naming
			CScriptManager ScriptManager;
			CFontManager FontManager;
			CSpriteManager SpriteManager;
			CShaderManager ShaderManager;

			//State manager
			CStateManager StateManager;

            CLogger Logger;
			//CScene Scene;
			CConsole Console;
			CInput Input;
			CMpeg MPEGPlayback;
			CPhysicsWorld Physics;
			CAudioSystem Audio;

			int ScreenWidth;
			int ScreenHeight;
			bool Fullscreen;
			std::string Title;

            //File paths
            std::string SystemPath;
            std::string ScenePath;
			std::string ScriptsPath;
			std::string ModelsPath;
			std::string MaterialsPath;
			std::string TexturesPath;
			std::string HeightmapsPath;
			std::string FontsPath;
			std::string VideosPath;
			std::string ShadersPath;
			std::string AnimationsPath;
			std::string AudioPath;

			bool Running() { return IsRunning; }

		private:

			//Time engine started
			int StartTime;

			//Time elapsed since engine start
			int ElapsedTime;

			//Current time
			int CurrTime;

			//Delta time
			int DeltaTime;

			//Old time (for delta)
			int OldTime;

			//Is engine running
            bool IsRunning;

			//Has cleanup already beend one
			bool CleanupDone;
	};

	extern CEngine* Engine;

} //Namespace

#endif


