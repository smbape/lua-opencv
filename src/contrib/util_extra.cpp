#include "util_extra.h"
#include "callback_function.impl.hpp"

using namespace LUA_MODULE_NAME;

namespace cvextra {
	void redirectError(lua_State* L, const Function& errCallback, const Object& userdata) {
		auto invoker = lua_tocallback(L, errCallback, userdata, static_cast<callback_function_t<cv::ErrorCallback>*>(nullptr));
		cv::redirectError(invoker.callback, invoker.userdata);
	}
}
