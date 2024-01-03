#pragma once
#include <luadef.hpp>
#include <opencv2/highgui.hpp>

// CV_EXPORTS_W : include this file in lua_generated_include

namespace cvextra {
	void setMouseCallback(
		const std::string& winname,
		sol::function onMouse,
		sol::object userdata
	);

	int createButton(
		const std::string& bar_name,
		sol::function on_change,
		sol::object userdata,
		int type,
		bool initial_button_state
	);

	int createTrackbar(
		const std::string& trackbarname,
		const std::string& winname,
		int value,
		int count
	);

	int createTrackbar(
		const std::string& trackbarname,
		const std::string& winname,
		int value,
		int count,
		sol::function onChange,
		sol::object userdata
	);
}
