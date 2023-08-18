#pragma once
#include "util_extra.h"
#include <registration.hpp>

// CV_EXPORTS_W : include this file in lua_generated_include

namespace {
	using namespace LUA_MODULE_NAME;

	struct ErrWorker {
		ErrWorker(
			sol::function errCallback,
			sol::object userdata
		) :
			errCallback(errCallback),
			userdata(userdata),
			has_data(false) {}

		static std::vector<ErrWorker> registered_workers;

		static ErrWorker& add_worker(sol::function errCallback, sol::object userdata) {
			{
				std::lock_guard<std::mutex> lock(callback_mutex);
				registered_workers.emplace_back(errCallback, userdata);
			}
			return registered_workers.back();
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

		static void onCallbackNotify(void* userdata) {
			auto worker = reinterpret_cast<ErrWorker*>(userdata);
			if (worker->has_data) {
				worker->has_data = false;
				worker->errCallback(worker->status, worker->func_name, worker->err_msg, worker->file_name, worker->line, worker->userdata);
			}
		}

		static void call(void* userdata) {
			auto worker = reinterpret_cast<ErrWorker*>(userdata);
		}

		sol::function errCallback;
		sol::object userdata;
		bool has_data;
		int status;
		std::string func_name;
		std::string err_msg;
		std::string file_name;
		int line;
	};

	std::vector<ErrWorker> ErrWorker::registered_workers;
}

namespace cvextra {
	void redirectError(sol::function errCallback, sol::object userdata) {
		auto& worker = ErrWorker::add_worker(errCallback, userdata);
		registerCallback(&ErrWorker::onCallbackNotify, &worker);
		cv::redirectError(&ErrWorker::registerErrCallback, &worker);
	}
}
