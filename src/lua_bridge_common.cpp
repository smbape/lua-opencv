#include <lua_bridge_common.hpp>

namespace LUA_MODULE_NAME {
	// ================================
	// misc
	// ================================

	bool lua_newkwargs_from_table(lua_State* L, int index, bool& is_valid) {
		is_valid = lua_istable(L, index);
		if (!is_valid) {
			return is_valid;
		}

		if (index < 0) {
			index += lua_gettop(L) + 1;
		}

		lua_newtable(L);
		// stack now contains: -1 => kwargs

		const auto kwargs_index = lua_gettop(L);
		const auto __top__ = kwargs_index + 1;

		// https://www.lua.org/manual/5.1/manual.html#lua_next

		lua_pushnil(L);  /* first key */
		// stack now contains: -2 => kwargs; -1 => nil

		while (lua_next(L, index) != 0) {
			// stack now contains: -3 => kwargs; -2 => key; -1 => value
			const auto key = lua_to(L, -2, static_cast<std::string*>(nullptr), is_valid);

			if (!is_valid) {
				/* removes 'value'; keeps 'key' for the next iteration */
				lua_pop(L, 1);
				// stack now contains: -2 => kwargs; -1 => key

				/* removes 'key'; break iteration */
				lua_pop(L, 1);
				// stack now contains: -1 => kwargs

				break;
			}

			const auto k = __top__ - 2;
			const auto v = __top__ - 1;
			lua_pushvalue(L, k);
			lua_pushvalue(L, v);
			lua_rawset(L, kwargs_index);

			/* removes 'value'; keeps 'key' for the next iteration */
			lua_pop(L, 1);
			// stack now contains: -2 => kwargs; -1 => key
		}

		if (is_valid) {
			usertype_push_metatable<Keywords>(L);
			lua_setmetatable(L, -2);
		}
		else {
			lua_pop(L, 1);
		}

		return is_valid;
	}
}

namespace {
	using namespace LUA_MODULE_NAME;

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

	void register_Global_state(lua_State* L) {
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

	std::mutex gil_mutex;
	std::unique_lock<std::mutex> gil{ gil_mutex, std::defer_lock };
	std::mutex yielder_mutex;

	void register_Callbacks(lua_State* L) {
		const struct luaL_Reg funcs_callbacks[] = {
			{ "notifyCallbacks", yield },
			{ "yield", yield },
			{ NULL, NULL }
		};

		lua_pushfuncs(L, funcs_callbacks);

		// the main thread has the lock
		gil.lock();
	}
}

namespace LUA_MODULE_NAME {
	lua_State* get_global_state() {
		return lua_globalState;
	}

	void init_global_state(lua_State* L) {
		register_Global_state(L);
		register_Keywords(L);
		register_Callbacks(L);
	}

	bool has_lua_jit() {
		return _has_lua_jit;
	}

	GilLock::GilLock(const bool lock) {
		if (lock) {
			mutex_lock = std::make_unique<std::unique_lock<std::mutex>>(gil_mutex);
		}
	}

	GilYield::GilYield() {
		yielder_lock = std::make_unique<std::unique_lock<std::mutex>>(yielder_mutex, std::defer_lock);
		if (yielder_lock->try_lock()) {
			// if (!gil.owns_lock()) {
			// 	LUAL_MODULE_ERROR_RETURN(L, "GIL is not locked.");
			// }
			gil.unlock();
		}
	}

	GilYield::~GilYield() {
		if (yielder_lock->owns_lock()) {
			// if (gil.owns_lock()) {
			// 	LUAL_MODULE_ERROR_RETURN(L, "GIL is locked.");
			// }
			gil.lock();
		}
	}

	int yield(lua_State* L) {
		GilYield yielder;
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
