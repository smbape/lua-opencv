#include <registration.hpp>
#include <bit.hpp>

namespace {
	using namespace LUA_MODULE_NAME;

	void register_version(lua_State* L) {
		lua_pushliteral(L, "version");
		lua_pushliteral(L, "Lua bindings " LUA_MODULE_QUOTE_STRING(LUA_MODULE_VERSION) " for " LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_NAME) " " LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_VERSION));
		lua_rawset(L, -3);
	}

	void register_bit(lua_State* L) {
#ifdef LUA_BITLIBNAME
		lua_pushliteral(L, "bit");
		lua_getglobal(L, LUA_BITLIBNAME);
		lua_rawset(L, -3);
#else
		lua_pushliteral(L, "bit");
		lua_newtable(L);
		luaopen_bit(L);
		lua_rawset(L, -3);

#if LUA_VERSION_NUM >= 503
		// Lua supports the following bitwise operators
		lua_pushliteral(L, "bit");
		lua_rawget(L, -2); // push bit table

		const char bit_filename[] = "<built-in>/" LUA_MODULE_NAME_STR "/bit.lua";
		const char bit_string[] =
#include "bit_string.lua.inc"
		;

		auto top = lua_gettop(L);

		if (luaL_loadbuffer(L, bit_string, sizeof(bit_string) - 1, bit_filename) != 0) {
			lua_error(L);
		}

		lua_call(L, 0, LUA_MULTRET);
		auto nresults = lua_gettop(L) - top;

		if (nresults != 1) {
			luaL_error(L, "%s: 1 result expected, got %d", bit_filename, nresults);
		} else if (!lua_isfunction(L, -1)) {
			luaL_typeerror(L, -1, "function");
		}

		lua_pushvalue(L, -2);
		lua_call(L, 1, 0);
		lua_pop(L, 1); // pop bit table
#endif

#endif
	}

	int _round(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc == 1) {
			bool is_valid;
			auto num = lua_to(L, 1, static_cast<double*>(nullptr), is_valid);
			if (!is_valid) {
				return luaL_typeerror(L, 1, "number");
			}
			lua_push(L, std::round(num));
			return 1;
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
			lua_push(L, std::round(num * mult) / mult);
			return 1;
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
			lua_push(L, static_cast<int>(num));
			return 1;
		}

		return luaL_error(L, "1 argument expected, got %d", vargc);
	}

	void register_math(lua_State* L) {
		const struct luaL_Reg funcs[] = {
			{ "round", _round },
			{ "int", _int },
			{ NULL, NULL }
		};

		lua_pushliteral(L, "math");
		lua_newtable(L);
		lua_pushfuncs(L, funcs);
		lua_rawset(L, -3);
	}

	/* trim.c - based on http://lua-users.org/lists/lua-l/2009-12/msg00951.html from Sean Conner */
	int trim(lua_State* L) {
		auto vargc = lua_gettop(L);
		if (vargc == 0 || vargc > 3) {
			return luaL_error(L, "1 to 3 argument expected, got %d", vargc);
		}

		if (lua_type(L, 1) != LUA_TSTRING) {
			return luaL_typeerror(L, 1, "string");
		}

		if (vargc >= 2 && !lua_isboolean(L, 2)) {
			return luaL_typeerror(L, 2, "boolean");
		}

		if (vargc == 3 && !lua_isboolean(L, 3)) {
			return luaL_typeerror(L, 3, "boolean");
		}

		bool ltrim = vargc < 2 || lua_toboolean(L, 2);
		bool rtrim = vargc < 3 || lua_toboolean(L, 3);

		const char* front;
		const char* end;
		size_t size;

		front = lua_tolstring(L, 1, &size);
		end = &front[size - 1];

		if (ltrim) {
			while (size && isspace((unsigned char)*front)) {
				size--;
				front++;
			}
		}

		if (rtrim) {
			while (size && isspace((unsigned char)*end)) {
				size--;
				end--;
			}
		}

		lua_pushlstring(L, front, size);
		return 1;
	}

	void register_string(lua_State* L) {
		const struct luaL_Reg funcs[] = {
			{ "trim", trim },
			{ NULL, NULL }
		};

		lua_pushliteral(L, "string");
		lua_newtable(L);
		lua_pushfuncs(L, funcs);
		lua_rawset(L, -3);
	}
}

int LUA_MODULE_LUAOPEN(lua_State* L) {
	// ================================================================
	// to avoid multiple registration, which cause class metatables to not be found,
	// make the call from c equivalent to require(modname)
	// ================================================================
	lua_getglobal(L, "package"); // get package
	if (lua_isnil(L, -1)) {
		luaL_error(L, "global variable 'package' wast not found");
	}

	lua_getfield(L, -1, "loaded"); // get package.loaded
	if (lua_isnil(L, -1)) {
		luaL_error(L, "'package.loaded' was not found");
	}

	lua_getfield(L, -1, LUA_MODULE_NAME_STR); // get package.loaded[modname]

	if (lua_istable(L, -1)) {
		int ref = luaL_ref(L, LUA_REGISTRYINDEX);
		lua_pop(L, 2); // remove package.loaded, package
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
		luaL_unref(L, LUA_REGISTRYINDEX, ref);
		return 1;
	}

	// remove nil package.loaded[modname]
	lua_pop(L, 1);

	lua_pushliteral(L, LUA_MODULE_NAME_STR);
	lua_newtable(L); // create the table module
	lua_rawset(L, -3); // set package.loaded[modname]

	lua_pushliteral(L, LUA_MODULE_NAME_STR);
	lua_rawget(L, -2); // get package.loaded[modname]

	int ref = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_pop(L, 2); // remove package.loaded, package
	lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
	luaL_unref(L, LUA_REGISTRYINDEX, ref);
	// ================================================================

	using namespace LUA_MODULE_NAME;

	register_Common(L);

	register_version(L);
	register_bit(L);
	register_math(L);
	register_string(L);
	register_all(L);
	register_extensions(L);

	return 1;
}
