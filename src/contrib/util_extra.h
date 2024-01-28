#pragma once
#include <lua_bridge.hpp>

// CV_EXPORTS_W : include this file in lua_generated_include

namespace cvextra {
	void redirectError(lua_State* L, const LUA_MODULE_NAME::Function& errCallback, const LUA_MODULE_NAME::Object& userdata = LUA_MODULE_NAME::lua_nil);
}
