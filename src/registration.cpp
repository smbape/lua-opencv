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
#ifdef LUA_BITLIBNAME
		lua_pushliteral(L, "bit");
		lua_getglobal(L, "bit");
		lua_rawset(L, -3);
#else
		lua_pushliteral(L, "bit");
		lua_newtable(L);
		luaopen_bit(L);
		lua_rawset(L, -3);
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
}

#define _stringify(s) #s
#define stringify(s) _stringify(s)

int LUA_MODULE_LUAOPEN(lua_State* L) {
#if LUA_VERSION_NUM < 502
	const struct luaL_Reg no_funcs[] = {
		{ NULL, NULL }
	};
	luaL_register(L, stringify(LUA_MODULE_NAME), no_funcs);
#else
	lua_newtable(L);
#endif

	using namespace LUA_MODULE_NAME;

	register_Common(L);

	register_version(L);
	register_bit(L);
	register_math(L);
	register_all(L);
	register_extensions(L);

	return 1;
}
