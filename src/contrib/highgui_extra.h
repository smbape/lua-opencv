#pragma once
#include <lua_bridge.hpp>

// CV_EXPORTS_W : include this file in lua_generated_include

namespace cvextra {
	void setMouseCallback(
		lua_State* L,
		const std::string& winname,
		const LUA_MODULE_NAME::Function& onMouse,
		const LUA_MODULE_NAME::Object& userdata
	);

	int createButton(
		lua_State* L,
		const std::string& bar_name,
		const LUA_MODULE_NAME::Function& on_change,
		const LUA_MODULE_NAME::Object& userdata,
		int type,
		bool initial_button_state
	);

	int createTrackbar(
		lua_State* L,
		const std::string& trackbarname,
		const std::string& winname,
		int value,
		int count
	);

	int createTrackbar(
		lua_State* L,
		const std::string& trackbarname,
		const std::string& winname,
		int value,
		int count,
		const LUA_MODULE_NAME::Function& onChange,
		const LUA_MODULE_NAME::Object& userdata
	);
}
