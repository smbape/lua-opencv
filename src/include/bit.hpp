#pragma once

#include <lua.h>
#include <lauxlib.h>

#ifndef LUA_MODULE_NAME
#error "LUA_MODULE_NAME must be defined"
#endif

namespace LUA_MODULE_NAME {
    int luaopen_bit(lua_State *L);
}
