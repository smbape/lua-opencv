#pragma once

#include <luadef.hpp>
#include <NamedParameters.hpp>

namespace LUA_MODULE_NAME {

	template <typename _Tp, typename shared_ptr = std::shared_ptr<_Tp>>
	decltype(auto) reference_internal(_Tp* element, shared_ptr* ptr = static_cast<shared_ptr*>(nullptr)) {
		return shared_ptr(shared_ptr{}, element);
	}

	template <typename _Tp, typename shared_ptr = std::shared_ptr<_Tp>>
	decltype(auto) reference_internal(const _Tp* element, shared_ptr* ptr = static_cast<shared_ptr*>(nullptr)) {
		return shared_ptr(shared_ptr{}, const_cast<_Tp*>(element));
	}

	template <typename _Tp, typename shared_ptr = std::shared_ptr<_Tp>>
	decltype(auto) reference_internal(_Tp& element, shared_ptr* ptr = static_cast<shared_ptr*>(nullptr)) {
		return shared_ptr(shared_ptr{}, & element);
	}

	template <typename _Tp, typename shared_ptr = std::shared_ptr<_Tp>>
	decltype(auto) reference_internal(const _Tp& element, shared_ptr* ptr = static_cast<shared_ptr*>(nullptr)) {
		return shared_ptr(shared_ptr{}, const_cast<_Tp*>(&element));
	}

	template<typename _To, typename T>
	bool object_is_impl(const _To& obj, T*) {
		return obj.template is<T>();
	}

	template<typename _To, typename T>
	bool object_is_impl(_To& obj, T*) {
		return obj.template is<T>();
	}

	template<typename T, typename _To = sol::object>
	bool object_is(const _To& obj) {
		return object_is_impl(obj, static_cast<T*>(nullptr));
	}

	template<typename T, typename _To = sol::object>
	bool object_is(_To& obj) {
		return object_is_impl(obj, static_cast<T*>(nullptr));
	}

	template<typename _To, typename T>
	decltype(auto) object_as_impl(const _To& obj, T*) {
		return obj.template as<T>();
	}

	template<typename _To, typename T>
	decltype(auto) object_as_impl(_To& obj, T*) {
		return obj.template as<T>();
	}

	template<typename T, typename _To = sol::object>
	decltype(auto) object_as(const _To& obj) {
		return object_as_impl(obj, static_cast<T*>(nullptr));
	}

	template<typename T, typename _To = sol::object>
	decltype(auto) object_as(_To& obj) {
		return object_as_impl(obj, static_cast<T*>(nullptr));
	}
}

#include <lua_utils_extension.hpp>
