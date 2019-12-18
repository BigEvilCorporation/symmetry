#include "SYM_Core.h"

int LuaSpawn(lua_State *LuaState);
int LuaDrawCollision(lua_State *LuaState);
int LuaDebug(lua_State *LuaState);
int LuaLoadLevel(lua_State *LuaState);
int LuaSetPlayerPosition(lua_State *LuaState);
int LuaSetEnhancedVision(lua_State *LuaState);

void RegisterFunctions();