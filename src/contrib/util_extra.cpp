#include "util_extra.h"
#include <registration.hpp>

// CV_EXPORTS_W : include this file in lua_generated_include

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

	template<typename... Args>
	struct AsyncWorker {
		AsyncWorker(
			lua_State* L,
			const Function& callback,
			const Object& userdata
		) {
			this->callback.assign(L, callback);
			this->userdata.assign(L, userdata);
		}

		~AsyncWorker() = default;

		static void handleCallback(Args... args, void* userdata) {
			auto lock = lock_callbacks();
			auto worker = reinterpret_cast<AsyncWorker<Args...>*>(userdata);
			worker->has_data = true;
			worker->args = std::make_tuple(std::forward<Args>(args)...);
		}

		static void handleNotify(lua_State* L, void* userdata) {
			auto worker = reinterpret_cast<AsyncWorker<Args...>*>(userdata);
			if (worker->has_data) {
				worker->has_data = false;
				std::apply(&check_error<Args..., const Object&>, std::tuple_cat(std::make_tuple(L, worker->callback), worker->args, std::tie(worker->userdata)));
			}
		}

		static std::map<size_t, AsyncWorker<Args...>> registered_workers;

		static AsyncWorker<Args...>& add_worker(lua_State* L, const Function& callback, const Object& userdata) {
			auto key = registered_workers.size();
			{
				auto lock = lock_callbacks();
				registered_workers.emplace(std::piecewise_construct,
					std::forward_as_tuple(key),
					std::forward_as_tuple(L, callback, userdata));
			}
			return registered_workers.at(key);
		}

		Function callback;
		Object userdata;
		bool has_data = false;
		std::tuple<Args...> args = std::tuple<Args...>();
	};

	template<typename... Args>
	std::map<size_t, AsyncWorker<Args...>> AsyncWorker<Args...>::registered_workers;
}

namespace cvextra {
	void redirectError(lua_State* L, const Function& errCallback, const Object& userdata) {
		using Worker = AsyncWorker<int, std::string, std::string, std::string, int>;
		auto& worker = Worker::add_worker(L, errCallback, userdata);
		registerCallback(&Worker::handleNotify, &worker);
		cv::redirectError([](int status, const char* func_name, const char* err_msg, const char* file_name, int line, void* userdata) {
			Worker::handleCallback(status, func_name, err_msg, file_name, line, userdata);
			return 0; // The return value isn't used
		}, &worker);
	}
}
