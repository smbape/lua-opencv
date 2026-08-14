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

#define LUA_GIL_MUTEX_UNIQUE_NAME "__lua_brige_gil_mutex__"
#define LUA_GIL_LOCK_UNIQUE_NAME "__lua_brige_gil__"

namespace {
	using namespace LUA_MODULE_NAME;

	std::mutex gil_open_mutex;

	int global_luaopen_index = 0;
	thread_local int thread_local_luaopen_index = -1;

	void register_LuaOpenIndex(lua_State* L) {
		if (get_luaopen_index(L) != -1) {
			luaL_error(L, LUA_MODULE_LUAOPEN_STR " has been called more than once for the current lua_State");
			return;
		}

		{
			std::unique_lock lock(gil_open_mutex);
			thread_local_luaopen_index = global_luaopen_index++;
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

		// we make the assumption that there will be only one lua_open_* call per thread
		// the thread call lua_open_* must have the lock
		// the lock should be release on thread end
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

	int gil_mutex_destroy(lua_State* L) {
		auto gil_mutex = static_cast<std::shared_ptr<std::mutex>*>(lua_touserdata(L, 1));
		gil_mutex->~shared_ptr();
		return 0;
	}

	std::shared_ptr<std::mutex>& gil_mutex_create_and_push(lua_State* L) {
		lua_pushliteral(L, LUA_GIL_MUTEX_UNIQUE_NAME);

		auto gil_mutex = static_cast<std::shared_ptr<std::mutex>*>(lua_newuserdata(L, sizeof(std::shared_ptr<std::mutex>)));
		new(gil_mutex) std::shared_ptr<std::mutex>(std::make_shared<std::mutex>()); // userdata = new std::mutex()

		lua_newtable(L);
		lua_pushstring(L, "__gc");
		lua_pushcfunction(L, gil_mutex_destroy);
		lua_rawset(L, -3);

		lua_setmetatable(L, -2);
		lua_rawset(L, LUA_REGISTRYINDEX);

		lua_pushliteral(L, LUA_GIL_MUTEX_UNIQUE_NAME);
		lua_rawget(L, LUA_REGISTRYINDEX);

		return *gil_mutex;
	}

	void lua_push_this_thread_id_key(lua_State* L) {
		thread_local std::string thread_id_key = [] () {
			std::ostringstream id_key(LUA_GIL_LOCK_UNIQUE_NAME);
			id_key << std::this_thread::get_id();
			return id_key.str();
		} ();
		lua_pushlstring(L, thread_id_key.c_str(), thread_id_key.size());
	}

	struct GilHolder {
		std::shared_ptr<std::unique_lock<std::mutex>> gil;
		std::shared_ptr<std::mutex> gil_mutex;
	};

	int gil_destroy(lua_State* L) {
		auto gil_holder = static_cast<std::shared_ptr<GilHolder>*>(lua_touserdata(L, 1));
		gil_holder->~shared_ptr();
		return 0;
	}

	std::shared_ptr<std::unique_lock<std::mutex>> gil_create_and_push(lua_State* L, std::shared_ptr<std::mutex> gil_mutex) {
		lua_push_this_thread_id_key(L);

		auto gil_holder = static_cast<std::shared_ptr<GilHolder>*>(lua_newuserdata(L, sizeof(std::shared_ptr<GilHolder>)));
		new(gil_holder) std::shared_ptr<GilHolder>(std::make_shared<GilHolder>(GilHolder{
			.gil = std::make_shared<std::unique_lock<std::mutex>>(*gil_mutex, std::defer_lock),
			.gil_mutex = gil_mutex
		}));

		lua_newtable(L);
		lua_pushstring(L, "__gc");
		lua_pushcfunction(L, gil_destroy);
		lua_rawset(L, -3);

		lua_setmetatable(L, -2);
		lua_rawset(L, LUA_REGISTRYINDEX);

		lua_push_this_thread_id_key(L);
		lua_rawget(L, LUA_REGISTRYINDEX);

		return gil_holder->get()->gil;
	}

	std::shared_ptr<std::unique_lock<std::mutex>> get_or_create_gil(
		lua_State* L,
		std::shared_ptr<std::mutex> creator_gil_mutex,
		std::thread::id creator_thread_id,
		std::shared_ptr<std::unique_lock<std::mutex>> creator_gil
	) {
		if (std::this_thread::get_id() == creator_thread_id) {
			return creator_gil;
		}

		std::unique_lock lock(*creator_gil_mutex);
		return get_thread_gil(L, creator_gil_mutex);
	}
}

namespace LUA_MODULE_NAME {
	int get_luaopen_index(lua_State* L) {
		if (thread_local_luaopen_index == -1) {
			lua_pushliteral(L, LUA_MODULE_LUAOPEN_STR);
			lua_rawget(L, LUA_REGISTRYINDEX);
			if (lua_isnumber(L, -1)) {
				thread_local_luaopen_index = static_cast<int>(lua_tonumber(L, -1));
			}
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

	std::shared_ptr<std::mutex> get_gil_mutex(lua_State* L) {
		lua_pushliteral(L, LUA_GIL_MUTEX_UNIQUE_NAME);
		lua_rawget(L, LUA_REGISTRYINDEX);

		if (lua_isnil(L, -1)) {
			lua_pop(L, 1);
			gil_mutex_create_and_push(L);
		}

		decltype(auto) gil_mutex = *static_cast<std::shared_ptr<std::mutex>*>(lua_touserdata(L, -1));
		lua_pop(L, 1);
		return gil_mutex;
	}

	std::shared_ptr<std::unique_lock<std::mutex>> get_thread_gil(lua_State* L) {
		return get_thread_gil(L, get_gil_mutex(L));
	}

	std::shared_ptr<std::unique_lock<std::mutex>> get_thread_gil(lua_State* L, std::shared_ptr<std::mutex> gil_mutex) {
		lua_push_this_thread_id_key(L);
		lua_rawget(L, LUA_REGISTRYINDEX);

		if (lua_isnil(L, -1)) {
			lua_pop(L, 1);
			gil_create_and_push(L, gil_mutex);
		}

		decltype(auto) gil = static_cast<std::shared_ptr<GilHolder>*>(lua_touserdata(L, -1))->get()->gil;
		lua_pop(L, 1);
		return gil;
	}

	GilLock::GilLock(lua_State* L) : GilLock::GilLock(
		L,
		get_gil_mutex(L),
		std::this_thread::get_id(),
		get_thread_gil(L)
	) {
		// Nothing to do
	}

	GilLock::GilLock(
		lua_State* L,
		std::shared_ptr<std::mutex> creator_gil_mutex,
		std::thread::id creator_thread_id,
		std::shared_ptr<std::unique_lock<std::mutex>> creator_gil
	) : gil_mutex(creator_gil_mutex),
		gil(get_or_create_gil(L, creator_gil_mutex, creator_thread_id, creator_gil)),
		locked(false)
	{
		if (!gil->owns_lock()) {
			gil->lock();
			locked = true;
		}
	}

	GilLock::~GilLock() {
		if (locked) {
			gil->unlock();
		}
	}

	GilYield::GilYield(lua_State* L) : GilYield::GilYield(L, 0L) {
		// Nothing to do
	}

	GilYield::GilYield(lua_State* L, long timeout_ms) : timeout_ms(timeout_ms), gil_mutex(get_gil_mutex(L)), gil(get_thread_gil(L)), yielded(false) {
		if (gil->owns_lock()) {
			gil->unlock();
			yielded = true;

			if (timeout_ms > 0) {
				std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
			}
		}
	}

	GilYield::~GilYield() {
		if (yielded) {
			gil->lock();
		}
	}

	int yield(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc > 1) {
			return luaL_error(L, "too many arguments");
		}

		// TODO : find a better way to force context switch when another thread is waiting for the mutex
		long timeout_ms = 5;

		if (vargc == 1) {
			bool is_valid = false;
			timeout_ms = lua_to(L, 1, static_cast<long*>(nullptr), is_valid);
			if (!is_valid) {
				luaL_typeerror(L, 1, "long");
			}
		}

		GilYield yielder(L, timeout_ms);
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
