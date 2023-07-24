#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <luadef.hpp>

#include <functional>
#include <iostream>
#include <vector>

namespace LUA_MODULE_NAME {
    // https://stackoverflow.com/a/15128524
    // https://stackoverflow.com/questions/15128444/c-calling-a-function-from-a-vector-of-function-pointers-inside-a-class-where-t/15128524#15128524

    std::vector<std::function<void(sol::table&)>>& lua_module_get_functions();

    void lua_module_call_registered(sol::table& module);

    template<typename Function>
    void lua_module_register(Function && fn) {
        lua_module_get_functions().push_back(std::forward<Function>(fn));
    }

    int readonly(lua_State* L);
}

// This function needs to be exported.
// See luadef.hpp for details.
LUAAPI(int) LUA_MODULE_LUAOPEN(lua_State* L);
