#pragma once

#include <luadef.hpp>
#include <NamedParameters.hpp>
#include <lua_utils_extension.hpp>

namespace LUA_MODULE_NAME {

    template <typename _Tp>
    const std::shared_ptr<typename _Tp> reference_internal(_Tp* element) {
        return std::shared_ptr<_Tp>(std::shared_ptr<_Tp>{}, element);
    }

    template <typename _Tp>
    const std::shared_ptr<typename _Tp> reference_internal(const _Tp* element) {
        return std::shared_ptr<_Tp>(std::shared_ptr<_Tp>{}, const_cast<_Tp*>(element));
    }

    template <typename _Tp>
    const std::shared_ptr<typename _Tp> reference_internal(_Tp& element) {
        return std::shared_ptr<_Tp>(std::shared_ptr<_Tp>{}, & element);
    }

    template <typename _Tp>
    const std::shared_ptr<typename _Tp> reference_internal(const _Tp& element) {
        return std::shared_ptr<_Tp>(std::shared_ptr<_Tp>{}, const_cast<_Tp*>(&element));
    }

    template<typename _To, typename T>
    bool object_is_impl(const _To& obj, T*) {
        return obj.is<T>();
    }

    template<typename _To, typename T>
    bool object_is_impl(_To& obj, T*) {
        return obj.is<T>();
    }

    template<typename T, typename _To = sol::object>
    bool object_is(const _To& obj) {
        return object_is_impl(obj, static_cast<T*>(nullptr) );
    }

    template<typename T, typename _To = sol::object>
    bool object_is(_To& obj) {
        return object_is_impl(obj, static_cast<T*>(nullptr) );
    }

    template<typename _To, typename T>
    decltype(auto) object_as_impl(const _To& obj, T*) {
        return obj.as<T>();
    }

    template<typename _To, typename T>
    decltype(auto) object_as_impl(_To& obj, T*) {
        return obj.as<T>();
    }

    template<typename T, typename _To = sol::object>
    decltype(auto) object_as(const _To& obj) {
        return object_as_impl(obj, static_cast<T*>(nullptr));
    }

    template<typename T, typename _To = sol::object>
    decltype(auto) object_as(_To& obj) {
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
