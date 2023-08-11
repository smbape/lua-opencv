#include <my_object/my_object.hpp>
#include <register_all.hpp>

namespace {
}

namespace LUA_MODULE_NAME {
	std::vector<std::function<void(sol::state_view&, sol::table&)>>& lua_module_get_functions() {
		static std::vector<std::function<void(sol::state_view&, sol::table&)>> functions;
		return functions;
	}

	void lua_module_call_registered(sol::state_view& lua, sol::table& module) {
		auto& functions = lua_module_get_functions();
		for(auto && fn : functions) {
			fn(lua, module);
		}
	}

	sol::table LUA_MODULE_OPEN(sol::this_state L) {
		sol::state_view lua(L);
		sol::table module = lua.create_table();

		auto kwargs = module.new_usertype<NamedParameters>("kwargs", sol::call_constructor,
			sol::constructors<NamedParameters(), NamedParameters(NamedParameters::Table)>()
		);

		kwargs.set_function("kwargs", [] (NamedParameters* self, const std::string& key) {
			return self->count(key) != 0;
		});

		kwargs.set_function("get", [] (NamedParameters* self, const std::string& key) {
			return self->at(key);
		});

		kwargs.set_function("size", [] (NamedParameters* self) {
			return self->size();
		});

		kwargs.set_function("is_instance", [] (sol::object obj) {
			return obj.is<NamedParameters>();
		});

		// regitster_my_object(lua, module);
		register_all(lua, module);

		lua_module_call_registered(lua, module);

		return module;
	}

	int deny_new_index(lua_State* L) {
		return luaL_error(L, "Hacking is good. Rebuild it yourself without this protection!");
	}
}

int LUA_MODULE_LUAOPEN(lua_State* L) {
	// pass the lua_State,
	// the index to start grabbing arguments from,
	// and the function itself
	// optionally, you can pass extra arguments to the function
	// if that's necessary, but that's advanced usage and is
	// generally reserved for internals only
	return sol::stack::call_lua(
		 L, 1, LUA_MODULE_NAME::LUA_MODULE_OPEN);
}
