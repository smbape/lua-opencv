#pragma once

#include <lua_bridge.hpp>
#include <shared_mutex>

namespace LUA_MODULE_NAME {
	int __call_constructor(lua_State* L);

	std::unique_lock<std::shared_timed_mutex> lock_callbacks();
}

// This function needs to be exported.
// See luadef.hpp for details.
LUAAPI(int) LUA_MODULE_LUAOPEN(lua_State* L);
