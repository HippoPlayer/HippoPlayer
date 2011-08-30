#ifndef HIPPOLUA_H
#define HIPPOLUA_H

struct lua_State;
struct lua_State* g_luaState;

void HippoLua_updateScript();
void HippoLua_registerLuaFunctions(struct lua_State* luaState);

#endif
