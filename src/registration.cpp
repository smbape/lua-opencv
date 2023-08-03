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

        module.new_usertype<NamedParameters>("kwargs",
            sol::constructors<NamedParameters(), NamedParameters(NamedParameters::Table)>()
        );

        // regitster_my_object(lua, module);
        register_all(lua, module);

        lua_module_call_registered(lua, module);

        // https://sol2.readthedocs.io/en/latest/api/readonly.html#id1
        sol::table proxy = lua.create_table();

        // Properly self-index metatable to block things
        module[sol::meta_function::new_index] = deny_new_index;
        module[sol::meta_function::index] = module;

        // Set it on the actual table
        proxy[sol::metatable_key] = module;

        return proxy;
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
