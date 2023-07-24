#pragma once

#include <registration.hpp>

namespace LUA_MODULE_NAME {
    void regitster_my_object(sol::table& module);

    struct test {
        int value;

        test() = default;
        test(int val) : value(val) {
        }
    };

} // namespace LUA_MODULE_NAME
