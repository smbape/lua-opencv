#pragma once

#include <luadef.hpp>
#include <NamedParameters.hpp>

namespace LUA_MODULE_NAME {

	template<typename _Tp, typename shared_ptr = std::shared_ptr<_Tp>>
	inline decltype(auto) reference_internal(_Tp* element, shared_ptr* ptr = static_cast<shared_ptr*>(nullptr)) {
		return shared_ptr(shared_ptr{}, element);
	}

	template<typename _Tp, typename shared_ptr = std::shared_ptr<_Tp>>
	inline decltype(auto) reference_internal(const _Tp* element, shared_ptr* ptr = static_cast<shared_ptr*>(nullptr)) {
		return shared_ptr(shared_ptr{}, const_cast<_Tp*>(element));
	}

	template<typename _Tp, typename shared_ptr = std::shared_ptr<_Tp>>
	inline decltype(auto) reference_internal(_Tp& element, shared_ptr* ptr = static_cast<shared_ptr*>(nullptr)) {
		return shared_ptr(shared_ptr{}, &element);
	}

	template<typename _Tp, typename shared_ptr = std::shared_ptr<_Tp>>
	inline decltype(auto) reference_internal(const _Tp& element, shared_ptr* ptr = static_cast<shared_ptr*>(nullptr)) {
		return shared_ptr(shared_ptr{}, const_cast<_Tp*>(&element));
	}

	template<typename T>
	struct is_usertype : std::integral_constant<bool, false> {};

	template<typename T>
	constexpr inline bool is_usertype_v = is_usertype<T>::value;

    template <>
    struct is_usertype<NamedParameters> : std::true_type { };

	template<typename _To, typename T>
	inline auto maybe_impl(_To& obj, T*) {
		if constexpr (is_usertype_v<T>) {
			if (obj.template is<std::shared_ptr<T>>()) {
				return obj.template as<std::shared_ptr<T>>();
			}

			if (obj.template is<T>()) {
				return reference_internal(obj.template as<T>());
			}

			return std::shared_ptr<T>();
		} else {
			return obj.template as<sol::optional<T>>();
		}
	}

	template<typename _To, typename T>
	inline auto maybe_impl(const _To& obj, T* ptr) {
		return maybe_impl(const_cast<_To&>(obj), ptr);
	}

	template<typename _To, typename T>
	inline decltype(auto) maybe_impl(_To& obj, std::vector<T>*) {
		if (obj.get_type() == sol::type::userdata) {
			return obj.template as<sol::optional<std::vector<T>>>();
		}

		static auto empty = sol::optional<std::vector<T>>();

		auto maybe_table = obj.template as<sol::optional<sol::table>>();
		if (!maybe_table) {
			return empty;
		}

		sol::optional<std::vector<T>> res(sol::in_place, std::move(std::vector<T>()));

		auto i = 1;
		for (const auto& key_value_pair : *maybe_table) {
			sol::object value = key_value_pair.second;
			auto elem = maybe_impl(value, static_cast<T*>(nullptr));
			if (!elem) {
				return empty; 
			}
			(*res).push_back(*elem);
		}

		return res;
	}

	template<typename _To, typename T>
	inline decltype(auto) maybe_impl(const _To& obj, std::vector<T>* ptr) {
		return maybe_impl(const_cast<_To&>(obj), ptr);
	}

	template<typename T, typename _To = sol::object>
	inline decltype(auto) maybe(_To& obj) {
		return maybe_impl(obj, static_cast<T*>(nullptr));
	}

	template<typename T, typename _To = sol::object>
	inline decltype(auto) maybe(const _To& obj) {
		return maybe_impl(obj, static_cast<T*>(nullptr));
	}
}

#include <lua_utils_extension.hpp>
