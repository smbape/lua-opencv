#include "highgui_extra.h"
#include <registration.hpp>

namespace {
	using namespace LUA_MODULE_NAME;

	template<typename... Args>
	inline void check_error(sol::this_state& ts, const sol::function& fn, Args&&... args) {
		sol::state_view lua(ts);

		sol::protected_function_result result = fn(std::forward<Args>(args)...);

		if (!result.valid()) {
			sol::error err = result;

			sol::call_status status = result.status();
			LUA_MODULE_ERROR("Something went horribly wrong "
				"running the code: "
				<< sol::to_string(status) << " error"
				<< "\n\t" << err.what());

			luaL_error(lua.lua_state(), "callback");
		}
	}

	struct HighGui {
		HighGui(
			sol::state_view& lua,
			sol::function callback,
			sol::object userdata
		) {
			L = lua.lua_state();

			callback.push();
			callback_ref = luaL_ref(L, LUA_REGISTRYINDEX);

			if (userdata != sol::nil) {
				userdata.push();
				userdata_ref = luaL_ref(L, LUA_REGISTRYINDEX);
			}
		}

		sol::function callback() {
			lua_rawgeti(L, LUA_REGISTRYINDEX, callback_ref);
			sol::stack_aligned_unsafe_function stack_callback(L, -1);
			sol::function callback = stack_callback;
			lua_pop(L, 1);
			return callback;
		}

		sol::object userdata() {
			if (userdata_ref == LUA_REFNIL) {
				return sol::nil;
			}
			lua_rawgeti(L, LUA_REGISTRYINDEX, userdata_ref);
			sol::stack_object stack_obj(L, -1);
			sol::object obj = stack_obj;
			lua_pop(L, 1);
			return obj;
		}

		static std::map<size_t, HighGui> registered_workers;

		static HighGui& add_worker(sol::state_view& lua, sol::function callback, sol::object userdata) {
			auto key = registered_workers.size();
			{
				std::lock_guard<std::mutex> lock(callback_mutex);
				registered_workers.emplace(std::piecewise_construct,
					std::forward_as_tuple(key),
					std::forward_as_tuple(lua, callback, userdata));
			}
			return registered_workers.at(key);
		}

		static void registerTrackbarCallback(int pos, void* userdata) {
			std::lock_guard<std::mutex> lock(callback_mutex);
			auto worker = reinterpret_cast<HighGui*>(userdata);
			worker->has_data = true;
			worker->pos = pos;
		}

		static void onTrackbarNotify(void* userdata, sol::this_state& ts) {
			auto worker = reinterpret_cast<HighGui*>(userdata);
			if (worker->has_data) {
				worker->has_data = false;
				check_error(ts, worker->callback(), worker->pos, worker->userdata());
			}
		}

		static void registerButtonCallback(int state, void* userdata) {
			std::lock_guard<std::mutex> lock(callback_mutex);
			auto worker = reinterpret_cast<HighGui*>(userdata);
			worker->has_data = true;
			worker->state = state;
		}

		static void onButtonNotify(void* userdata, sol::this_state& ts) {
			auto worker = reinterpret_cast<HighGui*>(userdata);
			if (worker->has_data) {
				worker->has_data = false;
				check_error(ts, worker->callback(), worker->state, worker->userdata());
			}
		}

		static void registerMouseCallback(int event, int x, int y, int flags, void* userdata) {
			std::lock_guard<std::mutex> lock(callback_mutex);
			auto worker = reinterpret_cast<HighGui*>(userdata);
			worker->has_data = true;
			worker->event = event;
			worker->x = x;
			worker->y = y;
			worker->flags = flags;
		}

		static void onMouseNotify(void* userdata, sol::this_state& ts) {
			auto worker = reinterpret_cast<HighGui*>(userdata);
			if (worker->has_data) {
				worker->has_data = false;
				check_error(ts, worker->callback(), worker->event, worker->x, worker->y, worker->flags, worker->userdata());
			}
		}

		lua_State* L;
		int callback_ref = LUA_REFNIL;
		int userdata_ref = LUA_REFNIL;
		bool has_data = false;
		int pos;
		int state;
		int event;
		int x;
		int y;
		int flags;
	};

	std::map<size_t, HighGui> HighGui::registered_workers;
}

namespace cvextra {
	void setMouseCallback(
		sol::state_view& lua,
		const std::string& winname,
		sol::function onMouse,
		sol::object userdata
	) {
		auto& worker = HighGui::add_worker(lua, onMouse, userdata);
		registerCallback(&HighGui::onMouseNotify, &worker);
		cv::setMouseCallback(winname, &HighGui::registerMouseCallback, &worker);
	}

	int createButton(
		sol::state_view& lua,
		const std::string& bar_name,
		sol::function on_change,
		sol::object userdata,
		int type,
		bool initial_button_state
	) {
		auto& worker = HighGui::add_worker(lua, on_change, userdata);
		registerCallback(&HighGui::onButtonNotify, &worker);
		return cv::createButton(bar_name, &HighGui::registerButtonCallback, &worker, type, initial_button_state);
	}

	int createTrackbar(
		sol::state_view& lua,
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
		sol::state_view& lua,
		const std::string& trackbarname,
		const std::string& winname,
		int value,
		int count,
		sol::function onChange,
		sol::object userdata
	) {
		auto& worker = HighGui::add_worker(lua, onChange, userdata);
		registerCallback(&HighGui::onTrackbarNotify, &worker);
		int n = cv::createTrackbar(trackbarname, winname, nullptr, count, &HighGui::registerTrackbarCallback, &worker);
		cv::setTrackbarPos(trackbarname, winname, value);
		return n;
	}
}
