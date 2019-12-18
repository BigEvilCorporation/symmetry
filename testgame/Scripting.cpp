#include "Scripting.h"

#include "Playground.h"
#include "Hacking.h"
#include "Game.h"

#include "WeaponRocketTurret.h"
#include "WeaponTurretLauncher.h"
#include "WeaponMiniTurret.h"
#include "ProjectileBase.h"

#include "PickupGrappleGun.h"
#include "PickupRocketLauncher.h"
#include "PickupRocketTurretRemote.h"

#include "VideoBoard.h"

void RegisterFunctions()
{
	sym::Engine->ScriptManager.AddRegisteredFunction("debug", LuaDebug);
	sym::Engine->ScriptManager.AddRegisteredFunction("drawcollision", LuaDrawCollision);
	sym::Engine->ScriptManager.AddRegisteredFunction("loadplayground", LuaLoadPlayground);
	sym::Engine->ScriptManager.AddRegisteredFunction("loadlevel", LuaLoadLevel);
	sym::Engine->ScriptManager.AddRegisteredFunction("loadhacklevel", LuaLoadHackLevel);
	sym::Engine->ScriptManager.AddRegisteredFunction("spawn", LuaSpawn);
	sym::Engine->ScriptManager.AddRegisteredFunction("setplayerposition", LuaSetPlayerPosition);
	sym::Engine->ScriptManager.AddRegisteredFunction("setenhancedvision", LuaSetEnhancedVision);
}

int LuaLoadLevel(lua_State *LuaState)
{
	if(lua_gettop(LuaState) == 1)
	{
		CLevelState *State = new CLevelState();

		Game->ChangeLevel(State);

		State->LoadScene(lua_tostring(sym::Engine->ScriptManager.GetLuaState(), 1));
	}
	else
	{
	    Game->Log.AddEntry("Error: 'loadlevel' takes 1 argument");
		sym::Engine->Console.PrintLine("> Error: 'loadlevel' takes 1 argument");
		sym::Engine->Console.PrintLine("> loadlevel(string Filename)");
	}

	return 0;
}

int LuaSetPlayerPosition(lua_State *LuaState)
{
	if(lua_gettop(LuaState) == 3)
	{
		CLevelState *State = Game->GetCurrentLevel();

		if(!State)
		{
			Game->Log.AddEntry("Error: 'setplayerposition' cannot be called with no level loaded");
			sym::Engine->Console.PrintLine("> Error: 'setplayerposition' cannot be called with no level loaded");
		}
		else
		{
			sym::CVector3 Position(	(float)lua_tonumber(sym::Engine->ScriptManager.GetLuaState(), 1), 
									(float)lua_tonumber(sym::Engine->ScriptManager.GetLuaState(), 2),
									(float)lua_tonumber(sym::Engine->ScriptManager.GetLuaState(), 3));

			State->GetPlayer()->SetPosition(Position);
		}

	}
	else
	{
	    Game->Log.AddEntry("Error: 'setplayerposition' takes 3 arguments");
		sym::Engine->Console.PrintLine("> Error: 'setplayerposition' takes 3 arguments");
		sym::Engine->Console.PrintLine("> setplayerposition(float X, float Y, float Z)");
	}

	return 0;
}

int LuaSetEnhancedVision(lua_State *LuaState)
{
	if(lua_gettop(LuaState) == 1)
	{
		CLevelState *State = Game->GetCurrentLevel();

		if(!State)
		{
			Game->Log.AddEntry("Error: 'setenhancedvision' cannot be called with no level loaded");
			sym::Engine->Console.PrintLine("> Error: 'setenhancedvision' cannot be called with no level loaded");
		}
		else
		{
			State->SetVisionMode(lua_toboolean(sym::Engine->ScriptManager.GetLuaState(), 1));
		}
	}
	else
	{
	    Game->Log.AddEntry("Error: 'setenhancedvision' takes 1 argument");
		sym::Engine->Console.PrintLine("> Error: 'setenhancedvision' takes 1 argument");
		sym::Engine->Console.PrintLine("> setenhancedvision(boolean)");
	}

	return 0;
}

int LuaDebug(lua_State *LuaState)
{
	if(lua_gettop(LuaState) == 1)
	{
		Game->SetDebugMode((bool)lua_toboolean(sym::Engine->ScriptManager.GetLuaState(), 1));
	}
	else
	{
	    Game->Log.AddEntry("Error: 'debug' takes 1 argument");
		sym::Engine->Console.PrintLine("> Error: 'debug' takes 1 argument");
		sym::Engine->Console.PrintLine("> debug(boolean)");
	}

	return 0;
}

int LuaDrawCollision(lua_State *LuaState)
{
	if(lua_gettop(LuaState) == 1)
	{
		CLevelState *Level = Game->GetCurrentLevel();

		if(Level)
		{
			bool Value = (bool)lua_toboolean(sym::Engine->ScriptManager.GetLuaState(), 1);
			Level->GetScene()->ShowCollision(Value);
		}
		else
		{
			Game->Log.AddEntry("Error: Cannot change scene settings, no level loaded");
			sym::Engine->Console.PrintLine("> Error: Cannot change scene settings, no level loaded");
		}
	}
	else
	{
	    Game->Log.AddEntry("Error: 'drawcollision' takes 1 argument");
		sym::Engine->Console.PrintLine("> Error: 'drawcollision' takes 1 argument");
		sym::Engine->Console.PrintLine("> drawcollision(boolean)");
	}

	return 0;
}

int LuaSpawn(lua_State *LuaState)
{
	if(lua_gettop(LuaState) == 1)
	{
		CLevelState *Level = Game->GetCurrentLevel();
		CCharacterPlayer *Player = Level->GetPlayer();

		if(Level)
		{
			std::string Name = lua_tostring(sym::Engine->ScriptManager.GetLuaState(), 1);

			if(Name == "turretlauncher") Level->Spawn<CTurretLauncher>(Player->GetPosition() + (Player->GetForward() * 50), Player->GetRotation());
			else if(Name == "miniturret") Level->Spawn<CMiniTurret>(Player->GetPosition() + (Player->GetForward() * 50), Player->GetRotation());
			else if(Name == "rocketturret") Level->Spawn<CRocketTurret>(Player->GetPosition() + (Player->GetForward() * 100), Player->GetRotation());
			else if(Name == "remotepickup") Level->Spawn<CPickupRocketTurretRemote>(Player->GetPosition() + (Player->GetForward() * 50), Player->GetRotation());
			else if(Name == "grapplepickup") Level->Spawn<CPickupGrappleGun>(Player->GetPosition() + (Player->GetForward() * 50), Player->GetRotation());
			else if(Name == "rocketlauncher") Level->Spawn<CPickupRocketLauncher>(Player->GetPosition() + (Player->GetForward() * 50), Player->GetRotation());
			else if(Name == "projectile") Level->Spawn<CProjectileBase>(Player->GetPosition() + (Player->GetForward() * 50), Player->GetRotation());
			else if(Name == "videoboard") Level->Spawn<CVideoBoard>(Player->GetPosition() + (Player->GetForward() * 100), Player->GetRotation());
			else if(Name == "crate")
			{
				sym::CBrush *Crate = Level->GetScene()->AddBox("", sym::PHYSICS_OBJ_BOX, sym::CVector3(30.0f, 30.0f, 30.0f));
				Crate->SetPosition(Level->GetPlayer()->GetPosition() + (Level->GetPlayer()->GetForward() * 50));
				Crate->SetRotation(Player->GetRotation());
				Crate->SetMass(20.0f);
				Crate->SetMaterial("crate.sym.material");
			}
			else
			{
				Game->Log.AddEntry("Error: Cannot spawn " + Name + ", no object by that name");
				sym::Engine->Console.PrintLine("> Error: Cannot spawn " + Name + ", no object by that name");
			}
		}
		else
		{
			Game->Log.AddEntry("Error: Cannot spawn anything, no level loaded");
			sym::Engine->Console.PrintLine("> Error: Cannot spawn anything, no level loaded");
		}
	}
	else
	{
	    Game->Log.AddEntry("Error: 'spawn' takes 1 argument");
		sym::Engine->Console.PrintLine("> Error: 'spawn' takes 1 argument");
		sym::Engine->Console.PrintLine("> spawn(ObjectName)");
	}

	return 0;
}