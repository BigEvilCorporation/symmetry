#ifndef __SYM_SCRIPT
#define __SYM_SCRIPT

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

extern "C"
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "tolua++.h"

#include "lua_binding/lua_vector.h"
#include "lua_binding/lua_matrix.h"

namespace sym
{
	//Register C functions
	void RegisterCFunctions(lua_State *LuaState);

	/////////LUA registered functions

	//Engine
	static int shutdown(lua_State *LuaState);
	static int getscreenresolution(lua_State *LuaState);
	static int writelog(lua_State *LuaState);

	//Scripting
	static int loadscript(lua_State *LuaState);
	static int runscript(lua_State *LuaState);
	static int stopscript(lua_State *LuaState);

	//Input
	static int iskeydown(lua_State *LuaState);
	static int iskeydownascii(lua_State *LuaState);
	static int iskeypressed(lua_State *LuaState);
	static int iskeypressedascii(lua_State *LuaState);
	static int islmbdown(lua_State *LuaState);
	static int ismmbdown(lua_State *LuaState);
	static int isrmbdown(lua_State *LuaState);
	static int getmouseabsolute(lua_State *LuaState);
	static int getmouserelative(lua_State *LuaState);
	static int getmouseresolution(lua_State *LuaState);
	static int warpmouse(lua_State *LuaState);

	//Console
	static int consoleprint(lua_State *LuaState);
	static int loadconsoletexture(lua_State *LuaState);

	class CScript
	{
		public:
			//Constructor
			CScript();

			//Load script
			bool LoadScript(std::string Filename);

			//Close Lua state
			void Close();

			//Run script
			void Run();

			//Stop script
			void Stop();

			//Run script's Tick() function
			void Tick();

			//Is this script running
			bool IsRunning();

			//Empty argument list
            static std::vector<const void*> EmptyArgs;

			//Call a function within the script
			void CallFunction(std::string FunctionName, std::string ArgSignature = "", std::vector<const void*>& Args = EmptyArgs);

			//The script name
			std::string Name;

		private:
			//The Lua script state manager
			lua_State *LuaState;

			//Is script running
			bool Running;
	};

	class CScriptManager
	{
		public:
            CScriptManager();
            ~CScriptManager();

			//Init LUA and register all functions
			bool Init();

			//Shutdown LUA
			void Shutdown();

			//Run a single LUA string
			void RunCommand(std::string Command);

			//Load a script file
			bool LoadScript(std::string Name, std::string Filename);

			//Run script
			void RunScript(std::string Name);

			//Stop script
			void StopScript(std::string Name);

			//Delete script
			void DeleteScript(std::string Name);

			//Run Tick() on all scripts
			void TickScripts();

			//Empty argument list
            static std::vector<const void*> EmptyArgs;

			//Run a function inside all scripts
			void CallGlobalFunction(std::string FunctionName, std::string ArgSignature = "", std::vector<const void*>& Args = EmptyArgs);

			//Get the Lua state
			lua_State *GetLuaState() { return LuaState; }

			//Add function to be registered
			void AddRegisteredFunction(std::string Name, lua_CFunction Function);

		private:
			//Lua script state manager
			lua_State *LuaState;

			//Mapped container of scripts
			std::map<std::string, CScript*> Scripts;

			//Temp CScript for loading
			CScript *TempScript;

			//List of functions to be registered on init
			std::map<std::string, lua_CFunction> InitFunctions;
	};


} //Namespace

#endif
