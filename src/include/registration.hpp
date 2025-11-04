#pragma once

#include <lua_bridge.hpp>

// This function needs to be exported.
// See luadef.hpp for details.
LUAAPI(int) LUA_MODULE_LUAOPEN(lua_State* L);
