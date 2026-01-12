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

	ThreadSafeSetMap<std::size_t, std::size_t> type_children_map;

	std::mutex yielder_mutex;
	std::vector<std::unique_ptr<std::mutex>> gil_mutexes;

	int global_luaopen_index = 0;
	thread_local int thread_local_luaopen_index = -1;

	void register_LuaOpenIndex(lua_State* L) {
		{
			std::unique_lock yielder_lock(yielder_mutex);
			thread_local_luaopen_index = global_luaopen_index++;
			gil_mutexes.push_back(std::make_unique<std::mutex>());
		}
		lua_pushliteral(L, LUA_MODULE_LUAOPEN_STR);
		lua_pushnumber(L, thread_local_luaopen_index);
		lua_rawset(L, LUA_REGISTRYINDEX);
	}

	void register_Callbacks(lua_State* L) {
		const struct luaL_Reg callbacks_funcs[] = {
			{ "notifyCallbacks", yield },
			{ "yield", yield },
			{ NULL, NULL }
		};
		lua_pushfuncs(L, callbacks_funcs);

		// the main thread has the lock
		thread_local GilLock lock(L);
	}

	int lua__self(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc == 0) {
			return luaL_error(L, "self is not defined");
		}

		if (vargc != 1) {
			return luaL_error(L, "too many arguments");
		}

		bool is_valid = false;
		auto ptr = lua_to(L, 1, static_cast<void**>(nullptr), is_valid);
		lua_push(L, ptr);
		return 1;
	}

	void register_GetSelf(lua_State* L) {
		lua_pushliteral(L, "__self");
		lua_pushcfunction(L, lua__self);
		lua_rawset(L, -3);
	}
}

namespace LUA_MODULE_NAME {
	int get_luaopen_index(lua_State* L) {
	    if (thread_local_luaopen_index == -1) {
	        lua_pushliteral(L, LUA_MODULE_LUAOPEN_STR);
	        lua_rawget(L, LUA_REGISTRYINDEX);
	        thread_local_luaopen_index = static_cast<int>(lua_tonumber(L, -1));
	        lua_pop(L, 1);
	    }
	    return thread_local_luaopen_index;
	}

	void register_Common(lua_State* L) {
		register_LuaOpenIndex(L);
		register_Keywords(L);
		register_Callbacks(L);
		register_GetSelf(L);
	}

	std::mutex& get_gil_mutex(lua_State* L) {
		return *gil_mutexes.at(get_luaopen_index(L));
	}

	std::unique_lock<std::mutex>& get_thread_lock(lua_State* L) {
		thread_local std::unique_lock lock{ get_gil_mutex(L), std::defer_lock };
		return lock;
	}

	GilLock::GilLock(lua_State* L) : L(L), locked(false) {
		auto& lock = get_thread_lock(L);
		if (!lock.owns_lock()) {
			lock.lock();
			locked = true;
		}
	}

	GilLock::~GilLock() {
		if (locked) {
			get_thread_lock(L).unlock();
		}
	}

	GilYield::GilYield(lua_State* L) : L(L), yielded(false) {
		auto& lock = get_thread_lock(L);
		if (lock.owns_lock()) {
			using namespace std::chrono_literals;
			lock.unlock();
			// TODO : find a better way to force context switch when another thread is waiting for the mutex
			std::this_thread::sleep_for(5ms);
			yielded = true;
		}
	}

	GilYield::~GilYield() {
		if (yielded) {
			get_thread_lock(L).lock();
		}
	}

	int yield(lua_State* L) {
		GilYield yielder(L);
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
