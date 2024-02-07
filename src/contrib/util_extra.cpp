#include "util_extra.h"
#include <registration.hpp>

// CV_EXPORTS_W : include this file in lua_generated_include

using namespace LUA_MODULE_NAME;

namespace {
	template<typename... Args>
	inline void check_error(lua_State* L, const Function& fn, Args&&... args) {
		lua_push(L, fn);

		int nargs = 0;
		([&] {
			// Do things in your "loop" lambda
			lua_push(L, args);
			nargs++;
			} (), ...);
		lua_call(L, nargs, 0);
	}

	struct ErrWorker {
		ErrWorker(
			lua_State* L,
			const Function& callback,
			const Object& userdata
		) {
			this->callback.assign(L, callback);
			this->userdata.assign(L, userdata);
		}

		~ErrWorker() {
			// callback and userdata lua_State are invalid at this point
			// because lua_State closes before the workers are destroyed on exit
			// therefore, dereference lua_State
			callback.free();
			userdata.free();
		}

		static std::map<size_t, ErrWorker> registered_workers;

		static ErrWorker& add_worker(lua_State* L, const Function& callback, const Object& userdata) {
			auto key = registered_workers.size();
			{
				std::lock_guard<std::mutex> lock(callback_mutex);
				registered_workers.emplace(std::piecewise_construct,
					std::forward_as_tuple(key),
					std::forward_as_tuple(L, callback, userdata));
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

		static void onCallbackNotify(lua_State* L, void* userdata) {
			auto worker = reinterpret_cast<ErrWorker*>(userdata);
			if (worker->has_data) {
				worker->has_data = false;
				check_error(L, worker->callback, worker->status, worker->func_name, worker->err_msg, worker->file_name, worker->line, worker->userdata);
			}
		}

		lua_State* L;
		Function callback;
		Object userdata;
		bool has_data = false;
		int status;
		const char* func_name;
		const char* err_msg;
		const char* file_name;
		int line;
	};

	std::map<size_t, ErrWorker> ErrWorker::registered_workers;
}

namespace cvextra {
	void redirectError(lua_State* L, const Function& errCallback, const Object& userdata) {
		auto& worker = ErrWorker::add_worker(L, errCallback, userdata);
		registerCallback(&ErrWorker::onCallbackNotify, &worker);
		cv::redirectError(&ErrWorker::registerErrCallback, &worker);
	}
}
