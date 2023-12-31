#pragma once

#include <luadef.hpp>
#include <NamedParameters.hpp>

namespace LUA_MODULE_NAME {
	// ================================
	// is_usertype generics
	// ================================

	template<typename T>
	struct is_usertype : std::integral_constant<bool, false> {};

	template<typename T>
	constexpr inline bool is_usertype_v = is_usertype<T>::value;

	template <>
	struct is_usertype<NamedParameters> : std::true_type { };


	// ================================
	// reference_internal generics
	// ================================

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


	// ================================
	// maybe_impl generics
	// ================================

	template<typename _To, typename T>
	inline auto maybe_impl(_To& obj, T*) {
		if constexpr (is_usertype_v<T>) {
			if (obj.template is<std::shared_ptr<T>>()) {
				return obj.template as<std::shared_ptr<T>>();
			}

			if (obj.template is<T&>()) {
				return reference_internal(obj.template as<T&>());
			}

			return std::shared_ptr<T>();
		} else {
			auto maybe_t = obj.template as<sol::optional<T>>();
			if (maybe_t) {
				return std::make_shared<T>(*maybe_t);
			}
			return std::shared_ptr<T>();
		}
	}

	template<typename _To, typename T>
	inline auto maybe_impl(const _To& obj, T* ptr) {
		return maybe_impl(const_cast<_To&>(obj), ptr);
	}

	template<typename _To>
	inline auto maybe_impl(_To& obj, std::string*) {
		using T = std::string;

		if (obj == sol::lua_nil) {
			return std::make_shared<T>();
		}

		auto maybe_t = obj.template as<sol::optional<T>>();
		if (maybe_t) {
			return std::make_shared<T>(*maybe_t);
		}
		return std::shared_ptr<T>();
	}

	template<typename _To>
	inline auto maybe_impl(const _To& obj, std::string* ptr) {
		return maybe_impl(const_cast<_To&>(obj), ptr);
	}

	template<typename _To, typename T>
	inline auto maybe_impl(_To& obj, std::vector<T>*);

	template<typename _To, typename T>
	inline auto maybe_impl(const _To& obj, std::vector<T>* ptr);

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

namespace LUA_MODULE_NAME {
    template<typename _To, typename T>
    inline auto maybe_impl(_To& obj, std::vector<T>*) {
        if (obj.get_type() == sol::type::userdata) {
            auto maybe_t = obj.template as<sol::optional<std::vector<T>&>>();
            if (maybe_t) {
                return reference_internal(*maybe_t);
            }
            return std::shared_ptr<std::vector<T>>();
        }

        static std::shared_ptr<std::vector<T>> empty;

        auto maybe_table = obj.template as<sol::optional<sol::table>&>();
        if (!maybe_table) {
            return empty;
        }

        auto& obj_table = *maybe_table;

        // https://github.com/ThePhD/sol2/issues/1048
        // .size() is a O(logn) operation: lua does not store the size of a table in any property, and it's basically
        // doing fancy counting of nodes. That's why it must be hoisted out of the loop.
        auto size = obj_table.size();

        std::shared_ptr<std::vector<T>> res(new std::vector<T>(size));

        for (auto i = 0; i < size; ++i) {
            auto value = obj_table.template get<sol::object>(i + 1);
            auto elem = maybe_impl(value, static_cast<T*>(nullptr));
            if (!elem) {
                return empty; 
            }
            (*res)[i] = *elem;
        }

        return res;
    }

    template<typename _To, typename T>
    inline auto maybe_impl(const _To& obj, std::vector<T>* ptr) {
        return maybe_impl(const_cast<_To&>(obj), ptr);
    }
}
