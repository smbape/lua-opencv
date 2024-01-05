#include "util_extra.h"
#include <registration.hpp>

// CV_EXPORTS_W : include this file in lua_generated_include

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

	struct ErrWorker {
		ErrWorker(
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

		static std::map<size_t, ErrWorker> registered_workers;

		static ErrWorker& add_worker(sol::state_view& lua, sol::function callback, sol::object userdata) {
			auto key = registered_workers.size();
			{
				std::lock_guard<std::mutex> lock(callback_mutex);
				registered_workers.emplace(std::piecewise_construct,
					std::forward_as_tuple(key),
					std::forward_as_tuple(lua, callback, userdata));
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
				check_error(ts, worker->callback(), worker->status, worker->func_name, worker->err_msg, worker->file_name, worker->line, worker->userdata());
			}
		}

		static void call(void* userdata) {
			auto worker = reinterpret_cast<ErrWorker*>(userdata);
		}

		lua_State* L;
		int callback_ref = LUA_REFNIL;
		int userdata_ref = LUA_REFNIL;
		bool has_data = false;
		int status;
		std::string func_name;
		std::string err_msg;
		std::string file_name;
		int line;
	};

	std::map<size_t, ErrWorker> ErrWorker::registered_workers;
}

namespace cvextra {
	void redirectError(sol::state_view& lua, sol::function errCallback, sol::object userdata) {
		auto& worker = ErrWorker::add_worker(lua, errCallback, userdata);
		registerCallback(&ErrWorker::onCallbackNotify, &worker);
		cv::redirectError(&ErrWorker::registerErrCallback, &worker);
	}
}
