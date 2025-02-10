#include <lua_bridge_common.hpp>

namespace {
	lua_State* lua_globalState = nullptr;
}

namespace LUA_MODULE_NAME {
	lua_State* get_global_state() {
		return lua_globalState;
	}

	void init_global_state(lua_State* L) {
		lua_globalState = L;
	}

	/**
	 * https://en.cppreference.com/w/cpp/string/byte/atoi
	 */
	template<typename T>
	int atoi(lua_State* L, const std::string& s) {
		const char* str = s.c_str();
		auto len = s.length();

		T index = 0;
		decltype(len) i = 0;

		for (; i < len && std::isdigit(static_cast<unsigned char>(*str)); ++str) {
			int digit = *str - '0';
			index *= 10;
			index += digit;
			i++;
		}

		if (i != len) {
			luaL_error(L, "invalid index %s", s.c_str());
		}

		return index;
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
