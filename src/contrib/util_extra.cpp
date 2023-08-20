#include "util_extra.h"
#include <registration.hpp>

// CV_EXPORTS_W : include this file in lua_generated_include

namespace {
	using namespace LUA_MODULE_NAME;

	inline void check_error(sol::protected_function_result result, sol::this_state& ts) {
		if (!result.valid()) {
			sol::error err = result;

			sol::call_status status = result.status();
			LUA_MODULE_ERROR("Something went horribly wrong "
				"running the code: "
				<< sol::to_string(status) << " error"
				<< "\n\t" << err.what());

			sol::state_view lua(ts);
			luaL_error(lua.lua_state(), "callback");
		}
	}

	struct ErrWorker {
		ErrWorker(
			sol::safe_function errCallback,
			sol::object userdata
		) :
			errCallback(errCallback),
			userdata(userdata),
			has_data(false) {}

		static std::map<size_t, ErrWorker> registered_workers;

		static ErrWorker& add_worker(sol::safe_function errCallback, sol::object userdata) {
			auto key = registered_workers.size();
			{
				std::lock_guard<std::mutex> lock(callback_mutex);
				registered_workers.emplace(std::piecewise_construct,
					std::forward_as_tuple(key),
					std::forward_as_tuple(errCallback, userdata));
			}
			return registered_workers.at(key);
		}

		static int registerErrCallback(int status, const char* func_name, const char* err_msg, const char* file_name, int line, void* userdata) {
			std::lock_guard<std::mutex> lock(callback_mutex);
			auto worker = reinterpret_cast<ErrWorker*>(userdata);
			worker->has_data = true;
			worker->status = status;
			worker->func_name = func_name;
			worker->err_msg = err_msg;
			worker->file_name = file_name;
			worker->line = line;
			return 0; // The return value isn't used
		}

		static void onCallbackNotify(void* userdata, sol::this_state& ts) {
			auto worker = reinterpret_cast<ErrWorker*>(userdata);
			if (worker->has_data) {
				worker->has_data = false;
				check_error(worker->errCallback(worker->status, worker->func_name, worker->err_msg, worker->file_name, worker->line, worker->userdata), ts);
			}
		}

		static void call(void* userdata) {
			auto worker = reinterpret_cast<ErrWorker*>(userdata);
		}

		sol::safe_function errCallback;
		sol::object userdata;
		bool has_data;
		int status;
		std::string func_name;
		std::string err_msg;
		std::string file_name;
		int line;
	};

	std::map<size_t, ErrWorker> ErrWorker::registered_workers;
}

namespace cvextra {
	void redirectError(sol::safe_function errCallback, sol::object userdata) {
		auto& worker = ErrWorker::add_worker(errCallback, userdata);
		registerCallback(&ErrWorker::onCallbackNotify, &worker);
		cv::redirectError(&ErrWorker::registerErrCallback, &worker);
	}
}
