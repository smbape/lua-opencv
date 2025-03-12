#include <lua_bridge_common.hpp>

namespace {
	lua_State* lua_globalState = nullptr;

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
	}

	/**
	 * https://en.cppreference.com/w/cpp/string/byte/atoi
	 */
	template<typename T>
	int atoi(lua_State* L, const std::string& s) {
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

	/**
	 * https://en.cppreference.com/w/cpp/string/byte/atoi
	 */
	size_t atosize_t(lua_State* L, const std::string& s) {
		return atoi<size_t>(L, s);
	}

	/**
	 * https://en.cppreference.com/w/cpp/string/byte/atoi
	 */
	int atoi(lua_State* L, const std::string& s) {
		return atoi<int>(L, s);
	}
}
