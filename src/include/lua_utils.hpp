#pragma once

#include <luadef.hpp>
#include <NamedParameters.hpp>
#include <lua_utils_extension.hpp>

namespace LUA_MODULE_NAME {
    template<typename T>
    bool object_is_impl(const sol::object& obj, T*) {
        return obj.is<T>();
    }

    template<typename T>
    bool object_is_impl(sol::object& obj, T*) {
        return obj.is<T>();
    }

    template<typename T>
    bool object_is(const sol::object& obj) {
        return object_is_impl(obj, static_cast<T*>(nullptr) );
    }

    template<typename T>
    bool object_is(sol::object& obj) {
        return object_is_impl(obj, static_cast<T*>(nullptr) );
    }

    template<typename T>
    decltype(auto) object_as_impl(const sol::object& obj, T*) {
        return obj.as<T>();
    }

    template<typename T>
    decltype(auto) object_as_impl(sol::object& obj, T*) {
        return obj.as<T>();
    }

    template<typename T>
    decltype(auto) object_as(const sol::object& obj) {
        return object_as_impl(obj, static_cast<T*>(nullptr));
    }

    template<typename T>
    decltype(auto) object_as(sol::object& obj) {
        return object_as_impl(obj, static_cast<T*>(nullptr));
    }

    template<typename T>
    bool vargs_is(sol::variadic_args& args, int index, bool optional = false) {
        if (index >= args.size()) {
            return optional;
        }
        return object_is_impl(args.get<sol::object>(index), static_cast<T*>(nullptr) );
    }

    template<typename T>
    decltype(auto) vargs_get(sol::variadic_args& args, int index) {
        return object_as_impl(args.get<sol::object>(index), static_cast<T*>(nullptr) );
    }
}
