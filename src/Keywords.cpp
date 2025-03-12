#include <registration.hpp>

namespace {
	using namespace LUA_MODULE_NAME;

	int Keywords_isinstance(lua_State* L) {
		return lua_push(L, lua_gettop(L) == 1 && usertype_info<Keywords>::lua_userdata_is(L, 1));
	}

	// Define the functions for Lua bindings
	int Keywords_constructor(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc == 0) {
			lua_newtable(L);
			return lua_push(L, static_cast<Keywords*>(nullptr));
		}

		if (vargc == 1) {
			if (!lua_istable(L, 1)) {
				return luaL_typeerror(L, 1, "table");
			}
			lua_pushvalue(L, 1);
			lua_push(L, static_cast<Keywords*>(nullptr));
			return lua_gettop(L) - vargc;
		}

		return luaL_error(L, "0 or 1 table argument expected, got %d", vargc);
	}

	int Keywords_has(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc == 0) {
			return luaL_error(L, "self is not defined");
		}

		if (!lua_istable(L, 1)) {
			return luaL_typeerror(L, 1, "table");
		}

		if (vargc != 2) {
			return luaL_error(L, "expecting one string argument");
		}

		if (lua_type(L, 2) != LUA_TSTRING) {
			return luaL_typeerror(L, 2, "string");
		}

		if (!lua_istable(L, 1)) {
			return luaL_typeerror(L, 1, "table");
		}

		size_t len;
		const char* key = lua_tolstring(L, 2, &len);
		lua_pushboolean(L, Keywords::has(L, 1, key));
		return 1;
	}

	int Keywords_get(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc == 2 && lua_istable(L, 1) && lua_type(L, 2) == LUA_TSTRING) {
			size_t len;
			const char* key = lua_tolstring(L, 2, &len);
			Keywords::push(L, 1, key);
			return 1;
		}

		return luaL_error(L, "Overload resolution failed");
	}

	int Keywords_size(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc == 1 && lua_istable(L, 1)) {
			lua_pushnumber(L, static_cast<lua_Number>(Keywords::size(L, 1)));
			return 1;
		}

		return luaL_error(L, "Overload resolution failed");
	}
}

namespace LUA_MODULE_NAME {
	int usertype_info<Keywords>::metatable = LUA_REFNIL;
	const void* usertype_info<Keywords>::signature;
	std::set<const void*> usertype_info<Keywords>::derives;
	const std::map<std::variant<std::string, int>, std::function<int(lua_State*)>> usertype_info<Keywords>::getters({});
	const std::map<std::variant<std::string, int>, std::function<int(lua_State*)>> usertype_info<Keywords>::setters({});

	bool usertype_info<Keywords>::lua_userdata_is(lua_State* L, int index) {
		if (!lua_istable(L, index) || !lua_getmetatable(L, index)) {
			return false;
		}

		auto signature = lua_topointer(L, -1);
		lua_pop(L, 1);

		return lua_userdata_signature_is<0, Keywords>(L, index, signature);
	}

	std::shared_ptr<Keywords> usertype_info<Keywords>::lua_userdata_to(lua_State* L, int index, bool& is_valid) {
		is_valid = usertype_info<Keywords>::lua_userdata_is(L, index);
		return lua_userdata_signature_to<Keywords>(L, index);
	}

	const struct luaL_Reg usertype_info<Keywords>::methods[] = {
		{"isinstance", Keywords_isinstance},
		{"new", Keywords_constructor},
		{"has", Keywords_has},
		{"get", Keywords_get},
		{"size", Keywords_size},
		{NULL, NULL} // Sentinel
	};

	const struct luaL_Reg usertype_info<Keywords>::meta_methods[] = {
		{"__call", __call_constructor},
		{NULL, NULL} // Sentinel
	};

	void Keywords::push(lua_State* L, int index, const char* key) {
		if (index < 0) {
			index += lua_gettop(L) + 1;
		}
		lua_pushstring(L, key);
		lua_gettable(L, index);
	}

	bool Keywords::has(lua_State* L, int index, const char* key) {
		push(L, index, key);
		auto result = !lua_isnil(L, -1);
		lua_pop(L, 1);
		return result;
	}

	/**
	 * https://stackoverflow.com/a/6142700
	 * @param  L     [description]
	 * @param  index [description]
	 * @return       [description]
	 */
	int Keywords::size(lua_State* L, int index) {
		int size = 0;

		// Push another reference to the table on top of the stack (so we know
		// where it is, and this function can work for negative, positive and
		// pseudo indices
		lua_pushvalue(L, index);
		// stack now contains: -1 => table

		lua_pushnil(L);
		// stack now contains: -1 => nil; -2 => table

		while (lua_next(L, -2)) {
			// pop value
			lua_pop(L, 1);
			// stack now contains: -1 => key; -2 => table

			size++;
		}

		// stack now contains: -1 => table (when lua_next returns 0 it pops the key
		// but does not push anything.)
		// Pop table
		lua_pop(L, 1);
		// Stack is now the same as it was on entry to this function

		return size;
	}

	void register_Keywords(lua_State* L) {
		lua_register_class<Keywords>(L, "kwargs");
	}

	int lua_push(lua_State* L, Keywords* raw_ptr) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, usertype_info<Keywords>::metatable);
		lua_setmetatable(L, -2);
		return 1; // return table
	}
}
