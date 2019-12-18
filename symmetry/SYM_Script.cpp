#include <algorithm>

#include "SYM_Script.h"
#include "SYM_Maths.h"
#include "SYM_Engine.h"

#include <stdarg.h>

namespace sym
{
    void LuaError(lua_State *LuaState, int Status)
    {
        if (Status != 0)
        {
            Engine->Logger.AddEntry("Lua error: " + (std::string)lua_tostring(LuaState, -1));

            //Remove entry
            lua_pop(LuaState, 1);
        }
    }

    std::vector<const void*> CScriptManager::EmptyArgs;
    std::vector<const void*> CScript::EmptyArgs;

	CScript::CScript()
	{
	    LuaState = 0;
		Running = false;
	}

	bool CScript::LoadScript(std::string Filename)
	{
		//Fire up Lua
		LuaState = lua_open();

		//Check for errors
		if(!LuaState) return false;

		//Load Lua libraries
		luaL_openlibs(LuaState);

		//Register C functions
		RegisterCFunctions(LuaState);

		//Convert filename to upper case
		//transform(Filename.begin(), Filename.end(), Filename.begin(), toupper);

		//Prefix filename with path from engine config
		Filename = Engine->ScriptsPath + Filename;

		//Load the script
		int Error = luaL_dofile(LuaState, Filename.c_str());

		//Check for errors
        LuaError(LuaState, Error);
        if (Error != 0) return false;

		return true;
 	}

	void CScript::Close()
	{
		lua_close(LuaState);
	}

	bool CScript::IsRunning()
	{
		return Running;
	}

	void CScript::Run()
	{
		//Call the script's OnRun() function
		CallFunction("OnRun");

		Running = true;
	}

	void CScript::Stop()
	{
		Running = false;
	}

	void CScript::CallFunction(std::string FunctionName, std::string ArgSignature, std::vector<const void*>& Args)
	{
	    //Check Lua function exists
	    lua_pushstring(LuaState, FunctionName.c_str());
        lua_rawget(LuaState, LUA_GLOBALSINDEX);
        bool Exists = lua_isfunction(LuaState, -1);
        lua_pop(LuaState, 1);

        if(Exists)
        {
            //Check ArgSignature matches number of args
            if(ArgSignature.size() != Args.size())
            {
                Engine->Logger.AddEntry("Error: CallFunction called with mismatched argument signature: " + FunctionName);
                return;
            }

            //Get function
            lua_getglobal(LuaState, FunctionName.c_str());

            //Loop through signature
            for(int i = 0; i < (int)ArgSignature.size(); i++)
            {
                //Check type
                switch(ArgSignature[i])
                {
                    case 'i':
                    {
                        lua_pushnumber(LuaState, (int)Args[i]);
                        break;
                    }

                    case 'f':
                    {
                        lua_pushnumber(LuaState, *(float*)Args[i]);
                        break;
                    }

                    case 's':
                    {
                        lua_pushstring(LuaState, (char *)Args[i]);
                        break;
                    }
                }
            }

            //Do the call
            int Error = lua_pcall(LuaState, ArgSignature.size(), LUA_MULTRET, 0);

            //Perform garbage collection
            lua_gc(LuaState, LUA_GCCOLLECT, 0);

            //Check for errors
            LuaError(LuaState, Error);
        }
	}

	void CScript::Tick()
	{
		//Run the script's Tick(), pass in delta time
		std::vector<const void*> Args;
		float Delta = Engine->GetDeltaTime();
	    Args.push_back(&Delta);
		CallFunction("OnTick", "f", Args);
	}

	CScriptManager::CScriptManager()
	{
	}

    CScriptManager::~CScriptManager()
    {
        for(std::map<std::string, CScript*>::iterator i = Scripts.begin(); i != Scripts.end(); i++)
		{
		    delete (*i).second;
		}
    }

	bool CScriptManager::Init()
	{
		//Fire up Lua
		LuaState = 0;
		LuaState = lua_open();

		//Check for errors
		if(!LuaState) return false;

		//Load Lua libraries
		luaL_openlibs(LuaState);

		//Register C functions
		RegisterCFunctions(LuaState);

		//Register function in init stack
		for(std::map<std::string, lua_CFunction>::iterator i = InitFunctions.begin(); i != InitFunctions.end(); i++)
		{
			lua_register(LuaState, (*i).first.c_str(), (*i).second);
		}

		//Clear init stack
		InitFunctions.clear();

		return true;
	}

	void CScriptManager::Shutdown()
	{
		//Close all Lua states
		for(std::map<std::string, CScript*>::iterator i = Scripts.begin(); i != Scripts.end(); i++)
		{
			(*i).second->Stop();
			(*i).second->Close();
		}

		//Shutdown the Lua state
		lua_close(LuaState);
	}

	void CScriptManager::AddRegisteredFunction(std::string Name, lua_CFunction Function)
	{
		InitFunctions[Name] = Function;
	}

	void CScriptManager::RunCommand(std::string Command)
	{
		//Push string
		luaL_dostring(LuaState, Command.c_str());

		//Run command
		lua_pcall(LuaState, 0, LUA_MULTRET, 0);
	}

	bool CScriptManager::LoadScript(std::string Name, std::string Filename)
	{
	    if(Scripts.find(Name) != Scripts.end())
		{
		    Engine->Logger.AddEntry("Error: Script with name " + Name + " already exists");
		    return false;
		}
		else
		{
            //New temp CScript
            TempScript = new CScript;

            //Set name
            TempScript->Name = Name;

            //Load script
            if (!TempScript->LoadScript(Filename))
            {
                Engine->Logger.AddEntry("Error: Script not found, or script parsing error: " + Filename);
                Engine->Console.PrintLine("> Error: Script not found, or script parsing error");
                delete TempScript;
                return false;
            }

            //Push back to the map
            Scripts[Name] = TempScript;

            //Run the script's OnLoad function, pass in script name
            std::vector<const void*> Args;
            Args.push_back(Name.c_str());
            Scripts[Name]->CallFunction("OnLoad", "s", Args);
		}

		return true;
	}

	void CScriptManager::RunScript(std::string Name)
	{
	    if(Scripts.find(Name) != Scripts.end())
		{
            Scripts[Name]->Run();
		}
		else
		{
		    Engine->Logger.AddEntry("Error: Script " + Name + " does not exist - RunScript called");
		}
	}

	void CScriptManager::StopScript(std::string Name)
	{
	    if(Scripts.find(Name) != Scripts.end())
		{
            Scripts[Name]->Stop();
        }
		else
		{
		    Engine->Logger.AddEntry("Error: Script " + Name + " does not exist - StopScript called");
		}
	}

	void CScriptManager::TickScripts()
	{
		//Tick all running scripts
		for(std::map<std::string, CScript*>::iterator i = Scripts.begin(); i != Scripts.end(); i++)
		{
			if((*i).second->IsRunning())
			{
				(*i).second->Tick();
			}
		}
	}

	void CScriptManager::CallGlobalFunction(std::string FunctionName, std::string ArgSignature, std::vector<const void*>& Args)
	{
	    //Run function on all running scripts
		for(std::map<std::string, CScript*>::iterator i = Scripts.begin(); i != Scripts.end(); i++)
		{
			if((*i).second->IsRunning())
			{
				(*i).second->CallFunction(FunctionName, ArgSignature, Args);
			}
		}
	}

	/////////Lua registered functions
	void RegisterCFunctions(lua_State *LuaState)
	{
	    tolua_open(LuaState);
        tolua_sym_vector_open(LuaState);
        tolua_sym_matrix_open(LuaState);

	    //Engine
	    lua_register(LuaState, "shutdown", shutdown);
		lua_register(LuaState, "writelog", writelog);
	    lua_register(LuaState, "getscreenresolution", getscreenresolution);

		//Scripting
		lua_register(LuaState, "loadscript", loadscript);
		lua_register(LuaState, "runscript", runscript);
		lua_register(LuaState, "stopscript", stopscript);

		//Input
		lua_register(LuaState, "iskeydown", iskeydown);
		lua_register(LuaState, "iskeydownascii", iskeydownascii);
		lua_register(LuaState, "iskeypressed", iskeypressed);
		lua_register(LuaState, "iskeypressedascii", iskeypressedascii);
		lua_register(LuaState, "islmbdown", islmbdown);
		lua_register(LuaState, "ismmbdown", ismmbdown);
		lua_register(LuaState, "isrmbdown", isrmbdown);
		lua_register(LuaState, "getmouseabsolute", getmouseabsolute);
		lua_register(LuaState, "getmouserelative", getmouserelative);
		lua_register(LuaState, "getmouseresolution", getmouseresolution);
		lua_register(LuaState, "warpmouse", warpmouse);

		//Console
		lua_register(LuaState, "consoleprint", consoleprint);
		lua_register(LuaState, "loadconsoletexture", loadconsoletexture);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////
	// ENGINE
	////////////////////////////////////////////////////////////////////////////////////////////////

	static int shutdown(lua_State *LuaState)
	{
		if(lua_gettop(LuaState) == 0)
		{
			Engine->ShutDown();
		}
		else
		{
		    Engine->Logger.AddEntry("Error: 'shutdown' takes no arguments");
			Engine->Console.PrintLine("> Error: 'shutdown' takes no arguments");
		}

		return 0;
	}

	static int getscreenresolution(lua_State *LuaState)
	{
		if(lua_gettop(LuaState) == 0)
		{
			float ResX = (float)Engine->ScreenWidth;
			float ResY = (float)Engine->ScreenHeight;
			lua_pushnumber(LuaState, ResX);
			lua_pushnumber(LuaState, ResY);
		}
		else
		{
		    Engine->Logger.AddEntry("Error: 'getscreenresolution' takes no arguments");
			Engine->Console.PrintLine("> Error: 'getscreenresolution' takes no arguments");
		}

		return 2;
	}

	static int writelog(lua_State *LuaState)
	{
		if(lua_gettop(LuaState) == 1)
		{
			Engine->Logger.AddEntry(lua_tostring(LuaState, 1));
		}
		else
		{
		    Engine->Logger.AddEntry("Error: 'writelog' takes 1 argument");
			Engine->Console.PrintLine("> Error: 'writelog' takes 1 argument");
		}

		return 0;
	}

    ////////////////////////////////////////////////////////////////////////////////////////////////
	// SCRIPTING
	////////////////////////////////////////////////////////////////////////////////////////////////
	static int loadscript(lua_State *LuaState)
	{
		if(lua_gettop(LuaState) == 2)
		{
			Engine->ScriptManager.LoadScript(lua_tostring(LuaState, 1), lua_tostring(LuaState, 2));
			bool DrawDebugInfo = (bool)lua_tonumber(LuaState, 3);
		}
		else
		{
		    Engine->Logger.AddEntry("Error: 'loadscript' takes 2 arguments");
			Engine->Console.PrintLine("> Error: 'loadscript' takes 2 arguments");
		}

		return 0;
	}

	static int runscript(lua_State *LuaState)
	{
		if(lua_gettop(LuaState) == 1)
		{
			Engine->ScriptManager.RunScript(lua_tostring(LuaState, 1));
		}
		else
		{
		    Engine->Logger.AddEntry("Error: 'runscript' takes 1 argument");
			Engine->Console.PrintLine("> Error: 'runscript' takes 1 argument");
		}

		return 0;
	}

	static int stopscript(lua_State *LuaState)
	{
		if(lua_gettop(LuaState) == 1)
		{
			Engine->ScriptManager.StopScript(lua_tostring(LuaState, 1));
		}
		else
		{
		    Engine->Logger.AddEntry("Error: 'stopscript' takes 1 argument");
			Engine->Console.PrintLine("> Error: 'stopscript' takes 1 argument");
		}

		return 0;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////
	// INPUT
	////////////////////////////////////////////////////////////////////////////////////////////////

	static int iskeydown(lua_State *LuaState)
	{
	    int Result = 0;

		if(lua_gettop(LuaState) == 1)
		{
			Result = (int)Engine->Input.IsKeyDown((int)lua_tostring(LuaState, 1)[0]);
			lua_pushnumber(LuaState, Result);
		}
		else
		{
		    Engine->Logger.AddEntry("Error: 'iskeydown' takes 1 argument");
			Engine->Console.PrintLine("> Error: 'iskeydown' takes 1 argument");
		}

		return 1;
	}

	static int iskeydownascii(lua_State *LuaState)
	{
	    int Result = 0;

		if(lua_gettop(LuaState) == 1)
		{
			Result = (int)Engine->Input.IsKeyDown(lua_tonumber(LuaState, 1));
			lua_pushnumber(LuaState, Result);
		}
		else
		{
		    Engine->Logger.AddEntry("Error: 'iskeydownascii' takes 1 argument");
			Engine->Console.PrintLine("> Error: 'iskeydownascii' takes 1 argument");
		}

		return 1;
	}

	static int iskeypressed(lua_State *LuaState)
	{
	    int Result = 0;

		if(lua_gettop(LuaState) == 1)
		{
			Result = (int)Engine->Input.IsKeyPressed((int)lua_tostring(LuaState, 1)[0]);
			lua_pushnumber(LuaState, Result);
		}
		else
		{
		    Engine->Logger.AddEntry("Error: 'iskeypressed' takes 1 argument");
			Engine->Console.PrintLine("> Error: 'iskeypressed' takes 1 argument");
		}

		return 1;
	}

	static int iskeypressedascii(lua_State *LuaState)
	{
	    int Result = 0;

		if(lua_gettop(LuaState) == 1)
		{
			Result = (int)Engine->Input.IsKeyPressed(lua_tonumber(LuaState, 1));
			lua_pushnumber(LuaState, Result);
		}
		else
		{
		    Engine->Logger.AddEntry("Error: 'iskeypressedascii' takes 1 argument");
			Engine->Console.PrintLine("> Error: 'iskeypressedascii' takes 1 argument");
		}

		return 1;
	}

	static int islmbdown(lua_State *LuaState)
	{
	    int Result = 0;

		if(lua_gettop(LuaState) == 0)
		{
			Result = (int)Engine->Input.IsLMBDown();
			lua_pushnumber(LuaState, Result);
		}
		else
		{
		    Engine->Logger.AddEntry("Error: 'islmbdown' takes no arguments");
			Engine->Console.PrintLine("> Error: 'islmbdown' takes no arguments");
		}

		return 1;
	}

	static int ismmbdown(lua_State *LuaState)
	{
	    int Result = 0;

		if(lua_gettop(LuaState) == 0)
		{
			Result = (int)Engine->Input.IsMMBDown();
			lua_pushnumber(LuaState, Result);
		}
		else
		{
		    Engine->Logger.AddEntry("Error: 'ismmbdown' takes no arguments");
			Engine->Console.PrintLine("> Error: 'ismmbdown' takes no arguments");
		}

		return 1;
	}

	static int isrmbdown(lua_State *LuaState)
	{
	    int Result = 0;

		if(lua_gettop(LuaState) == 0)
		{
			Result = (int)Engine->Input.IsRMBDown();
			lua_pushnumber(LuaState, Result);
		}
		else
		{
		    Engine->Logger.AddEntry("Error: 'isrmbdown' takes no arguments");
			Engine->Console.PrintLine("> Error: 'isrmbdown' takes no arguments");
		}

		return 1;
	}

	static int getmouseabsolute(lua_State *LuaState)
	{
		if(lua_gettop(LuaState) == 0)
		{
			float MouseX = (float)Engine->Input.GetMouseAbsoluteX();
			float MouseY = (float)Engine->Input.GetMouseAbsoluteY();
			lua_pushnumber(LuaState, MouseX);
			lua_pushnumber(LuaState, MouseY);
		}
		else
		{
		    Engine->Logger.AddEntry("Error: 'getmouseabsolute' takes no arguments");
			Engine->Console.PrintLine("> Error: 'getmouseabsolute' takes no arguments");
		}

		return 2;
	}

	static int getmouserelative(lua_State *LuaState)
	{
		if(lua_gettop(LuaState) == 0)
		{
			float MouseX = (float)Engine->Input.GetMouseRelativeX();
			float MouseY = (float)Engine->Input.GetMouseRelativeY();
			lua_pushnumber(LuaState, MouseX);
			lua_pushnumber(LuaState, MouseY);
		}
		else
		{
		    Engine->Logger.AddEntry("Error: 'getmouserelative' takes no arguments");
			Engine->Console.PrintLine("> Error: 'getmouserelative' takes no arguments");
		}

		return 2;
	}

	static int getmouseresolution(lua_State *LuaState)
	{
		if(lua_gettop(LuaState) == 0)
		{
			float Res = Engine->Input.GetMouseResolution();
			lua_pushnumber(LuaState, Res);
		}
		else
		{
		    Engine->Logger.AddEntry("Error: 'getmouseresolution' takes no arguments");
			Engine->Console.PrintLine("> Error: 'getmouseresolution' takes no arguments");
		}

		return 1;
	}

	static int warpmouse(lua_State *LuaState)
	{
		if(lua_gettop(LuaState) == 2)
		{
			Engine->Input.WarpMouse(lua_tonumber(LuaState, 1), lua_tonumber(LuaState, 2));
		}
		else
		{
		    Engine->Logger.AddEntry("Error: 'warpmouse' takes 2 arguments");
			Engine->Console.PrintLine("> Error: 'warpmouse' takes 2 arguments");
		}

		return 0;
	}

    ////////////////////////////////////////////////////////////////////////////////////////////////
	// CONSOLE
	////////////////////////////////////////////////////////////////////////////////////////////////
	static int consoleprint(lua_State *LuaState)
	{
		if(lua_gettop(LuaState) == 1)
		{
			Engine->Console.PrintLine(lua_tostring(LuaState, 1));
		}
		else
		{
		    Engine->Logger.AddEntry("Error: 'consoleprint' takes 1 argument");
			Engine->Console.PrintLine("> Error: 'consoleprint' takes 1 argument");
		}

		return 0;
	}

	static int loadconsoletexture(lua_State *LuaState)
	{
		if(lua_gettop(LuaState) == 1)
		{
			Engine->Console.SetBackgroundTexture(lua_tostring(LuaState, 1));
		}
		else
		{
		    Engine->Logger.AddEntry("Error: 'loadconsoletexture' takes 1 argument");
			Engine->Console.PrintLine("> Error: 'loadconsoletexture' takes 1 argument");
		}

		return 0;
	}

} //Namespace
