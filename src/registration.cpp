#include <registration.hpp>
#include <bit.hpp>

namespace {
	using namespace LUA_MODULE_NAME;

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
			if (!lua_is(L, 1, static_cast<double*>(nullptr))) {
				return luaL_typeerror(L, 1, "number");
			}
			auto num = lua_to(L, 1, static_cast<double*>(nullptr));
			return lua_push(L, std::round(num));
		}

		if (vargc == 2) {
			if (!lua_is(L, 1, static_cast<double*>(nullptr))) {
				return luaL_typeerror(L, 1, "number");
			}

			if (!lua_is(L, 2, static_cast<int32_t*>(nullptr))) {
				return luaL_typeerror(L, 1, "integer");
			}

			auto num = lua_to(L, 1, static_cast<double*>(nullptr));
			auto n = lua_to(L, 2, static_cast<int32_t*>(nullptr));
			double mult = std::pow((double)10, (double)n);
			return lua_push(L, std::round(num * mult) / mult);
		}

		return luaL_error(L, "1 or 2 arguments expected, got %d", vargc);
	}

	int _int(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc == 1) {
			if (!lua_is(L, 1, static_cast<double*>(nullptr))) {
				return luaL_typeerror(L, 1, "number");
			}
			auto num = lua_to(L, 1, static_cast<double*>(nullptr));
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

	bool garbage_collecte_active = false;

	int is_call_garbage_collect(lua_State* L) {
		return lua_push(L, garbage_collecte_active);
	}

	int call_garbage_collect(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc == 1) {
			if (!lua_is(L, 1, static_cast<bool*>(nullptr))) {
				return luaL_typeerror(L, 1, "boolean");
			}
			auto value = lua_to(L, 1, static_cast<bool*>(nullptr));
			garbage_collecte_active = value;
			return 0;
		}

		return luaL_error(L, "1 argument expected, got %d", vargc);
	}

	const struct luaL_Reg funcs_garbage_collect[] = {
		{ "is_call_garbage_collect", is_call_garbage_collect },
		{ "call_garbage_collect", call_garbage_collect },
		{ NULL, NULL }
	};

	void register_garbage_collect(lua_State* L) {
		lua_pushfuncs(L, funcs_garbage_collect);
	}

	std::vector<std::tuple<Callback, void*>> registered_callbacks;

	int notifyCallbacks(lua_State* L) {
		std::lock_guard<std::mutex> lock(callback_mutex);
		for (const auto& [callback, userdata] : registered_callbacks) {
			callback(L, userdata);
		}
		return 0;
	}

	const struct luaL_Reg funcs_callbacks[] = {
		{ "notifyCallbacks",    notifyCallbacks },
		{ NULL, NULL }
	};

	void regiter_callbacks(lua_State* L) {
		lua_pushfuncs(L, funcs_callbacks);
	}

	const struct luaL_Reg no_funcs[] = {
		{ NULL, NULL }
	};
}

namespace LUA_MODULE_NAME {
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

	bool is_call_garbage_collect() {
		return garbage_collecte_active;
	}

	std::mutex callback_mutex;

	void registerCallback(Callback callback, void* userdata) {
		std::lock_guard<std::mutex> lock(callback_mutex);
		registered_callbacks.push_back(std::make_tuple(callback, userdata));
	}
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

	register_Keywords(L);
	register_bit(L);
	register_math(L);
	register_garbage_collect(L);
	regiter_callbacks(L);
	register_all(L);
	register_extensions(L);

	return 1;
}
