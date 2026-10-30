#include <lua_bridge_common.hpp>

#define luaB_require(L, modname) _luaB_require(L, "" modname, sizeof(modname) / sizeof(char) - 1)

namespace {
	int _luaB_require(lua_State* L, const char* modname, size_t len) {
		lua_getglobal(L, "require");
		if (lua_isnil(L, -1)) {
			luaL_error(L, "global variable 'require' wast not found");
		}

		lua_pushlstring(L, modname, len);
		lua_call(L, 1, 1);

		return lua_gettop(L);
	}

	void* lua_cdata_to_pointer(lua_State* L, int index, bool& is_valid) {
		/* working with luajit 2.x but may not work when luajit updates to 3.0 */
		is_valid = lua_type(L, index) == LUA_TCDATA;
		if (!is_valid) {
			return nullptr;
		}

		if (index < 0) {
			index += lua_gettop(L) + 1;
		}

		is_valid = false;

		auto top = lua_gettop(L);
		intptr_t ptr = 0;
		uint8_t* bytes = reinterpret_cast<uint8_t*>(&ptr);
		size_t N;

		// call cdata_utils.get_cdata_bytes(cdata)
		luaB_require(L, LUA_MODULE_NAME_STR ".cdata_utils.get_cdata_bytes");
		lua_pushvalue(L, index);
		if (lua_pcall(L, 1, LUA_MULTRET, 0) != 0) {
			goto invalid;
		}

		N = lua_gettop(L) - top;
		if (N == 0) {
			goto invalid;
		}

		for (auto i = 1; i <= N; ++i) {
			bytes[i - 1] = static_cast<uint8_t>(lua_tonumber(L, top + i));
		}

		is_valid = true;
		lua_settop(L, top);
		return reinterpret_cast<void*>(ptr);

		invalid:
		if (top < lua_gettop(L)) {
			lua_settop(L, top);
		}

		return nullptr;
	}

	void* lua_self_pointer(lua_State* L, int index, bool& is_valid) {
		void* ptr = nullptr;

		if (index < 0) {
			index += lua_gettop(L) + 1;
		}

		is_valid = (lua_isuserdata(L, index) || lua_istable(L, index)) && lua_getmetatable(L, index); // push metatable
		if (!is_valid) {
			return ptr;
		}

		lua_pushliteral(L, "__has_self");
		lua_rawget(L, -2); // push metatable.__has_self
		is_valid = lua_isboolean(L, -1) && lua_toboolean(L, -1);
		lua_pop(L, 2); // pop metatable.__has_self, metatable

		if (!is_valid) {
			return ptr;
		}

		lua_pushliteral(L, "__self");
		lua_gettable(L, index); // push userdata.__self
		is_valid = lua_islightuserdata(L, -1);

		if (is_valid) {
			ptr = lua_touserdata(L, -1);
			is_valid = static_cast<bool>(ptr);
		}

		lua_pop(L, 1); // pop userdata.__self
		return ptr;
	}
}

namespace LUA_MODULE_NAME {


	// ================================
	// bool
	// ================================

	bool lua_to(lua_State* L, int index, bool*, bool& is_valid) {
		if constexpr (has_lua_to_custom_bridge_v<bool>) {
			const auto v = lua_to_custom_bridge<bool>::lua_to(L, index, is_valid);
			if (is_valid) {
				return v;
			}
		}

		is_valid = lua_isboolean(L, index);
		return is_valid && !!lua_toboolean(L, index);
	}


	// ================================
	// const char*
	// ================================

	const char* lua_to(lua_State* L, int index, const char**, bool& is_valid) {
		if constexpr (has_lua_to_custom_bridge_v<const char*>) {
			const auto v = lua_to_custom_bridge<const char*>::lua_to(L, index, is_valid);
			if (is_valid) {
				return v;
			}
		}

		is_valid = lua_isnil(L, index);
		if (is_valid) {
			return nullptr;
		}

		is_valid = lua_islightuserdata(L, index);
		if (is_valid) {
			return static_cast<const char*>(lua_touserdata(L, index));
		}

		is_valid = lua_type(L, index) == LUA_TSTRING;
		if (!is_valid) {
			return nullptr;
		}

		size_t len;
		auto c_str = lua_tolstring(L, index, &len);
		return c_str;
	}


	// ================================
	// void*
	// ================================

	void* lua_to(lua_State* L, int index, void**, bool& is_valid) {
		if constexpr (has_lua_to_custom_bridge_v<void*>) {
			const auto ptr = lua_to_custom_bridge<void*>::lua_to(L, index, is_valid);
			if (is_valid) {
				return ptr;
			}
		}

		is_valid = lua_isnil(L, index);
		if (is_valid) {
			return nullptr;
		}

		is_valid = lua_type(L, index) == LUA_TSTRING;
		if (is_valid) {
			size_t len;
			auto c_str = lua_tolstring(L, index, &len);
			return const_cast<char*>(c_str);
		}

		is_valid = lua_islightuserdata(L, index);
		if (is_valid) {
			return lua_touserdata(L, index);
		}

		void* ptr;

		ptr = lua_cdata_to_pointer(L, index, is_valid);
		if (is_valid) {
			return ptr;
		}

		ptr = lua_self_pointer(L, index, is_valid);
		if (is_valid) {
			return ptr;
		}

		return nullptr;
	}


	// ================================
	// misc functions
	// ================================

	int try_mt__index(lua_State* L) {
		if (lua_getmetatable(L, 1)) {
			lua_pushvalue(L, 2); // push the key
			lua_rawget(L, -2);
			lua_remove(L, -2); // remove the metatable

			if (!lua_isnil(L, -1)) {
				return 1; // return metatable[key]
			}

			lua_pop(L, 1); // pop nil
		}

		return 0;
	}

	int lua_missing_declaration(lua_State* L) {
		const auto arg = 2;
		char const* sname;
		if (lua_type(L, arg) == LUA_TSTRING) {
			sname = lua_tostring(L, arg);
		}
		else if (lua_type(L, arg) == LUA_TLIGHTUSERDATA) {
			sname = "light userdata";  /* special name for messages */
		}
		else {
			sname = luaL_typename(L, arg);  /* standard name */
		}
		luaL_error(L, "missing declaration for symbol '%s'", sname);
		return 0;
	}

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
#define LUA_CONDITION_VARIABLE_UNIQUE_NAME "__lua_brige_condition_variable__"
#define LUA_YIELD_COUNTER_UNIQUE_NAME "__lua_brige_yield_counter__"

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

	int luaB_tolighuserdata(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc == 0) {
			return luaL_error(L, "self is not defined");
		}

		if (vargc != 1) {
			return luaL_error(L, "too many arguments");
		}

		bool is_valid = false;
		auto ptr = lua_to(L, 1, static_cast<void**>(nullptr), is_valid);

		if (!is_valid) {
			luaL_typeerror(L, 1, "void*");
		}

		lua_push(L, ptr);
		return 1;
	}

	void register_GetSelf(lua_State* L) {
		lua_pushliteral(L, "__self");
		lua_pushcfunction(L, luaB_tolighuserdata);
		lua_rawset(L, -3);

		lua_pushliteral(L, "tolightuserdata");
		lua_pushcfunction(L, luaB_tolighuserdata);
		lua_rawset(L, -3);
	}

	int luaB_createtable(lua_State *L) {
		auto vargc = lua_gettop(L);

		if (vargc > 2) {
			return luaL_error(L, "too many arguments");
		}

		bool is_valid = false;

		int narr = 0;
		if (vargc >= 1) {
			narr = lua_to(L, 1, static_cast<int*>(nullptr), is_valid);
			if (!is_valid) {
				luaL_typeerror(L, 1, "int");
			}
		}

		int nrec = 0;
		if (vargc >= 2) {
			nrec = lua_to(L, 2, static_cast<int*>(nullptr), is_valid);
			if (!is_valid) {
				luaL_typeerror(L, 2, "int");
			}
		}

		lua_createtable(L, narr, nrec);
		return 1;
	
	}

	void register_CreateTable(lua_State* L) {
		lua_pushliteral(L, "createtable");
		lua_pushcfunction(L, luaB_createtable);
		lua_rawset(L, -3);
	}

	template<typename T>
	int _generic__gc(lua_State* L) {
		using SharedPtr = std::shared_ptr<T>;
		auto userdata_ptr = static_cast<SharedPtr*>(lua_touserdata(L, 1));
		userdata_ptr->~SharedPtr();
		return 0;
	}

	void gil_mutex_create_and_push(lua_State* L) {
		lua_pushliteral(L, LUA_GIL_MUTEX_UNIQUE_NAME);

		auto gil_mutex = static_cast<std::shared_ptr<std::mutex>*>(lua_newuserdata(L, sizeof(std::shared_ptr<std::mutex>)));
		new(gil_mutex) std::shared_ptr<std::mutex>(std::make_shared<std::mutex>()); // userdata = new std::mutex()

		lua_newtable(L);
		lua_pushstring(L, "__gc");
		lua_pushcfunction(L, _generic__gc<std::mutex>);
		lua_rawset(L, -3);

		lua_setmetatable(L, -2);
		lua_rawset(L, LUA_REGISTRYINDEX);

		lua_pushliteral(L, LUA_GIL_MUTEX_UNIQUE_NAME);
		lua_rawget(L, LUA_REGISTRYINDEX);
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

	void gil_create_and_push(lua_State* L, std::shared_ptr<std::mutex> gil_mutex) {
		lua_push_this_thread_id_key(L);

		auto gil_holder = static_cast<std::shared_ptr<GilHolder>*>(lua_newuserdata(L, sizeof(std::shared_ptr<GilHolder>)));
		new(gil_holder) std::shared_ptr<GilHolder>(std::make_shared<GilHolder>(GilHolder{
			.gil = std::make_shared<std::unique_lock<std::mutex>>(*gil_mutex, std::defer_lock),
			.gil_mutex = gil_mutex
		}));

		lua_newtable(L);
		lua_pushstring(L, "__gc");
		lua_pushcfunction(L, _generic__gc<GilHolder>);
		lua_rawset(L, -3);

		lua_setmetatable(L, -2);
		lua_rawset(L, LUA_REGISTRYINDEX);

		lua_push_this_thread_id_key(L);
		lua_rawget(L, LUA_REGISTRYINDEX);
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

	void condition_variable_create_and_push(lua_State* L) {
		lua_pushliteral(L, LUA_CONDITION_VARIABLE_UNIQUE_NAME);

		auto cv = static_cast<std::shared_ptr<std::condition_variable>*>(lua_newuserdata(L, sizeof(std::shared_ptr<std::condition_variable>)));
		new(cv) std::shared_ptr<std::condition_variable>(std::make_shared<std::condition_variable>()); // userdata = new std::condition_variable()

		lua_newtable(L);
		lua_pushstring(L, "__gc");
		lua_pushcfunction(L, _generic__gc<std::condition_variable>);
		lua_rawset(L, -3);

		lua_setmetatable(L, -2);
		lua_rawset(L, LUA_REGISTRYINDEX);

		lua_pushliteral(L, LUA_CONDITION_VARIABLE_UNIQUE_NAME);
		lua_rawget(L, LUA_REGISTRYINDEX);
	}

	std::shared_ptr<std::condition_variable> get_condition_variable(lua_State* L) {
		lua_pushliteral(L, LUA_CONDITION_VARIABLE_UNIQUE_NAME);
		lua_rawget(L, LUA_REGISTRYINDEX);

		if (lua_isnil(L, -1)) {
			lua_pop(L, 1);
			condition_variable_create_and_push(L);
		}

		decltype(auto) cv = *static_cast<std::shared_ptr<std::condition_variable>*>(lua_touserdata(L, -1));
		lua_pop(L, 1);
		return cv;
	}

	short get_yielding_counter(lua_State* L) {
		lua_pushliteral(L, LUA_YIELD_COUNTER_UNIQUE_NAME);
		lua_rawget(L, LUA_REGISTRYINDEX);

		if (lua_isnil(L, -1)) {
			lua_pop(L, 1);
			lua_pushliteral(L, LUA_YIELD_COUNTER_UNIQUE_NAME);
			lua_push(L, static_cast<short>(0));
			lua_rawset(L, LUA_REGISTRYINDEX);
			return 0;
		}

		decltype(auto) counter = static_cast<short>(lua_tonumber(L, -1));
		lua_pop(L, 1);
		return counter;
	}

	void increment_yielding(lua_State* L) {
		lua_pushliteral(L, LUA_YIELD_COUNTER_UNIQUE_NAME);
		lua_pushnumber(L, get_yielding_counter(L) + 1);
		lua_rawset(L, LUA_REGISTRYINDEX);
	}

	void decrement_yielding(lua_State* L) {
		lua_pushliteral(L, LUA_YIELD_COUNTER_UNIQUE_NAME);
		lua_pushnumber(L, get_yielding_counter(L) - 1);
		lua_rawset(L, LUA_REGISTRYINDEX);
	}

	bool is_yielding(lua_State* L) {
		return get_yielding_counter(L) != 0;
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
		register_CreateTable(L);
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
	) :
		gil_mutex(creator_gil_mutex),
		gil(get_or_create_gil(L, creator_gil_mutex, creator_thread_id, creator_gil)),
		cv(get_condition_variable(L)),
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
			cv->notify_all();
		}
	}

	GilYield::GilYield(lua_State* L, bool yielding_lock) : GilYield::GilYield(L, 0L, yielding_lock) {
		// Nothing to do
	}

	GilYield::GilYield(lua_State* L, long timeout_ms, bool yielding_lock) :
		L(L),
		timeout_ms(timeout_ms),
		gil_mutex(get_gil_mutex(L)),
		gil(get_thread_gil(L)),
		cv(get_condition_variable(L)),
		yielding_lock(yielding_lock),
		yielded(false) {
		if (!gil->owns_lock()) {
			return;
		}

		increment_yielding(L);
		gil->unlock();
		yielded = true;

		if (timeout_ms > 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
		}
	}

	GilYield::~GilYield() {
		if (!yielded) {
			return;
		}

		gil->lock();
		decrement_yielding(L);

		if (!yielding_lock && is_yielding(L)) {
			cv->wait(*gil, [L = L] { return !is_yielding(L); });
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

#define LUA_MODULE_LUAOPEN_cdata_utils LUA_TOKEN_CONCAT_3(luaopen_, LUA_MODULE_NAME, _cdata_utils)
LUAAPI(int) LUA_MODULE_LUAOPEN_cdata_utils(lua_State* L) {
	const char cdata_utils_filename[] = "<built-in>/" LUA_MODULE_NAME_STR "/cdata_utils.lua";
	const char cdata_utils_string[] =
#include "cdata_utils_string.lua.inc"
	;

	auto top = lua_gettop(L);

	if (luaL_loadbuffer(L, cdata_utils_string, sizeof(cdata_utils_string) - 1, cdata_utils_filename) != 0) {
		lua_error(L);
	}

	lua_call(L, 0, LUA_MULTRET);
	return lua_gettop(L) - top;
}

#define LUA_MODULE_LUAOPEN_cdata_utils_tovoid_ptr LUA_TOKEN_CONCAT_3(luaopen_, LUA_MODULE_NAME, _cdata_utils_tovoid_ptr)
LUAAPI(int) LUA_MODULE_LUAOPEN_cdata_utils_tovoid_ptr(lua_State* L) {
	auto top = lua_gettop(L);
	auto cdata_utils = luaB_require(L, LUA_MODULE_NAME_STR ".cdata_utils");
	lua_pushliteral(L, "tovoid_ptr");
	lua_rawget(L, cdata_utils);
	lua_replace(L, cdata_utils);
	return lua_gettop(L) - top;
}

#define LUA_MODULE_LUAOPEN_cdata_utils_get_cdata_bytes LUA_TOKEN_CONCAT_3(luaopen_, LUA_MODULE_NAME, _cdata_utils_get_cdata_bytes)
LUAAPI(int) LUA_MODULE_LUAOPEN_cdata_utils_get_cdata_bytes(lua_State* L) {
	auto top = lua_gettop(L);
	auto cdata_utils = luaB_require(L, LUA_MODULE_NAME_STR ".cdata_utils");
	lua_pushliteral(L, "get_cdata_bytes");
	lua_rawget(L, cdata_utils);
	lua_replace(L, cdata_utils);
	return lua_gettop(L) - top;
}
