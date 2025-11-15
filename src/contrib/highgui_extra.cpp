#include "highgui_extra.h"
#include "callback_function.impl.hpp"

using namespace LUA_MODULE_NAME;

namespace cvextra {
	void setMouseCallback(
		lua_State* L,
		const std::string& winname,
		const Function& onMouse,
		const Object& userdata
	) {
		auto invoker = lua_tocallback(L, onMouse, userdata, static_cast<callback_function_t<cv::MouseCallback>*>(nullptr));
		cv::setMouseCallback(winname, invoker.callback, invoker.userdata);
	}

	int createButton(
		lua_State* L,
		const std::string& bar_name,
		const Function& on_change,
		const Object& userdata,
		int type,
		bool initial_button_state
	) {
		auto invoker = lua_tocallback(L, on_change, userdata, static_cast<callback_function_t<cv::ButtonCallback>*>(nullptr));
		return cv::createButton(bar_name, invoker.callback, invoker.userdata, type, initial_button_state);
	}

	int createTrackbar(
		lua_State* L,
		const std::string& trackbarname,
		const std::string& winname,
		int value,
		int count
	) {
		int n = cv::createTrackbar(trackbarname, winname, nullptr, count);
		cv::setTrackbarPos(trackbarname, winname, value);
		return n;
	}

	int createTrackbar(
		lua_State* L,
		const std::string& trackbarname,
		const std::string& winname,
		int value,
		int count,
		const Function& onChange,
		const Object& userdata
	) {
		auto info = lua_tocallback(L, onChange, userdata, static_cast<callback_function_t<cv::TrackbarCallback>*>(nullptr));

		// Calling setTrackbarPos causes onChange to be called.
		// However, during initialization, onChange may refer to objects not yet created, causing exceptions.
		// Therefore, we skip the initial call on onChange to only call it when there is a change.
		// Prevent the callback to be called
		info.invoker->active = false;

		int n = cv::createTrackbar(trackbarname, winname, nullptr, count, info.callback, info.userdata);
		cv::setTrackbarPos(trackbarname, winname, value);

		// The callback can now be called
		info.invoker->active = true;
		return n;
	}
}
