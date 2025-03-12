#include <registration.hpp>
#include <bit.hpp>

namespace {
	using namespace LUA_MODULE_NAME;

	void register_version(lua_State* L) {
		lua_pushliteral(L, "version");
		lua_pushliteral(L, "Lua bindings " LUA_MODULE_QUOTE_STRING(LUA_MODULE_VERSION) " for OpenCV " LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_VERSION));
		lua_rawset(L, -3);
	}

	void register_bit(lua_State* L) {
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setfield(L, -3, "bit");
		luaopen_bit(L);
		lua_pop(L, 1);
	}

	int _round(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc == 1) {
			bool is_valid;
			auto num = lua_to(L, 1, static_cast<double*>(nullptr), is_valid);
			if (!is_valid) {
				return luaL_typeerror(L, 1, "number");
			}
			return lua_push(L, std::round(num));
		}

		if (vargc == 2) {
			bool is_valid;

			auto num = lua_to(L, 1, static_cast<double*>(nullptr), is_valid);
			if (!is_valid) {
				return luaL_typeerror(L, 1, "number");
			}

			auto n = lua_to(L, 2, static_cast<int32_t*>(nullptr), is_valid);
			if (!is_valid) {
				return luaL_typeerror(L, 2, "integer");
			}

			double mult = std::pow((double)10, (double)n);
			return lua_push(L, std::round(num * mult) / mult);
		}

		return luaL_error(L, "1 or 2 arguments expected, got %d", vargc);
	}

	int _int(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc == 1) {
			bool is_valid;
			auto num = lua_to(L, 1, static_cast<double*>(nullptr), is_valid);
			if (!is_valid) {
				return luaL_typeerror(L, 1, "number");
			}
			return lua_push(L, static_cast<int>(num));
		}

		return luaL_error(L, "1 argument expected, got %d", vargc);
	}

	const struct luaL_Reg funcs_math[] = {
		{ "round", _round },
		{ "int", _int },
		{ NULL, NULL }
	};

	void register_math(lua_State* L) {
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setfield(L, -3, "math");
		lua_pushfuncs(L, funcs_math);
		lua_pop(L, 1);
	}

	const struct luaL_Reg funcs_callbacks[] = {
		{ "notifyCallbacks",    notifyCallbacks },
		{ NULL, NULL }
	};

	void regiter_callbacks(lua_State* L) {
		lua_pushfuncs(L, funcs_callbacks);
	}

	const struct luaL_Reg no_funcs[] = {
		{ NULL, NULL }
	};
}

namespace {
	struct CallbackHandler {
		Callback callback;
		void* userdata = nullptr;
	};

	std::map<int, CallbackHandler> registered_callbacks;
	std::vector<int> once_ids;
	int _callback_id = 0;

	std::shared_timed_mutex callback_mutex;

	struct Notifier {
		static std::shared_timed_mutex notifier_mutex;
		static bool notifying;

		bool notify;

		Notifier() {
			std::unique_lock lock(notifier_mutex);

			notify = !notifying;

			if (notify) {
				notifying = true;
			}
		}

		~Notifier() {
			std::unique_lock lock(notifier_mutex);

			if (notify) {
				notifying = false;
			}
		}

		operator const bool() const {
			return notify;
		}
	};

	std::shared_timed_mutex Notifier::notifier_mutex;
	bool Notifier::notifying = false;
}

namespace LUA_MODULE_NAME {
	int __call_constructor(lua_State* L) {
		auto vargc = lua_gettop(L);
		auto nargs = vargc - 1;

		lua_pushliteral(L, "new");
		lua_rawget(L, 1);

		for (int i = 2; i <= vargc; i++) {
			lua_pushvalue(L, i);
		}

		lua_call(L, nargs, LUA_MULTRET);

		return lua_gettop(L) - vargc;
	}

	std::unique_lock<std::shared_timed_mutex> lock_callbacks() {
		return std::unique_lock<std::shared_timed_mutex>(callback_mutex);
	}

	int registerCallback(Callback callback, void* userdata, std::optional<std::function<void(int)>> onRegistration) {
		auto lock = lock_callbacks();

		registered_callbacks.emplace(std::piecewise_construct,
			std::forward_as_tuple(_callback_id),
			std::forward_as_tuple(std::move(callback), userdata));

		if (onRegistration) {
			onRegistration.value()(_callback_id);
		}

		return _callback_id++;
	}

	int registerCallbackOnce(Callback callback, void* userdata, std::optional<std::function<void(int)>> onRegistration) {
		return registerCallback(std::move(callback), userdata, std::move([onRegistration](int callback_id) {
			once_ids.push_back(callback_id);
			if (onRegistration) {
				onRegistration.value()(callback_id);
			}
			}));
	}

	bool unregisterCallback(int callback_id) {
		auto lock = lock_callbacks();

		if (registered_callbacks.count(callback_id)) {
			registered_callbacks.erase(callback_id);
			return true;
		}

		return false;
	}

	int notifyCallbacks(lua_State* L) {
		Notifier notify;

		// avoid notifyCallbacks while already in notifyCallbacks
		if (notify) {
			auto lock = lock_callbacks();

			for (const auto& [callback_id, value] : registered_callbacks) {
				const auto& [callback, userdata] = value;
				callback(L, userdata);
			}

			for (const auto& callback_id : once_ids) {
				registered_callbacks.erase(callback_id);
			}
		}

		return 0;
	}
}

#define _stringify(s) #s
#define stringify(s) _stringify(s)

int LUA_MODULE_LUAOPEN(lua_State* L) {
#if LUA_VERSION_NUM < 502
	luaL_register(L, stringify(LUA_MODULE_NAME), no_funcs);
#else
	luaL_newlib(L, no_funcs);
#endif

	using namespace LUA_MODULE_NAME;

	init_global_state(L);

	register_version(L);
	register_Keywords(L);
	register_bit(L);
	register_math(L);
	regiter_callbacks(L);
	register_all(L);
	register_extensions(L);

	return 1;
}
