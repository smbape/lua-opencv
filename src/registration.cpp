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
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setfield(L, -3, "bit");
		luaopen_bit(L);
		lua_pop(L, 1);
	}

	int _round(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc == 1) {
			bool is_valid;
			auto num = lua_to(L, 1, static_cast<double*>(nullptr), is_valid);
			if (!is_valid) {
				return luaL_typeerror(L, 1, "number");
			}
			return lua_push(L, std::round(num));
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
			return lua_push(L, std::round(num * mult) / mult);
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
			return lua_push(L, static_cast<int>(num));
		}

		return luaL_error(L, "1 argument expected, got %d", vargc);
	}

	const struct luaL_Reg funcs_math[] = {
		{ "round", _round },
		{ "int", _int },
		{ NULL, NULL }
	};

	void register_math(lua_State* L) {
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setfield(L, -3, "math");
		lua_pushfuncs(L, funcs_math);
		lua_pop(L, 1);
	}

	const struct luaL_Reg no_funcs[] = {
		{ NULL, NULL }
	};
}

#define _stringify(s) #s
#define stringify(s) _stringify(s)

int LUA_MODULE_LUAOPEN(lua_State* L) {
#if LUA_VERSION_NUM < 502
	luaL_register(L, stringify(LUA_MODULE_NAME), no_funcs);
#else
	luaL_newlib(L, no_funcs);
#endif

	using namespace LUA_MODULE_NAME;

	init_global_state(L);

	register_version(L);
	register_bit(L);
	register_math(L);
	register_all(L);
	register_extensions(L);

	return 1;
}
