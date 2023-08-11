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
		return shared_ptr(shared_ptr{}, &element);
	}

	template <typename _Tp, typename shared_ptr = std::shared_ptr<_Tp>>
	decltype(auto) reference_internal(const _Tp& element, shared_ptr* ptr = static_cast<shared_ptr*>(nullptr)) {
		return shared_ptr(shared_ptr{}, const_cast<_Tp*>(&element));
	}

	template <typename T>
	struct is_usertype : std::integral_constant<bool, false> {};

	template <typename T>
	constexpr inline bool is_usertype_v = is_usertype<T>::value;

	template<typename _To, typename T>
	bool object_is_impl(const _To& obj, T*) {
		return is_usertype_v<T> && obj.template is<std::shared_ptr<T>>() || obj.template is<T>();
	}

	template<typename _To, typename T>
	bool object_is_impl(const _To& obj, std::vector<T>*) {
		// if (obj.template is<std::vector<T>>()) {
		// 	return true;
		// }

		if (obj.get_type() == sol::type::userdata) {
			return obj.template is<std::vector<T>>();
		}

		if (!obj.template is<sol::table>()) {
			return false;
		}

		auto obj_table = obj.template as<sol::table>();

		auto i = 1;
		for (const auto& key_value_pair : obj_table) {
			sol::object key = key_value_pair.first;
			if (!key.template is<int>() || key.template as<int>() != i++) {
				return false;
			}

			sol::object value = key_value_pair.second;
			if (!object_is_impl(value, static_cast<T*>(nullptr))) {
				return false;
			}
		}

		return true;
	}

	template<typename T, typename _To = sol::object>
	bool object_is(const _To& obj) {
		return object_is_impl(obj, static_cast<T*>(nullptr));
	}

	template<typename _To, typename T>
	decltype(auto) object_as_impl(_To& obj, T*) {
		if constexpr (is_usertype_v<T>) {
			if (obj.template is<std::shared_ptr<T>>()) {
				return *obj.template as<std::shared_ptr<T>>();
			}
		}
		return obj.template as<T>();
	}

	template<typename _To, typename T>
	decltype(auto) object_as_impl(const _To& obj, T*) {
		if constexpr (is_usertype_v<T>) {
			if (obj.template is<std::shared_ptr<T>>()) {
				return *obj.template as<std::shared_ptr<T>>();
			}
		}
		return obj.template as<T>();
	}

	template<typename _To, typename T>
	decltype(auto) object_as_impl(_To& obj, std::vector<T>*) {
		// if (obj.template is<std::vector<T>>()) {
		// 	return obj.template as<std::vector<T>>();
		// }

		if (obj.get_type() == sol::type::userdata) {
			return obj.template as<std::vector<T>>();
		}

		std::vector<T> res;
		auto obj_table = obj.template as<sol::table>();

		auto i = 1;
		for (const auto& key_value_pair : obj_table) {
			sol::object value = key_value_pair.second;
			res.push_back(object_as_impl(value, static_cast<T*>(nullptr)));
		}

		return res;
	}

	template<typename _To, typename T>
	decltype(auto) object_as_impl(const _To& obj, std::vector<T>*) {
		// if (obj.template is<std::vector<T>>()) {
		// 	return obj.template as<std::vector<T>>();
		// }

		if (obj.get_type() == sol::type::userdata) {
			return obj.template as<std::vector<T>>();
		}

		std::vector<T> res;
		auto obj_table = obj.template as<sol::table>();

		auto i = 1;
		for (const auto& key_value_pair : obj_table) {
			sol::object value = key_value_pair.second;
			res.push_back(object_as_impl(value, static_cast<T*>(nullptr)));
		}

		return res;
	}

	template<typename T, typename _To = sol::object>
	decltype(auto) object_as(_To& obj) {
		return object_as_impl(obj, static_cast<T*>(nullptr));
	}

	template<typename T, typename _To = sol::object>
	decltype(auto) object_as(const _To& obj) {
		return object_as_impl(obj, static_cast<T*>(nullptr));
	}
}

#include <lua_utils_extension.hpp>
