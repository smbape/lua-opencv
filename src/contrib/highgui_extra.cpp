#include "highgui_extra.h"
#include <registration.hpp>

namespace {
	using namespace LUA_MODULE_NAME;

	template<typename... Args>
	inline void check_error(sol::this_state& ts, sol::safe_function& fn, Args&&... args) {
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
			sol::safe_function callback,
			sol::object userdata
		) :
			callback(callback),
			userdata(userdata),
			has_data(false) {}

		static std::map<size_t, HighGui> registered_workers;

		static HighGui& add_worker(sol::safe_function callback, sol::object userdata) {
			auto key = registered_workers.size();
			{
				std::lock_guard<std::mutex> lock(callback_mutex);
				registered_workers.emplace(std::piecewise_construct,
					std::forward_as_tuple(key),
					std::forward_as_tuple(callback, userdata));
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
				check_error(ts, worker->callback, worker->pos, worker->userdata);
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
				check_error(ts, worker->callback, worker->state, worker->userdata);
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
				check_error(ts, worker->callback, worker->event, worker->x, worker->y, worker->flags, worker->userdata);
			}
		}

		sol::safe_function callback;
		sol::object userdata;
		bool has_data;
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
		const std::string& winname,
		sol::safe_function onMouse,
		sol::object userdata
	) {
		auto& worker = HighGui::add_worker(onMouse, userdata);
		registerCallback(&HighGui::onMouseNotify, &worker);
		cv::setMouseCallback(winname, &HighGui::registerMouseCallback, &worker);
	}

	int createButton(
		const std::string& bar_name,
		sol::safe_function on_change,
		sol::object userdata,
		int type,
		bool initial_button_state
	) {
		auto& worker = HighGui::add_worker(on_change, userdata);
		registerCallback(&HighGui::onButtonNotify, &worker);
		return cv::createButton(bar_name, &HighGui::registerButtonCallback, &worker, type, initial_button_state);
	}

	int createTrackbar(
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
		const std::string& trackbarname,
		const std::string& winname,
		int value,
		int count,
		sol::safe_function onChange,
		sol::object userdata
	) {
		auto& worker = HighGui::add_worker(onChange, userdata);
		registerCallback(&HighGui::onTrackbarNotify, &worker);
		int n = cv::createTrackbar(trackbarname, winname, nullptr, count, &HighGui::registerTrackbarCallback, &worker);
		cv::setTrackbarPos(trackbarname, winname, value);
		return n;
	}
}
