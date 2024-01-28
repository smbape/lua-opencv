#pragma once

#include <lua_bridge.hpp>

namespace LUA_MODULE_NAME {
	int __call_constructor(lua_State* L);

	bool is_call_garbage_collect();

	using Callback = std::function<void(lua_State*, void*)>;
	extern std::mutex callback_mutex;
	void registerCallback(Callback callback, void* userdata);
}

// This function needs to be exported.
// See luadef.hpp for details.
LUAAPI(int) LUA_MODULE_LUAOPEN(lua_State* L);
