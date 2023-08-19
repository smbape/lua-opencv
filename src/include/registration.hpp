#pragma once

#include <lua_utils.hpp>

namespace LUA_MODULE_NAME {
	bool is_call_garbage_collect();
	void call_garbage_collect(bool value = true);
	int deny_new_index(lua_State* L);

	using Callback = std::function<void(void*, sol::this_state&)>;
	extern std::mutex callback_mutex;
	void registerCallback(Callback callback, void* userdata);
	void notifyCallbacks(sol::this_state ts);
}

// This function needs to be exported.
// See luadef.hpp for details.
LUAAPI(int) LUA_MODULE_LUAOPEN(lua_State* L);
