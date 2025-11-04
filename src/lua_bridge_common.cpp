#include <lua_bridge_common.hpp>

namespace {
	lua_State* lua_globalState = nullptr;

	bool _has_lua_jit = false;

	struct StateGuard {
		StateGuard() = default;

		~StateGuard() {
			lua_globalState = nullptr;
		}
	};

	int StateGuard__gc(lua_State* L) {
		auto userdata_ptr = static_cast<StateGuard*>(lua_touserdata(L, 1));
		userdata_ptr->~StateGuard();
		return 0;
	}
}

namespace LUA_MODULE_NAME {
	lua_State* get_global_state() {
		return lua_globalState;
	}

	void init_global_state(lua_State* L) {
		lua_globalState = L;

		// userdata = new StateGuard();
		auto userdata_ptr = static_cast<StateGuard*>(lua_newuserdata(L, sizeof(StateGuard)));
		new(userdata_ptr) StateGuard();

		// metatable = { __gc = function() --[[ dereference the global state ]] end }
		lua_newtable(L);
		lua_pushliteral(L, "__gc");
		lua_pushcfunction(L, (lua_CFunction)StateGuard__gc);
		lua_rawset(L, -3);

		// setmetatable(userdata, metatable)
		lua_setmetatable(L, -2);

		// keep reference to userdata until lua_State is closed
		luaL_ref(L, LUA_REGISTRYINDEX);

		// check if is luajit
		luaL_dostring(L, "return type(jit) == 'table'");
		_has_lua_jit = !!lua_toboolean(L, -1);
		lua_pop(L, 1);
	}

	bool has_lua_jit() {
		return _has_lua_jit;
	}
}

namespace {
	struct CallbackHandler {
		LUA_MODULE_NAME::Callback callback;
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
}

namespace {
	/**
	 * https://en.cppreference.com/w/cpp/string/byte/atoi
	 */
	template<typename T>
	int _atoi(lua_State* L, const std::string& s) {
		const char* str = s.c_str();
		auto len = s.length();

		T value = 0;
		decltype(len) i = 0;

		for (; i < len && std::isdigit(static_cast<unsigned char>(*str)); ++str) {
			int digit = *str - '0';
			value *= 10;
			value += digit;
			i++;
		}

		if (i != len) {
			luaL_error(L, "invalid index %s", s.c_str());
		}

		return value;
	}
}

namespace LUA_MODULE_NAME {
	/**
	 * https://en.cppreference.com/w/cpp/string/byte/atoi
	 */
	size_t atosize_t(lua_State* L, const std::string& s) {
		return _atoi<size_t>(L, s);
	}

	/**
	 * https://en.cppreference.com/w/cpp/string/byte/atoi
	 */
	int atoi(lua_State* L, const std::string& s) {
		return _atoi<int>(L, s);
	}
}
