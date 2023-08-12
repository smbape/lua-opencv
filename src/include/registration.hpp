#pragma once

#include <lua_utils.hpp>

namespace LUA_MODULE_NAME {
    int deny_new_index(lua_State* L);
}

// This function needs to be exported.
// See luadef.hpp for details.
LUAAPI(int) LUA_MODULE_LUAOPEN(lua_State* L);
