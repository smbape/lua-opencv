#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <luadef.hpp>

namespace LUA_MODULE_NAME {

    struct test {
        int value;

        test() = default;
        test(int val) : value(val) {
        }
    };

} // namespace LUA_MODULE_NAME

// this function needs to be exported from your dll.
// see luadef.hpp for details
LUAAPI(int) LUA_MODULE_LUAOPEN(lua_State* L);
