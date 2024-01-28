#include "highgui_extra.h"
#include <registration.hpp>

using namespace LUA_MODULE_NAME;

namespace {
	template<typename... Args>
	inline void check_error(lua_State* L, const Function& fn, Args&&... args) {
		lua_push(L, fn);

		// https://stackoverflow.com/questions/7230621/how-can-i-iterate-over-a-packed-variadic-template-argument-list/60136761#60136761
		int nargs = 0;
		([&] {
			lua_push(L, args);
			nargs++;
			} (), ...);
		lua_call(L, nargs, 0);
	}

	struct HighGui {
		HighGui(
			lua_State* L,
			const Function& callback,
			const Object& userdata
		) {
			this->callback.assign(L, callback);
			this->userdata.assign(L, userdata);
		}

		~HighGui() {
			// callback and userdata lua_State are invalid at this point
			// because lua_State closes before the workers are destroyed on exit
			// therefore, dereference lua_State
			callback.free();
			userdata.free();
		}

		static std::map<size_t, HighGui> registered_workers;

		static HighGui& add_worker(lua_State* L, const Function& callback, const Object& userdata) {
			auto key = registered_workers.size();
			{
				std::lock_guard<std::mutex> lock(callback_mutex);
				registered_workers.emplace(std::piecewise_construct,
					std::forward_as_tuple(key),
					std::forward_as_tuple(L, callback, userdata));
			}
			return registered_workers.at(key);
		}

		static void registerTrackbarCallback(int pos, void* userdata) {
			std::lock_guard<std::mutex> lock(callback_mutex);
			auto worker = reinterpret_cast<HighGui*>(userdata);
			worker->has_data = true;
			worker->pos = pos;
		}

		static void onTrackbarNotify(lua_State* L, void* userdata) {
			auto worker = reinterpret_cast<HighGui*>(userdata);
			if (worker->has_data) {
				worker->has_data = false;
				check_error(L, worker->callback, worker->pos, worker->userdata);
			}
		}

		static void registerButtonCallback(int state, void* userdata) {
			std::lock_guard<std::mutex> lock(callback_mutex);
			auto worker = reinterpret_cast<HighGui*>(userdata);
			worker->has_data = true;
			worker->state = state;
		}

		static void onButtonNotify(lua_State* L, void* userdata) {
			auto worker = reinterpret_cast<HighGui*>(userdata);
			if (worker->has_data) {
				worker->has_data = false;
				check_error(L, worker->callback, worker->state, worker->userdata);
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

		static void onMouseNotify(lua_State* L, void* userdata) {
			auto worker = reinterpret_cast<HighGui*>(userdata);
			if (worker->has_data) {
				worker->has_data = false;
				check_error(L, worker->callback, worker->event, worker->x, worker->y, worker->flags, worker->userdata);
			}
		}

		Function callback;
		Object userdata;
		bool has_data = false;
		int pos = 0;
		int state = 0;
		int event = 0;
		int x = 0;
		int y = 0;
		int flags = 0;
	};

	std::map<size_t, HighGui> HighGui::registered_workers;
}

namespace cvextra {
	void setMouseCallback(
		lua_State* L,
		const std::string& winname,
		const Function& onMouse,
		const Object& userdata
	) {
		auto& worker = HighGui::add_worker(L, onMouse, userdata);
		registerCallback(&HighGui::onMouseNotify, &worker);
		cv::setMouseCallback(winname, &HighGui::registerMouseCallback, &worker);
	}

	int createButton(
		lua_State* L,
		const std::string& bar_name,
		const Function& on_change,
		const Object& userdata,
		int type,
		bool initial_button_state
	) {
		auto& worker = HighGui::add_worker(L, on_change, userdata);
		registerCallback(&HighGui::onButtonNotify, &worker);
		return cv::createButton(bar_name, &HighGui::registerButtonCallback, &worker, type, initial_button_state);
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
		auto& worker = HighGui::add_worker(L, onChange, userdata);
		registerCallback(&HighGui::onTrackbarNotify, &worker);
		int n = cv::createTrackbar(trackbarname, winname, nullptr, count, &HighGui::registerTrackbarCallback, &worker);
		cv::setTrackbarPos(trackbarname, winname, value);
		return n;
	}
}
