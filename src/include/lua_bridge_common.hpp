#pragma once

#include <lua_bridge_common.hdr.hpp>
#include <Keywords.hpp>

namespace LUA_MODULE_NAME {
	// ================================
	// is_usertype generics
	// ================================

	template<class T>
	struct is_usertype_pointer<T*> : is_usertype<T> {};

	template<class T>
	struct is_usertype_pointer<T* const> : is_usertype<T> {};

	template<class T>
	struct is_usertype_pointer<T* volatile> : is_usertype<T> {};

	template<class T>
	struct is_usertype_pointer<T* const volatile> : is_usertype<T> {};


	// ================================
	// lua_reference_push
	// ================================

	template<typename T>
	std::enable_if_t<is_usertype_v<T>> lua_reference_push(lua_State* L, T& value) {
		lua_push(L, reference_internal(&value));
	}

	template<typename T>
	std::enable_if_t<!is_usertype_v<T>&& is_instantiation_of_v<std::optional, T>> lua_reference_push(lua_State* L, T& value) {
		if (value.has_value()) {
			lua_reference_push(L, value.value());
		}
		else {
			lua_push(L, value);
		}
	}

	template<typename T>
	std::enable_if_t<!is_usertype_v<T> && !is_instantiation_of_v<std::optional, T>> lua_reference_push(lua_State* L, T& value) {
		lua_push(L, value);
	}

	template<typename T>
	std::enable_if_t<!std::is_reference_v<T>> lua_reference_push(lua_State* L, const T& value) {
		lua_push(L, value);
	}


	// ================================
	// extract_holder
	// ================================

	template<typename H, typename V>
	inline decltype(auto) extract_holder(H& holder, V*) {
		if constexpr (has_extract_holder_v<std::remove_cvref_t<H>, V>) {
			return extract_holder_info<std::remove_cvref_t<H>, V>::extract(holder);
		}
		else if constexpr (std::is_same_v<std::remove_cvref_t<H>, V>) {
			return holder;
		}
		else if constexpr (std::is_enum_v<V>) {
			return static_cast<V>(holder);
		}
		else {
			return *holder;
		}
	}


	// ================================
	// bool
	// ================================

	inline auto lua_to(lua_State* L, int index, bool*, bool& is_valid) {
		if constexpr (has_lua_to_custom_bridge_v<bool>) {
			const auto v = lua_to_custom_bridge<bool>::lua_to(L, index, is_valid);
			if (is_valid) {
				return v;
			}
		}

		is_valid = lua_isboolean(L, index);
		return is_valid && !!lua_toboolean(L, index);
	}


	// ================================
	// std::integral
	// ================================

	template<typename T>
	inline std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<bool, std::decay_t<T>>, std::decay_t<T>> lua_to(lua_State* L, int index, T* ptr, bool& is_valid) {
		using Integer = std::decay_t<T>;

		if constexpr (has_lua_to_custom_bridge_v<Integer>) {
			const auto v = lua_to_custom_bridge<Integer>::lua_to(L, index, is_valid);
			if (is_valid) {
				return v;
			}
		}

		// allow strings with only one character to be treated as char
		if constexpr (std::is_same_v<char, Integer>) {
			is_valid = lua_type(L, index) == LUA_TSTRING;
			if (is_valid) {
				size_t len;
				auto c_str = lua_tolstring(L, index, &len);
				if (len == 1) {
					return static_cast<Integer>(c_str[0]);
				}
			}
		}

#if (defined __bool_true_false_are_defined || defined __BOOL_TRUE_FALSE_ARE_DEFINED)
		is_valid = lua_isboolean(L, index);
		if (is_valid) {
			return static_cast<Integer>(lua_toboolean(L, index) ? 1 : 0);
		}
#endif

		is_valid = lua_type(L, index) == LUA_TNUMBER;
		if (!is_valid) {
			return static_cast<Integer>(0);
		}

		const lua_Number v = lua_tonumber(L, index);
		// remove limits checking to be consistent with luajit ffi
		// is_valid = v >= std::numeric_limits<Integer>::min() && v <= std::numeric_limits<Integer>::max();
		return static_cast<Integer>(v);
	}


	// ================================
	// std::floating_point
	// ================================

	template<typename T>
	inline std::enable_if_t<std::is_floating_point_v<T>, std::decay_t<T>> lua_to(lua_State* L, int index, T* ptr, bool& is_valid) {
		using Float = std::decay_t<T>;

		if constexpr (has_lua_to_custom_bridge_v<Float>) {
			const auto v = lua_to_custom_bridge<Float>::lua_to(L, index, is_valid);
			if (is_valid) {
				return v;
			}
		}

		is_valid = lua_type(L, index) == LUA_TNUMBER;
		if (!is_valid) {
			return static_cast<Float>(0);
		}
		return static_cast<Float>(lua_tonumber(L, index));
	}


	// ================================
	// const char*
	// ================================

	inline const char* lua_to(lua_State* L, int index, const char**, bool& is_valid) {
		if constexpr (has_lua_to_custom_bridge_v<const char*>) {
			const auto v = lua_to_custom_bridge<const char*>::lua_to(L, index, is_valid);
			if (is_valid) {
				return v;
			}
		}

		is_valid = lua_isnil(L, index);
		if (is_valid) {
			return nullptr;
		}

		is_valid = lua_islightuserdata(L, index);
		if (is_valid) {
			return static_cast<const char*>(lua_touserdata(L, index));
		}

		is_valid = lua_type(L, index) == LUA_TSTRING;
		if (!is_valid) {
			return nullptr;
		}

		size_t len;
		auto c_str = lua_tolstring(L, index, &len);
		return c_str;
	}


	// ================================
	// std::string
	// ================================

	inline std::string lua_to(lua_State* L, int index, std::string*, bool& is_valid) {
		if constexpr (has_lua_to_custom_bridge_v<std::string>) {
			const auto v = lua_to_custom_bridge<std::string>::lua_to(L, index, is_valid);
			if (is_valid) {
				return v;
			}
		}

		is_valid = lua_isnil(L, index);
		if (is_valid) {
			return std::string();
		}

		is_valid = lua_type(L, index) == LUA_TSTRING;
		if (!is_valid) {
			return "";
		}

		size_t len;
		auto c_str = lua_tolstring(L, index, &len);
		return std::string(c_str, len);
	}

#ifdef _MSC_VER
	inline std::wstring lua_to(lua_State* L, int index, std::wstring*, bool& is_valid) {
		if constexpr (has_lua_to_custom_bridge_v<std::wstring>) {
			const auto v = lua_to_custom_bridge<std::wstring>::lua_to(L, index, is_valid);
			if (is_valid) {
				return v;
			}
		}

		is_valid = lua_isnil(L, index);
		if (is_valid) {
			return std::wstring();
		}

		is_valid = lua_type(L, index) == LUA_TSTRING;
		if (!is_valid) {
			return L"";
		}

		size_t len;
		auto c_str = lua_tolstring(L, index, &len);
		std::wstring wstr; wide_char::utf8_to_wcs(c_str, len, wstr);
		return wstr;
	}
#endif


	// ================================
	// void*
	// ================================

	inline void* lua_self_pointer(lua_State* L, int index, bool& is_valid) {
		void* ptr = nullptr;

		if (index < 0) {
			index += lua_gettop(L) + 1;
		}

		is_valid = (lua_isuserdata(L, index) || lua_istable(L, index)) && lua_getmetatable(L, index); // push metatable
		if (!is_valid) {
			return ptr;
		}

		lua_pushliteral(L, "__has_self");
		lua_rawget(L, -2); // push metatable.__has_self
		is_valid = lua_isboolean(L, -1) && lua_toboolean(L, -1);
		lua_pop(L, 2); // pop metatable.__has_self, metatable

		if (!is_valid) {
			return ptr;
		}

		lua_pushliteral(L, "__self");
		lua_gettable(L, index); // push userdata.__self
		is_valid = lua_islightuserdata(L, -1);

		if (is_valid) {
			ptr = lua_touserdata(L, -1);
			is_valid = static_cast<bool>(ptr);
		}

		lua_pop(L, 1); // pop userdata.__self
		return ptr;
	}

	inline void* lua_to(lua_State* L, int index, void**, bool& is_valid) {
		if constexpr (has_lua_to_custom_bridge_v<void*>) {
			const auto ptr = lua_to_custom_bridge<void*>::lua_to(L, index, is_valid);
			if (is_valid) {
				return ptr;
			}
		}

		is_valid = lua_isnil(L, index);
		if (is_valid) {
			return nullptr;
		}

		is_valid = lua_type(L, index) == LUA_TSTRING;
		if (is_valid) {
			size_t len;
			auto c_str = lua_tolstring(L, index, &len);
			return const_cast<char*>(c_str);
		}

		is_valid = lua_islightuserdata(L, index);
		if (is_valid) {
			return lua_touserdata(L, index);
		}

		auto ptr = lua_self_pointer(L, index, is_valid);
		if (is_valid) {
			return ptr;
		}

		return nullptr;
	}


	// ================================
	// _Object
	// ================================

	template<typename T>
	PushGuard::PushGuard(lua_State* L, const T& any) : L(L) {
		lua_push(L, any);
	}

	template<int Kind>
	template<typename T>
	_Object<Kind>::_Object(lua_State* L, const T& any) {
		PushGuard guardian(L, any);
		init(L, -1);
	}

	template<int Kind>
	const bool _Object<Kind>::isnil() const {
		if (L == nullptr) {
			return true;
		}
		PushGuard guardian(L, *this);
		return lua_isnil(L, -1);
	}

	template<int Kind, typename T>
	inline decltype(auto) lua_to(const _Object<Kind>& o, T* ptr, bool& is_valid) {
		PushGuard guardian(o.L, o);
		return lua_to(o.L, -1, ptr, is_valid);
	}

	template<int Kind, typename T>
	inline decltype(auto) lua_to(const _Object<Kind>& o, T& out, bool& is_valid) {
		PushGuard guardian(o.L, o);
		return lua_to(o.L, -1, out, is_valid);
	}


	// ================================
	// templated: lua_to, lua_push
	// ================================

	template<std::size_t I, typename... _Ts>
	inline bool check_metatable(lua_State* L, int luaopen_index, const void* mt_pointer) {
		using _Tuple = typename std::tuple<_Ts...>;
		using T = std::tuple_element_t<I, _Tuple>;

		const auto& expected_pointer = usertype_metatable_pointer<T>(luaopen_index);
		if (mt_pointer == expected_pointer) {
			return true;
		}

		if constexpr (I + 1 != sizeof...(_Ts)) {
			return check_metatable<I + 1, _Ts...>(L, luaopen_index, mt_pointer);
		}
		else {
			return false;
		}
	}

	template<typename T, typename... _Ts>
	inline bool check_metatable(lua_State* L, int index) {
		if (!lua_getmetatable(L, index)) {
			return false;
		}

		const auto& mt_pointer = lua_topointer(L, -1);
		bool is_valid = check_metatable<0, T, _Ts...>(L, get_luaopen_index(L), mt_pointer);
		lua_pop(L, 1);
		return is_valid;
	}

	template<typename T, typename... _Ts>
	inline std::shared_ptr<T> testudata_metatable(lua_State* L, int index, bool& is_valid) {
		is_valid = lua_isuserdata(L, index) && check_metatable<T, _Ts...>(L, index);
		if (!is_valid) {
			return std::shared_ptr<T>();
		}
		return *static_cast<std::shared_ptr<T>*>(lua_touserdata(L, index));
	}

	template<typename T>
	inline void usertype_push_metatable(lua_State* L) {
		thread_local const auto ref = usertype_metatable_ref<T>(get_luaopen_index(L));
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
	}


	// ================================
	// T
	// ================================

	template<typename T>
	inline T* lua_to(lua_State* L, int index, T*, T*& ref, bool& is_valid) {
		if constexpr (has_lua_to_custom_bridge_v<void*>) {
			const auto ptr = lua_to_custom_bridge<void*>::lua_to(L, index, is_valid);
			if (is_valid) {
				ref = static_cast<T*>(ptr);
				return ref;
			}
		}

		is_valid = lua_isnil(L, index);
		if (is_valid) {
			ref = static_cast<T*>(nullptr);
			return ref;
		}

		is_valid = lua_islightuserdata(L, index);
		if (is_valid) {
			ref = static_cast<T*>(lua_touserdata(L, index));
			return ref;
		}

		auto holder_value = lua_to(L, index, static_cast<T*>(nullptr), is_valid);
		if (is_valid) {
			*ref = extract_holder(holder_value, static_cast<T*>(nullptr));
		}
		return ref;
	}

	template<typename T>
	inline std::shared_ptr<T> lua_userdata_to(lua_State* L, int index, T*, bool& is_valid) {
		if constexpr (is_usertype_v<T>) {
			return usertype_info<T>::lua_userdata_to(L, index, is_valid);
		}
		else {
			is_valid = false;
			return std::shared_ptr<T>();
		}
	}

	template<typename T>
	inline void lua_push(lua_State* L, T* ptr, void (*d)(T*)) {
		lua_push(L, std::make_shared<T>(ptr, CFunctionDeleter(d)));
	}


	// ================================
	// T[]
	// ================================

	template<typename T>
	void PointerArray<T>::register_class(lua_State* L) {
		thread_local bool registered = [L]() {
			bool registered;
			{
				std::unique_lock lock(usertype_info<PointerArray<T>>::mutex);
				const auto index = get_luaopen_index(L);
				const auto size = usertype_info<PointerArray<T>>::metatable_pointers.size();
				registered = index < size && usertype_info<PointerArray<T>>::metatable_refs.at(index) != LUA_REFNIL;
			}

			if (!registered) {
				lua_newtable(L);
				lua_register_class<PointerArray<T>>(L, internal::GetTypeName<PointerArray<T>>());
				lua_register_defaults<PointerArray<T>>(L);

				{
					lua_pushstring(L, internal::GetTypeName<PointerArray<T>>());
					lua_rawget(L, -2); // cls = module[name]

					// For ffi purpose
					lua_pushliteral(L, "__sizeof");
					lua_push(L, sizeof(T*));
					lua_rawset(L, -3);

					lua_pop(L, 1);
				}

				lua_pop(L, 1);
			}
			return true;
		}();
	}

	template<typename T>
	int PointerArray<T>::__index(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc == 2) {
			bool is_valid;
			auto self = lua_to(L, 1, static_cast<PointerArray<T>*>(nullptr), is_valid);
			if (!is_valid) {
				goto overload;
			}

			if (lua_type(L, 2) == LUA_TSTRING && std::strcmp(lua_tostring(L, 2), "__self") == 0) {
				lua_pushlightuserdata(L, self->data);
				return 1;
			}

			auto holder_index = lua_to(L, 2, static_cast<size_t*>(nullptr), is_valid);
			if (!is_valid) {
				goto overload;
			}

			decltype(auto) index = extract_holder(holder_index, static_cast<size_t*>(nullptr));

			if constexpr (is_usertype_v<T>) {
				lua_push(L, &self->operator[](index));
			}
			else {
				lua_push(L, self->operator[](index));
			}

			return lua_gettop(L) - vargc;
		}
	overload:

		auto ret = try_mt__index(L);
		if (ret != 0) {
			return lua_gettop(L) - vargc;
		}

		return lua_missing_declaration(L);
	}

	template<typename T>
	int PointerArray<T>::__newindex(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc < 2) {
			LUAL_MODULE_ERROR_RETURN(L, "index is undefined");
		}

		if (vargc < 3) {
			LUAL_MODULE_ERROR_RETURN(L, "new value is undefined");
		}

		if (vargc > 3) {
			LUAL_MODULE_ERROR_RETURN(L, "too many arguments");
		}

		bool is_valid;
		auto self = lua_to(L, 1, static_cast<PointerArray<T>*>(nullptr), is_valid);
		if (!is_valid) {
			return luaL_typeerror(L, 1, internal::GetTypeName<PointerArray<T>>());
		}

		auto holder_index = lua_to(L, 2, static_cast<size_t*>(nullptr), is_valid);
		if (!is_valid) {
			// set the value on the instance
			lua_pushvalue(L, 2); // push the key
			lua_pushvalue(L, 3); // push the value
			lua_rawset(L, 1);
			return lua_gettop(L) - vargc;
		}

		decltype(auto) index = extract_holder(holder_index, static_cast<size_t*>(nullptr));

		auto holder_value = lua_to(L, 3, static_cast<T*>(nullptr), is_valid);
		if (!is_valid) {
			return luaL_typeerror(L, 3, internal::GetTypeName<T>());
		}
		decltype(auto) value = extract_holder(holder_value, static_cast<T*>(nullptr));

		self->operator[](index) = value;
		return lua_gettop(L) - vargc;
	}

	template<typename T>
	int PointerArray<T>::__add(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc < 2) {
			LUAL_MODULE_ERROR_RETURN(L, "index is undefined");
		}

		if (vargc > 2) {
			LUAL_MODULE_ERROR_RETURN(L, "too many arguments");
		}

		bool is_valid;
		auto self = lua_to(L, 1, static_cast<PointerArray<T>*>(nullptr), is_valid);
		if (!is_valid) {
			return luaL_typeerror(L, 1, internal::GetTypeName<PointerArray<T>>());
		}

		auto holder_index = lua_to(L, 2, static_cast<std::ptrdiff_t*>(nullptr), is_valid);
		if (!is_valid) {
			return luaL_typeerror(L, 2, "std::ptrdiff_t");
		}
		decltype(auto) index = extract_holder(holder_index, static_cast<std::ptrdiff_t*>(nullptr));

		lua_push(L, std::make_shared<PointerArray<T>>(self->data + index));
		return lua_gettop(L) - vargc;
	}

	template<typename T>
	int PointerArray<T>::__sub(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc < 2) {
			LUAL_MODULE_ERROR_RETURN(L, "index is undefined");
		}

		if (vargc > 2) {
			LUAL_MODULE_ERROR_RETURN(L, "too many arguments");
		}

		bool is_valid;
		auto self = lua_to(L, 1, static_cast<PointerArray<T>*>(nullptr), is_valid);
		if (!is_valid) {
			return luaL_typeerror(L, 1, internal::GetTypeName<PointerArray<T>>());
		}

		{
			auto rhs = lua_to(L, 2, static_cast<PointerArray<T>*>(nullptr), is_valid);
			if (is_valid) {
				lua_push(L, static_cast<std::ptrdiff_t>(self->data - rhs->data));
				return lua_gettop(L) - vargc;
			}
		}

		auto holder_index = lua_to(L, 2, static_cast<std::ptrdiff_t*>(nullptr), is_valid);
		if (!is_valid) {
			return luaL_typeerror(L, 2, "std::ptrdiff_t");
		}
		decltype(auto) index = extract_holder(holder_index, static_cast<std::ptrdiff_t*>(nullptr));

		lua_push(L, std::make_shared<PointerArray<T>>(self->data - index));
		return lua_gettop(L) - vargc;
	}

	template<typename T>
	int PointerArray<T>::__lt(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc < 2) {
			LUAL_MODULE_ERROR_RETURN(L, "index is undefined");
		}

		if (vargc > 2) {
			LUAL_MODULE_ERROR_RETURN(L, "too many arguments");
		}

		bool is_valid;
		auto lhs = lua_to(L, 1, static_cast<PointerArray<T>*>(nullptr), is_valid);
		if (!is_valid) {
			return luaL_typeerror(L, 1, internal::GetTypeName<PointerArray<T>>());
		}

		auto rhs = lua_to(L, 2, static_cast<PointerArray<T>*>(nullptr), is_valid);
		if (!is_valid) {
			return luaL_typeerror(L, 2, internal::GetTypeName<PointerArray<T>>());
		}

		lua_pushboolean(L, lhs->data < rhs->data);
		return lua_gettop(L) - vargc;
	}

	template<typename T>
	int PointerArray<T>::__le(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc < 2) {
			LUAL_MODULE_ERROR_RETURN(L, "index is undefined");
		}

		if (vargc > 2) {
			LUAL_MODULE_ERROR_RETURN(L, "too many arguments");
		}

		bool is_valid;
		auto lhs = lua_to(L, 1, static_cast<PointerArray<T>*>(nullptr), is_valid);
		if (!is_valid) {
			return luaL_typeerror(L, 1, internal::GetTypeName<PointerArray<T>>());
		}

		auto rhs = lua_to(L, 2, static_cast<PointerArray<T>*>(nullptr), is_valid);
		if (!is_valid) {
			return luaL_typeerror(L, 2, internal::GetTypeName<PointerArray<T>>());
		}

		lua_pushboolean(L, lhs->data <= rhs->data);
		return lua_gettop(L) - vargc;
	}

	template<typename T>
	std::mutex usertype_info<PointerArray<T>>::mutex;

	template<typename T>
	std::vector<const void*> usertype_info<PointerArray<T>>::metatable_pointers;

	template<typename T>
	std::vector<int> usertype_info<PointerArray<T>>::metatable_refs;

	template<typename T>
	const struct luaL_Reg usertype_info<PointerArray<T>>::methods[] = {
		{"__index", PointerArray<T>::__index},
		{"__newindex", PointerArray<T>::__newindex},
		{"__add", PointerArray<T>::__add},
		{"__sub", PointerArray<T>::__sub},
		{"__lt", PointerArray<T>::__lt},
		{"__le", PointerArray<T>::__le},
		{NULL, NULL} // Sentinel
	};

	template<typename T>
	const struct luaL_Reg usertype_info<PointerArray<T>>::meta_methods[] = {
		{NULL, NULL} // Sentinel
	};

	template<typename T>
	std::shared_ptr<PointerArray<T>> usertype_info<PointerArray<T>>::lua_userdata_to(lua_State* L, int index, bool& is_valid) {
		PointerArray<T>::register_class(L);

		auto parray = testudata_metatable<PointerArray<T>>(L, index, is_valid);
		if (is_valid) {
			return parray;
		}

		{
			auto holder_value = lua_to(L, index, static_cast<void**>(nullptr), is_valid);
			if (is_valid) {
				decltype(auto) value = extract_holder(holder_value, static_cast<void**>(nullptr));
				return std::make_shared<PointerArray<T>>(reinterpret_cast<T*>(value));
			}
		}

		return parray;
	}


	// ================================
	// T if std::is_enum_v<T>
	// ================================

	template<typename T>
	inline std::enable_if_t<std::is_enum_v<T>, int> lua_to(lua_State* L, int index, T* ptr, bool& is_valid) {
		return lua_to(L, index, static_cast<int*>(nullptr), is_valid);
	}


	// ================================
	// T if is_usertype_v<T>
	// ================================

	template<typename T>
	inline std::enable_if_t<is_usertype_v<T>, std::shared_ptr<T>> lua_to(lua_State* L, int index, std::shared_ptr<T>*, bool& is_valid) {
		is_valid = lua_isnil(L, index);
		if (is_valid) {
			return std::shared_ptr<T>();
		}

		is_valid = lua_isuserdata(L, index);
		if (!is_valid) {
			return std::shared_ptr<T>();
		}

		return lua_userdata_to(L, index, static_cast<T*>(nullptr), is_valid);
	}

	template<typename T>
	inline std::enable_if_t<is_usertype_v<T>, std::shared_ptr<T>> lua_to(lua_State* L, int index, T* ptr, bool& is_valid) {
		if constexpr (requires(lua_State * L, const size_t __top__, bool& is_valid) { usertype_info<T>::Lua_new(L, __top__, is_valid); }) {
			thread_local std::vector<const void*> seen;

			auto pointer = lua_topointer(L, index);

			// avoid stack overflow when trying implicit conversion
			is_valid = std::find(seen.begin(), seen.end(), pointer) == seen.end();
			if (!is_valid) {
				return std::shared_ptr<T>();
			}
			seen.push_back(pointer);

			auto value = lua_userdata_to(L, index, ptr, is_valid);
			if (is_valid) {
				seen.resize(seen.size() - 1);
				return value;
			}

			// Try implicit conversion
			auto __top__ = lua_gettop(L);
			lua_pushvalue(L, index);
			value = usertype_info<T>::Lua_new(L, __top__, is_valid);
			lua_pop(L, 1);

			// Try as kwargs
			if (!is_valid && lua_newkwargs_from_table(L, index, is_valid)) {
				value = usertype_info<T>::Lua_new(L, __top__, is_valid);
				lua_pop(L, 1);
			}

			seen.resize(seen.size() - 1);
			return value;
		}
		else {
			return lua_userdata_to(L, index, ptr, is_valid);
		}
	}

	template<typename T>
	inline std::enable_if_t<is_usertype_v<T>, T*> lua_to(lua_State* L, int index, T**, bool& is_valid) {
		if constexpr (has_lua_to_custom_bridge_v<T*>) {
			const auto ptr = lua_to_custom_bridge<T*>::lua_to(L, index, is_valid);
			if (is_valid) {
				return ptr;
			}
		}

		is_valid = lua_isnil(L, index);
		if (is_valid) {
			return static_cast<T*>(nullptr);
		}

		is_valid = lua_islightuserdata(L, index);
		if (is_valid) {
			auto ptr = lua_touserdata(L, index);
			return static_cast<T*>(ptr);
		}

		auto userdata_ptr = lua_to(L, index, static_cast<T*>(nullptr), is_valid);
		if (is_valid) {
			return userdata_ptr.get();
		}

		return static_cast<T*>(nullptr);
	}

	template<typename T>
	inline std::enable_if_t<is_usertype_v<T>, void> lua_push(lua_State* L, T* ptr) {
		if (ptr) {
			lua_push(L, reference_internal(ptr));
		}
		else {
			lua_pushnil(L);
		}
	}

	template<typename T>
	inline std::enable_if_t<is_usertype_v<T>, void> lua_push(lua_State* L, const T* ptr) {
		lua_push(L, const_cast<T*>(ptr));
	}

	template<typename T>
	inline std::enable_if_t<is_usertype_v<T>, void> lua_push(lua_State* L, T&& obj) {
		lua_push(L, std::make_shared<T>(std::move(obj)));
	}

	template<typename T>
	inline std::enable_if_t<is_usertype_v<T>, void> lua_push(lua_State* L, const T& obj) {
		lua_push(L, std::make_shared<T>(obj));
	}


	// ================================
	// T if !is_usertype_v<remove_cvref_all_pointers_t<T>
	// ================================

	template<typename T>
	inline std::enable_if_t<!std::is_function_v<T> && !is_usertype_v<remove_cvref_all_pointers_t<T>> && !std::is_same_v<remove_cvref_all_pointers_t<T>, void>, T*> lua_to(lua_State* L, int index, T**, bool& is_valid) {
		auto ptr = lua_to(L, index, static_cast<void**>(nullptr), is_valid);
		return static_cast<T*>(is_valid ? ptr : nullptr);
	}

	// ================================
	// T** as void*
	// ================================

	template<typename T>
	inline T** lua_to(lua_State* L, int index, T***, bool& is_valid) {
		auto ptr = lua_to(L, index, static_cast<void**>(nullptr), is_valid);
		return is_valid ? static_cast<T**>(ptr) : nullptr;
	}


	// ================================
	// std::shared_ptr
	// ================================

	template<typename T>
	inline std::enable_if_t<!is_usertype_v<T>, std::shared_ptr<T>> lua_to(lua_State* L, int index, std::shared_ptr<T>*, bool& is_valid) {
		is_valid = lua_isnil(L, index);
		if (is_valid) {
			return std::shared_ptr<T>();
		}
		return lua_to(L, index, static_cast<T*>(nullptr), is_valid);
	}

	template<typename T>
	inline void lua_push(lua_State* L, const std::shared_ptr<T>& ptr) {
		if (!ptr) {
			lua_pushnil(L);
			return;
		}

		if constexpr (requires(lua_State * L) { T::register_class(L); }) {
			T::register_class(L);
		}

		if constexpr (requires(std::size_t type) { usertype_info<T>::derives_pushers.count(type); }) {
			// Downcasting
			if (auto search = usertype_info<T>::derives_pushers.find(typeid(*ptr).hash_code()); search != usertype_info<T>::derives_pushers.end()) {
				search->second(L, ptr);
				return;
			}
		}

		using SharedPtr = std::shared_ptr<T>;
		auto userdata_ptr = static_cast<SharedPtr*>(lua_newuserdata(L, sizeof(SharedPtr)));
		new(userdata_ptr) SharedPtr(ptr); // userdata = new std::shared_ptr<T>(ptr)

		usertype_push_metatable<T>(L);
		lua_setmetatable(L, -2);
	}


	// ================================
	// stl map container
	// ================================

	template<template<typename, typename, typename...> typename Container, typename K, typename V, typename... _Ts>
	inline void _stl_map_container_lua_to(lua_State* L, int index, Container<K, V, _Ts...>& out, bool& is_valid) {
		using Map = Container<K, V, _Ts...>;

		if (lua_isuserdata(L, index)) {
			out = *lua_userdata_to(L, index, static_cast<Map*>(nullptr), is_valid);
			return;
		}

		is_valid = lua_istable(L, index);
		if (!is_valid) {
			return;
		}

		if (index < 0) {
			index += lua_gettop(L) + 1;
		}

		out.clear();

		// https://www.lua.org/manual/5.1/manual.html#lua_next

		lua_pushnil(L);  /* first key */
		// stack now contains: -1 => nil

		while (lua_next(L, index) != 0) {
			// stack now contains: -2 => key; -1 => value
			const auto key = lua_to(L, -2, static_cast<K*>(nullptr), is_valid);

			if (!is_valid) {
				/* removes 'value'; keeps 'key' for the next iteration */
				lua_pop(L, 1);
				// stack now contains: -1 => key

				/* removes 'key'; break iteration */
				lua_pop(L, 1);
				// stack is now the same as it was on entry to this function

				break;
			}

			auto holder_value = lua_to(L, -1, static_cast<V*>(nullptr), is_valid);

			/* removes 'value'; keeps 'key' for the next iteration */
			lua_pop(L, 1);
			// stack now contains: -1 => key

			if (!is_valid) {
				/* removes 'key'; break iteration */
				lua_pop(L, 1);
				// stack is now the same as it was on entry to this function

				break;
			}

			decltype(auto) value = extract_holder(holder_value, static_cast<V*>(nullptr));
			out.insert_or_assign(key, value);
		}
	}

	template<template<typename, typename, typename...> typename Container, typename K, typename V, typename... _Ts>
	inline std::shared_ptr<Container<K, V, _Ts...>> _stl_map_container_lua_to(lua_State* L, int index, Container<K, V, _Ts...>* ptr, bool& is_valid) {
		if (lua_isuserdata(L, index)) {
			return lua_userdata_to(L, index, ptr, is_valid);
		}

		if (index < 0) {
			index += lua_gettop(L) + 1;
		}

		auto out = std::make_shared<Container<K, V, _Ts...>>();
		lua_to(L, index, *out, is_valid);
		return out;
	}

	template<template<typename, typename, typename...> typename Container, typename K, typename V, typename... _Ts>
	inline void _stl_map_container_lua_push(lua_State* L, Container<K, V, _Ts...>&& kv) {
		const Container<K, V, _Ts...> _kv(std::move(kv));
		_stl_map_container_lua_push(L, _kv);
	}

	template<template<typename, typename, typename...> typename Container, typename K, typename V, typename... _Ts>
	inline void _stl_map_container_lua_push(lua_State* L, const Container<K, V, _Ts...>& kv) {
		lua_newtable(L);
		for (const auto& [k, v] : kv) {
			lua_push(L, k);
			lua_push(L, v);
			lua_rawset(L, -3);
		}
	}


	// ================================
	// std::map
	// ================================

	template<typename K, typename V, typename... _Ts>
	inline void lua_to(lua_State* L, int index, std::map<K, V, _Ts...>& out, bool& is_valid) {
		_stl_map_container_lua_to(L, index, out, is_valid);
	}

	template<typename K, typename V, typename... _Ts>
	inline std::shared_ptr<std::map<K, V, _Ts...>> lua_to(lua_State* L, int index, std::map<K, V, _Ts...>* ptr, bool& is_valid) {
		return _stl_map_container_lua_to(L, index, ptr, is_valid);
	}

	template<typename K, typename V, typename... _Ts>
	inline void lua_push(lua_State* L, std::map<K, V, _Ts...>&& kv) {
		_stl_map_container_lua_push(L, std::move(kv));
	}

	template<typename K, typename V, typename... _Ts>
	inline void lua_push(lua_State* L, const std::map<K, V, _Ts...>& kv) {
		_stl_map_container_lua_push(L, kv);
	}


	// ================================
	// std::multimap
	// ================================

	template<typename K, typename V, typename... _Ts>
	inline void lua_to(lua_State* L, int index, std::multimap<K, V, _Ts...>& out, bool& is_valid) {
		_stl_map_container_lua_to(L, index, out, is_valid);
	}

	template<typename K, typename V, typename... _Ts>
	inline std::shared_ptr<std::multimap<K, V, _Ts...>> lua_to(lua_State* L, int index, std::multimap<K, V, _Ts...>* ptr, bool& is_valid) {
		return _stl_map_container_lua_to(L, index, ptr, is_valid);
	}

	template<typename K, typename V, typename... _Ts>
	inline void lua_push(lua_State* L, std::multimap<K, V, _Ts...>&& kv) {
		_stl_map_container_lua_push(L, std::move(kv));
	}

	template<typename K, typename V, typename... _Ts>
	inline void lua_push(lua_State* L, const std::multimap<K, V, _Ts...>& kv) {
		_stl_map_container_lua_push(L, kv);
	}


	// ================================
	// std::unordered_map
	// ================================

	template<typename K, typename V, typename... _Ts>
	inline void lua_to(lua_State* L, int index, std::unordered_map<K, V, _Ts...>& out, bool& is_valid) {
		_stl_map_container_lua_to(L, index, out, is_valid);
	}

	template<typename K, typename V, typename... _Ts>
	inline std::shared_ptr<std::unordered_map<K, V, _Ts...>> lua_to(lua_State* L, int index, std::unordered_map<K, V, _Ts...>* ptr, bool& is_valid) {
		return _stl_map_container_lua_to(L, index, ptr, is_valid);
	}

	template<typename K, typename V, typename... _Ts>
	inline void lua_push(lua_State* L, std::unordered_map<K, V, _Ts...>&& kv) {
		_stl_map_container_lua_push(L, std::move(kv));
	}

	template<typename K, typename V, typename... _Ts>
	inline void lua_push(lua_State* L, const std::unordered_map<K, V, _Ts...>& kv) {
		_stl_map_container_lua_push(L, kv);
	}


	// ================================
	// std::unordered_multimap
	// ================================

	template<typename K, typename V, typename... _Ts>
	inline void lua_to(lua_State* L, int index, std::unordered_multimap<K, V, _Ts...>& out, bool& is_valid) {
		_stl_map_container_lua_to(L, index, out, is_valid);
	}

	template<typename K, typename V, typename... _Ts>
	inline std::shared_ptr<std::unordered_multimap<K, V, _Ts...>> lua_to(lua_State* L, int index, std::unordered_multimap<K, V, _Ts...>* ptr, bool& is_valid) {
		return _stl_map_container_lua_to(L, index, ptr, is_valid);
	}

	template<typename K, typename V, typename... _Ts>
	inline void lua_push(lua_State* L, std::unordered_multimap<K, V, _Ts...>&& kv) {
		_stl_map_container_lua_push(L, std::move(kv));
	}

	template<typename K, typename V, typename... _Ts>
	inline void lua_push(lua_State* L, const std::unordered_multimap<K, V, _Ts...>& kv) {
		_stl_map_container_lua_push(L, kv);
	}


	// ================================
	// std::optional
	// ================================

	template<typename T>
	inline std::shared_ptr<std::optional<T>> lua_to(lua_State* L, int index, std::optional<T>*, bool& is_valid) {
		is_valid = index > lua_gettop(L) || lua_isnil(L, index);
		if (is_valid) {
			return std::make_shared<std::optional<T>>(std::nullopt);
		}

		auto holder_value = lua_to(L, index, static_cast<T*>(nullptr), is_valid);
		if (!is_valid) {
			return std::shared_ptr<std::optional<T>>();
		}

		auto ptr = std::make_shared<std::optional<T>>();
		ptr->emplace();
		**ptr = extract_holder(holder_value, static_cast<T*>(nullptr));

		return ptr;
	}

	template<typename T>
	inline void lua_push(lua_State* L, const std::optional<T>& p) {
		if (p) {
			lua_push(L, p.value());
		}
		else {
			lua_pushnil(L);
		}
	}


	// ================================
	// std::pair
	// ================================

	template<typename T1, typename T2>
	inline std::shared_ptr<std::pair<T1, T2>> lua_to(lua_State* L, int index, std::pair<T1, T2>*, bool& is_valid) {
		auto out = std::make_shared<std::pair<T1, T2>>();

		is_valid = lua_istable(L, index);
		if (!is_valid) {
			return out;
		}

		auto size = lua_rawlen(L, index);

		is_valid = size == 2;
		if (!is_valid) {
			return out;
		}

		if (index < 0) {
			index += lua_gettop(L) + 1;
		}

		for (auto i = 1; is_valid && i <= size; ++i) {
			lua_pushnumber(L, i);
			lua_rawget(L, index);
			if (i == 1) {
				auto value = lua_to(L, -1, static_cast<T1*>(nullptr), is_valid);
				if (is_valid) {
					out->first = extract_holder(value, static_cast<T1*>(nullptr));
				}
			}
			else {
				auto value = lua_to(L, -1, static_cast<T2*>(nullptr), is_valid);
				if (is_valid) {
					out->second = extract_holder(value, static_cast<T2*>(nullptr));
				}
			}
			lua_pop(L, 1);
		}

		return out;
	}

	template<typename T1, typename T2>
	inline void lua_push(lua_State* L, const std::pair<T1, T2>& p) {
		lua_newtable(L);
		int index = lua_gettop(L);

		lua_push(L, p.first);
		lua_rawset(L, 1);

		lua_push(L, p.second);
		lua_rawset(L, 2);
	}


	// ================================
	// std::tuple
	// ================================

	template<std::size_t I = 0, typename... _Ts>
	inline void _lua_to(lua_State* L, int index, std::tuple<_Ts...>& out, bool& is_valid) {
		using _Tuple = typename std::tuple<_Ts...>;
		using T = std::tuple_element_t<I, _Tuple>;

		lua_pushnumber(L, I + 1);
		lua_rawget(L, index);
		auto value = lua_to(L, -1, static_cast<T*>(nullptr), is_valid);
		lua_pop(L, 1);

		if (!is_valid) {
			return;
		}

		std::get<I>(out) = extract_holder(value, static_cast<T*>(nullptr));

		if constexpr (I != sizeof...(_Ts) - 1) {
			_lua_to<I + 1, _Ts...>(L, index, out, is_valid);
		}
	}

	template<typename... _Ts>
	inline std::shared_ptr<std::tuple<_Ts...>> lua_to(lua_State* L, int index, std::tuple<_Ts...>* ptr, bool& is_valid) {
		auto out = std::make_shared<std::tuple<_Ts...>>();

		is_valid = lua_istable(L, index);
		if (!is_valid) {
			return out;
		}

		auto size = lua_rawlen(L, index);

		is_valid = size == sizeof...(_Ts);
		if (!is_valid) {
			return out;
		}

		if (index < 0) {
			index += lua_gettop(L) + 1;
		}

		_lua_to(L, index, *out, is_valid);
		return out;
	}

	template<std::size_t I = 0, typename... _Ts>
	inline void _lua_push(lua_State* L, int index, const std::tuple<_Ts...>& value) {
		using _Tuple = typename std::tuple<_Ts...>;
		using T = std::tuple_element_t<I, _Tuple>;

		lua_push(L, std::get<I>(value));
		lua_rawseti(L, index, I + 1);

		if constexpr (I != sizeof...(_Ts) - 1) {
			_lua_push<I + 1, _Ts...>(L, index, value);
		}
	}

	template<typename... _Ts>
	inline void lua_push(lua_State* L, const std::tuple<_Ts...>& value) {
		lua_newtable(L);
		int index = lua_gettop(L);
		_lua_push(L, index, value);
	}


	// ================================
	// std::variant
	// ================================

	template<std::size_t I = 0, typename... _Ts>
	inline std::variant<_Ts...> _lua_to(lua_State* L, int index, std::variant<_Ts...>* ptr, bool& is_valid) {
		using Variant = typename std::variant<_Ts...>;
		using _Tuple = typename std::tuple<_Ts...>;
		using T = std::tuple_element_t<I, _Tuple>;

		if constexpr (!std::is_same_v<std::monostate, T>) {
			auto holder = lua_to(L, index, static_cast<T*>(nullptr), is_valid);
			if (is_valid) {
				return extract_holder(holder, static_cast<T*>(nullptr));
			}
		}

		if constexpr (I == sizeof...(_Ts) - 1) {
			is_valid = false;
			return Variant();
		}
		else {
			return _lua_to<I + 1, _Ts...>(L, index, ptr, is_valid);
		}
	}

	template<typename... _Ts>
	inline std::variant<_Ts...> lua_to(lua_State* L, int index, std::variant<_Ts...>* ptr, bool& is_valid) {
		return _lua_to(L, index, ptr, is_valid);
	}

	template<std::size_t I = 0, typename... _Ts>
	inline void _lua_push(lua_State* L, const std::variant<_Ts...>& value) {
		using _Tuple = typename std::tuple<_Ts...>;
		using T = std::tuple_element_t<I, _Tuple>;

		if constexpr (!std::is_same_v<std::monostate, T>) {
			if (std::holds_alternative<T>(value)) {
				lua_push(L, std::get<T>(value));
				return;
			}
		}

		if constexpr (I + 1 != sizeof...(_Ts)) {
			_lua_push<I + 1, _Ts...>(L, value);
		}
		else {
			lua_pushnil(L);
		}
	}

	template<typename... _Ts>
	inline void lua_push(lua_State* L, const std::variant<_Ts...>& value) {
		_lua_push(L, value);
	}


	// ================================
	// stl container
	// ================================

	template<template<typename...> typename Container, typename... _Ts>
	inline void _stl_container_lua_to(lua_State* L, int index, Container<_Ts...>& container, bool& is_valid, size_t len, bool loose) {
		using _Tuple = typename std::tuple<_Ts...>;
		using T = std::tuple_element_t<0, _Tuple>;

		if (lua_isuserdata(L, index)) {
			container = *lua_userdata_to(L, index, static_cast<Container<_Ts...>*>(nullptr), is_valid);
			if (is_valid) {
				const auto& size = container.size();
				is_valid = len == 0 || (loose ? size <= len : size == len);
			}
			return;
		}

		is_valid = lua_istable(L, index);
		if (!is_valid) {
			return;
		}

		if (index < 0) {
			index += lua_gettop(L) + 1;
		}

		auto size = lua_rawlen(L, index);

		is_valid = len == 0 || (loose ? size <= len : size == len);
		if (!is_valid) {
			return;
		}

		if constexpr (requires(Container<_Ts...>&container) { container.Clear(); }) {
			container.Clear();
		}
		else {
			container.clear();
		}

		if constexpr (requires(Container<_Ts...>&container, int new_size) { container.Reserve(new_size); }) {
			container.Reserve(size);
		}
		else {
			container.reserve(size);
		}

		for (auto i = 1; i <= size; ++i) {
			lua_pushnumber(L, i);
			lua_rawget(L, index);
			auto holder_value = lua_to(L, -1, static_cast<T*>(nullptr), is_valid);
			lua_pop(L, 1);

			if (!is_valid) {
				break;
			}

			decltype(auto) value = extract_holder(holder_value, static_cast<T*>(nullptr));

			if constexpr (requires(Container<_Ts...>&container, const T & value) { container.Add(value); }) {
				container.Add(value);
			}
			else if constexpr (requires(Container<_Ts...>&container, T && value) { container.Add(std::move(value)); }) {
				container.Add(std::move(value));
			}
			else {
				container.push_back(value);
			}
		}
	}

	template<template<typename...> typename Container, typename... _Ts>
	inline std::shared_ptr<Container<_Ts...>> _stl_container_lua_to(lua_State* L, int index, Container<_Ts...>* ptr, bool& is_valid, size_t len, bool loose) {
		if (lua_isuserdata(L, index)) {
			auto container = lua_userdata_to(L, index, ptr, is_valid);
			if (!is_valid) {
				return std::shared_ptr<Container<_Ts...>>();
			}

			const auto& size = container->size();
			is_valid = len == 0 || (loose ? size <= len : size == len);
			return container;
		}

		auto container = std::make_shared<Container<_Ts...>>();
		lua_to(L, index, *container, is_valid, len, loose);
		return container;
	}

	template<template<typename...> typename Container, typename... _Ts>
	inline void _stl_container_lua_push(lua_State* L, Container<_Ts...>&& container) {
		Container<_Ts...> _container(std::move(container));
		lua_push(L, _container);
	}

	template<template<typename...> typename Container, typename... _Ts>
	inline void _stl_container_lua_push(lua_State* L, const Container<_Ts...>& container) {
		lua_newtable(L);
		int index = lua_gettop(L);
		int i = 0;
		for (const auto& v : container) {
			lua_push(L, v);
			lua_rawseti(L, index, ++i);
		}
	}


	// ================================
	// std::vector
	// ================================

	template<class T, class Allocator>
	inline void lua_to(lua_State* L, int index, std::vector<T, Allocator>& vec, bool& is_valid, size_t len, bool loose) {
		return _stl_container_lua_to(L, index, vec, is_valid, len, loose);
	}

	template<class T, class Allocator>
	inline std::shared_ptr<std::vector<T, Allocator>> lua_to(lua_State* L, int index, std::vector<T, Allocator>* ptr, bool& is_valid, size_t len, bool loose) {
		return _stl_container_lua_to(L, index, ptr, is_valid, len, loose);
	}

	template<class T, class Allocator>
	inline void lua_push(lua_State* L, std::vector<T, Allocator>&& vec) {
		_stl_container_lua_push(L, std::move(vec));
	}

	template<class T, class Allocator>
	inline void lua_push(lua_State* L, const std::vector<T, Allocator>& vec) {
		_stl_container_lua_push(L, vec);
	}


	// ================================
	// std::function
	// ================================

	// sol2/include/sol/function.hpp:79
	namespace detail {
		template<class R, class... Args>
		struct FunctionInvoker {
			Function fn;
			std::shared_ptr<std::mutex> creator_gil_mutex;
			std::thread::id creator_thread_id;
			std::shared_ptr<std::unique_lock<std::mutex>> creator_gil;

			FunctionInvoker() = default;

			FunctionInvoker(lua_State* L, int index, bool& is_valid) {
				auto fn = lua_to(L, index, static_cast<Function*>(nullptr), is_valid);
				if (is_valid) {
					this->fn.assign(L, fn);
					this->creator_gil_mutex = get_gil_mutex(L);
					this->creator_thread_id = std::this_thread::get_id();
					this->creator_gil = get_thread_gil(L, creator_gil_mutex);
				}
			}

			~FunctionInvoker() = default;

			template <class... _Ts>
			static R invoke(lua_State* L, Function& fn, _Ts&&... args) {
				lua_push(L, fn);

				// https://stackoverflow.com/questions/7230621/how-can-i-iterate-over-a-packed-variadic-template-argument-list/60136761#60136761
				int nargs = 0;
				([&] {
					lua_push(L, args);
					nargs++;
					} (), ...);

				if constexpr (std::is_same_v<R, void>) {
					lua_call(L, nargs, 0);
				}
				else {
					lua_call(L, nargs, 1);
				}

				if constexpr (!std::is_same_v<R, void>) {
					// I did not find a way to keep reference to pointers without memory leak
					// Return the pointer hoping it will not be garbage collected
					if constexpr (std::is_same_v<R, const char*>) {
						bool is_valid = lua_type(L, -1) == LUA_TSTRING;
						if (!is_valid) {
							luaL_typeerror(L, -1, internal::GetTypeName<R>());
						}

						size_t len;
						auto c_str = lua_tolstring(L, -1, &len);
						lua_pop(L, 1);
						return c_str;
					}
					else if constexpr (std::is_pointer_v<R>) {
						bool is_valid = lua_islightuserdata(L, -1);
						if (!is_valid) {
							luaL_typeerror(L, -1, internal::GetTypeName<R>());
						}

						auto ptr = static_cast<R>(lua_touserdata(L, -1));
						lua_pop(L, 1);
						return ptr;
					}
					else {
						bool is_valid;
						auto holder_value = lua_to(L, -1, static_cast<R*>(nullptr), is_valid);
						if (!is_valid) {
							luaL_typeerror(L, -1, internal::GetTypeName<R>());
						}

						lua_pop(L, 1);
						decltype(auto) value = extract_holder(holder_value, static_cast<R*>(nullptr));
						return value;
					}
				}
			}

			R operator()(Args&&... args) {
				auto& L = fn.L;
				GilLock lock(L, creator_gil_mutex, creator_thread_id, creator_gil);

				if constexpr (std::is_same_v<R, void>) {
					invoke(L, fn, std::forward<Args>(args)...);
				}
				else {
					return invoke(L, fn, std::forward<Args>(args)...);
				}
			}
		};
	} // namespace detail

	template<class R, class... Args>
	inline std::function<R(Args...)> lua_to(lua_State* L, int index, std::function<R(Args...)>*, bool& is_valid) {
		if (lua_isnil(L, index)) {
			is_valid = true;
			return nullptr;
		}
		detail::FunctionInvoker<R, Args...> fx(L, index, is_valid);
		return fx;
	}


	// ================================
	// misc functions
	// ================================

	template<typename T>
	int lua_method_isinstance(lua_State* L) {
		bool is_valid;
		lua_userdata_to(L, 1, static_cast<T*>(nullptr), is_valid);
		lua_pushboolean(L, is_valid);
		return 1;
	}

	template<typename T>
	int lua_method__eq(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc != 2) {
			return luaL_error(L, "missing lhs and rhs arguments");
		}

		bool is_valid;

		const auto lhs = lua_userdata_to(L, 1, static_cast<T*>(nullptr), is_valid);
		if (!is_valid) {
			lua_pushboolean(L, false);
			return 1;
		}

		const auto rhs = lua_userdata_to(L, 2, static_cast<T*>(nullptr), is_valid);
		if (!is_valid) {
			lua_pushboolean(L, false);
			return 1;
		}

		lua_pushboolean(L, lhs.get() == rhs.get());
		return 1;
	}

	template<typename T>
	int lua_method__gc(lua_State* L) {
		using SharedPtr = std::shared_ptr<T>;
		auto userdata_ptr = static_cast<SharedPtr*>(lua_touserdata(L, 1));

		// There are objects that are waiting
		// for callbacks to be executed before
		// destroying themselves
		GilYield yielder(L);

		userdata_ptr->~SharedPtr();
		return 0;
	}

	template<typename T>
	int lua_method__self(lua_State* L) {
		bool is_valid;
		const auto userdata = lua_userdata_to(L, 1, static_cast<T*>(nullptr), is_valid);

		if (!is_valid) {
			return 0;
		}

		lua_pushlightuserdata(L, static_cast<void*>(userdata.get()));
		return 1;
	}

	template<typename T>
	int lua_method__cast(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc == 0) {
			return luaL_error(L, "self is not defined");
		}

		if (vargc != 1) {
			return luaL_error(L, "too many arguments");
		}

		if (!lua_islightuserdata(L, 1)) {
			return luaL_typeerror(L, 1, "userdata or ligthuserdata");
		}

		lua_push(L, static_cast<T*>(lua_touserdata(L, 1)));
		return 1;
	}

	/**
	 * https://github.com/ThePhD/sol2/blob/v3.3.0/include/sol/stack_core.hpp#L1338
	 */
	template<typename T>
	int member_default_to_string(lua_State* L) {
		bool is_valid;
		lua_push(L, lua_userdata_to(L, 1, static_cast<T*>(nullptr), is_valid)->to_string());
		return 1;
	}

	/**
	 * https://github.com/ThePhD/sol2/blob/v3.3.0/include/sol/stack_core.hpp#L1352
	 */
	template <typename T>
	int adl_default_to_string(lua_State* L) {
		bool is_valid;
		lua_push(L, std::to_string(*lua_userdata_to(L, 1, static_cast<T*>(nullptr), is_valid)));
		return 1;
	}

	/**
	 * https://github.com/ThePhD/sol2/blob/v3.3.0/include/sol/stack_core.hpp#L1364
	 */
	template<typename T>
	int oss_default_to_string(lua_State* L) {
		std::ostringstream oss; bool is_valid;
		oss << *lua_userdata_to(L, 1, static_cast<T*>(nullptr), is_valid);
		lua_push(L, oss.str());
		return 1;
	}

	template<typename T>
	int usertype_default_to_string(lua_State* L) {
		bool is_valid;
		const auto userdata_ptr = lua_userdata_to(L, 1, static_cast<T*>(nullptr), is_valid);
		if (!is_valid) {
			lua_pushnil(L);
			return 1;
		}

		lua_pushfstring(L, "cdata<%s>: %p", internal::GetTypeName<T>(), static_cast<void*>(userdata_ptr.get()));
		return 1;
	}

	template<typename K, typename V>
	std::shared_ptr<std::map<K, V>> lua_map_new(const std::vector<std::pair<K, V>>& pairs) {
		auto res = std::make_shared<std::map<K, V>>();
		for (const auto& pair_ : pairs) {
			res->insert_or_assign(pair_.first, pair_.second);
		}
		return res;
	}

	template<template<typename, typename, typename...> typename Container, typename K, typename V, typename... _Ts>
	int lua_map_method__index(lua_State* L, Container<K, V, _Ts...>& m, K key) {
		if (m.count(key)) {
			lua_reference_push(L, m.at(key));
		}
		else {
			lua_pushnil(L);
		}
		return 1;
	}

	template<template<typename, typename, typename...> typename Container, typename K, typename V, typename... _Ts>
	void lua_map_method_keys(const Container<K, V, _Ts...>& m, std::vector<K>& keys) {
		for (auto it = m.begin(); it != m.end(); ++it) {
			keys.push_back(it->first);
		}
	}

	template<template<typename...> typename Container, typename... _Ts>
	inline decltype(auto) _stl_container__index(lua_State* L, Container<_Ts...>& container, size_t index) {
		if (index >= container.size()) {
			luaL_error(L, "index %d is out of range. Expecting a number between 0 and %d.", index, container.size() - 1);
		}
		return container.at(index);
	}

	size_t atosize_t(lua_State* L, const std::string& s);
	int atoi(lua_State* L, const std::string& s);

	template<template<typename...> typename Container, typename... _Ts>
	inline decltype(auto) _stl_container__index(lua_State* L, Container<_Ts...>& container, const std::string& s) {
		return _stl_container__index(L, container, atosize_t(L, s));
	}

	template<template<typename...> typename Container, typename... _Ts, typename T>
	inline void _stl_container__newindex(lua_State* L, Container<_Ts...>& container, size_t index, const T& value) {
		if (index > container.size()) {
			luaL_error(L, "index %d is out of range. Expecting a number between 0 and %d.", index, container.size());
		}

		if (index == container.size()) {
			if constexpr (requires(Container<_Ts...>&container, const T & value) { container.Add(value); }) {
				container.Add(value);
			}
			else if constexpr (requires(Container<_Ts...>&container, const T & value) { *container.Add() = value; }) {
				*container.Add() = value;
			}
			else {
				container.push_back(value);
			}
		}
		else {
			container.at(index) = value;
		}
	}

	template<template<typename...> typename Container, typename... _Ts, typename T>
	inline void _stl_container__newindex(lua_State* L, Container<_Ts...>& container, const std::string& s, const T& value) {
		_stl_container__newindex(L, container, atosize_t(L, s), value);
	}

	template<template<typename...> typename Container, typename... _Ts>
	int _stl_container__ipairs_iter(lua_State* L) {
		using Type = std::shared_ptr<Container<_Ts...>>;
		bool is_valid = false;

		auto holder_container = lua_to(L, 1, static_cast<Type*>(nullptr), is_valid);
		if (!is_valid) {
			luaL_typeerror(L, 1, internal::GetTypeName<Type>());
		}
		decltype(auto) container = extract_holder(holder_container, static_cast<Type*>(nullptr));

		auto holder_index = lua_to(L, 2, static_cast<size_t*>(nullptr), is_valid);
		if (!is_valid) {
			luaL_typeerror(L, 2, "size_t");
		}
		decltype(auto) index = extract_holder(holder_index, static_cast<size_t*>(nullptr));

		const auto i = index + 1;

		if (i >= container->size()) {
			return 0;
		}

		lua_push(L, i);
		lua_reference_push(L, container->at(i));
		return 2;
	}

	template<template<typename...> typename Container, typename... _Ts>
	inline int _stl_container__ipairs(lua_State* L, Container<_Ts...>& conaitner) {
		lua_pushcclosure(L, _stl_container__ipairs_iter<Container, _Ts...>, 0);
		lua_reference_push(L, conaitner);
		lua_push(L, -1);
		return 3;
	}

	template<std::size_t I, typename... _Ts>
	void lua_inherit(lua_State* L, int index) {
		using _Tuple = typename std::tuple<_Ts...>;
		using T = std::tuple_element_t<I, _Tuple>;

		if constexpr (is_usertype_v<T>) {
			usertype_push_metatable<T>(L);
			int parent = lua_gettop(L);

			// https://www.lua.org/manual/5.1/manual.html#lua_next

			lua_pushnil(L);  /* first key */
			// stack now contains: -1 => nil

			while (lua_next(L, parent) != 0) {
				// stack now contains: -2 => key; -1 => value
				const auto key = lua_gettop(L) - 1;
				const auto value = lua_gettop(L);

				lua_pushvalue(L, key);
				lua_rawget(L, index);
				const auto exists = !lua_isnil(L, -1);
				lua_pop(L, 1);

				if (!exists) {
					lua_pushvalue(L, key);
					lua_pushvalue(L, value);
					lua_rawset(L, index);
				}

				lua_pop(L, 1);
				// stack now contains: -1 => key
			}

			lua_pop(L, 1);
		}

		if constexpr (I != sizeof...(_Ts) - 1) {
			lua_inherit<I + 1, _Ts...>(L, index);
		}
	}

	template<typename T, typename... _Ts>
	void lua_inherit(lua_State* L) {
		if constexpr (sizeof...(_Ts) != 0) {
			usertype_push_metatable<T>(L);
			const auto index = lua_gettop(L);
			lua_inherit<0, _Ts...>(L, index);
			lua_pop(L, 1);
		}
	}

	inline void lua_defaults_pushfuncs(lua_State* L, const luaL_Reg* l) {
		for (; l->name; l++) {
			lua_pushstring(L, l->name);
			lua_rawget(L, -2);
			bool exists = !lua_isnil(L, -1);
			lua_pop(L, 1);

			if (!exists) {
				lua_pushstring(L, l->name);
				lua_pushcclosure(L, l->func, 0);
				lua_rawset(L, -3);
			}
		}
	}

	template<typename T>
	inline void lua_register_defaults(lua_State* L) {
		usertype_push_metatable<T>(L);

		// ================================================================
		// https://github.com/ThePhD/sol2/blob/v3.3.0/include/sol/types.hpp#L907
		// ================================================================

		// meta::supports_op_left_shift<std::ostream, meta::unqualified_t<T>>
		// https://github.com/ThePhD/sol2/blob/v3.3.0/include/sol/traits.hpp#L519
		// decltype(std::declval<T&>() << std::declval<U&>())
		if constexpr (requires(std::ostream & oss, const T & t) { oss << t; }) {
			const struct luaL_Reg lua_tostring_methods[] = {
				{"__tostring", oss_default_to_string<T>},
				{NULL, NULL} // Sentinel
			};
			lua_defaults_pushfuncs(L, lua_tostring_methods);
		}

		// meta::supports_to_string_member<meta::unqualified_t<T>>
		// https://github.com/ThePhD/sol2/blob/v3.3.0/include/sol/traits.hpp#L551
		// class supports_to_string_member : public meta::boolean<meta_detail::has_to_string_test<meta_detail::non_void_t<T>>::value> { };
		// https://github.com/ThePhD/sol2/blob/v3.3.0/include/sol/traits.hpp#L465
		// https://github.com/ThePhD/sol2/blob/v3.3.0/include/sol/traits.hpp#L469
		// static sfinae_yes_t test(decltype(std::declval<C>().to_string())*);
		else if constexpr (requires(const T & t) { t.to_string(); }) {
			const struct luaL_Reg lua_tostring_methods[] = {
				{"__tostring", member_default_to_string<T>},
				{NULL, NULL} // Sentinel
			};
			lua_defaults_pushfuncs(L, lua_tostring_methods);
		}

		// meta::supports_adl_to_string<meta::unqualified_t<T>>
		// https://github.com/ThePhD/sol2/blob/v3.3.0/include/sol/traits.hpp#L547
		// class supports_adl_to_string : public meta_detail::supports_adl_to_string_test<T> { };
		// https://github.com/ThePhD/sol2/blob/v3.3.0/include/sol/traits.hpp#L523
		// class supports_adl_to_string_test<T, void_t<decltype(to_string(std::declval<const T&>()))>> : public std::true_type { };
		else if constexpr (requires(const T & t) { std::to_string(t); }) {
			const struct luaL_Reg lua_tostring_methods[] = {
				{"__tostring", adl_default_to_string<T>},
				{NULL, NULL} // Sentinel
			};
			lua_defaults_pushfuncs(L, lua_tostring_methods);
		}

		// FIXME : how to use this method only if parent __tostring is not a usertype_default_to_string
		else if constexpr (requires(lua_State * L, int index, bool& is_valid) { usertype_info<T>::lua_userdata_to(L, index, is_valid); }) {
			const struct luaL_Reg lua_tostring_methods[] = {
				{"__tostring", usertype_default_to_string<T>},
				{NULL, NULL} // Sentinel
			};
			lua_defaults_pushfuncs(L, lua_tostring_methods);
		}

		if constexpr (requires(lua_State * L, int index, bool& is_valid) { usertype_info<T>::lua_userdata_to(L, index, is_valid); }) {
			// class Garbage-Collection and introspection methods
			const struct luaL_Reg lua_instance_misc_methods[] = {
				{"__gc", lua_method__gc<T>},
				{"__eq", lua_method__eq<T>},
				{"__cast", lua_method__cast<T>}, // For ffi purpose
				{"isinstance", lua_method_isinstance<T>},
				{NULL, NULL} // Sentinel
			};
			lua_defaults_pushfuncs(L, lua_instance_misc_methods);
		}

		lua_pop(L, 1);
	}

	template<typename T, typename... _Ts>
	inline void lua_register_class(lua_State* L, const char* name) {
		// reuse existing table if available, otherwise, create a new one
		lua_pushstring(L, name);
		lua_rawget(L, -2); // cls = module[name]

		if (lua_isnil(L, -1)) { // if cls == nil
			lua_pop(L, 1); // pop nil
			lua_pushstring(L, name);
			lua_newtable(L); // cls = {}
			lua_rawset(L, -3); // module[name] = cls
			lua_pushstring(L, name);
			lua_rawget(L, -2); // cls = module[name]
		}

		lua_pushliteral(L, "__index");
		lua_pushvalue(L, -2);
		lua_rawset(L, -3); // cls.__index = cls

		lua_pushliteral(L, "__name");
		lua_pushstring(L, internal::GetTypeName<T>());
		lua_rawset(L, -3); // cls.__name = typename

		if constexpr (requires(lua_State * L, int index, bool& is_valid) { usertype_info<T>::lua_userdata_to(L, index, is_valid); }) {
			lua_pushliteral(L, "__has_self");
			lua_pushboolean(L, true);
			lua_rawset(L, -3); // cls.__has_self = typename

			// For ffi purpose
			if constexpr (requires(lua_State * L) { lua_push(L, sizeof(T)); }) {
				lua_pushliteral(L, "__sizeof");
				lua_push(L, sizeof(T));
				lua_rawset(L, -3);
			}
		}

		// class registered methods
		lua_pushfuncs(L, usertype_info<T>::methods);

		// metatable = {}
		lua_newtable(L);

		lua_pushfuncs(L, usertype_info<T>::meta_methods);

		// setmetatable(cls, metatable)
		lua_setmetatable(L, -2);

		std::unique_lock lock(usertype_info<T>::mutex);
		const auto index = get_luaopen_index(L);
		const auto size = usertype_info<T>::metatable_pointers.size();
		if (index >= size) {
			usertype_info<T>::metatable_pointers.resize(index + 1, nullptr);
			usertype_info<T>::metatable_refs.resize(index + 1, LUA_REFNIL);
		}
		usertype_info<T>::metatable_pointers.at(index) = lua_topointer(L, -1);
		usertype_info<T>::metatable_refs.at(index) = luaL_ref(L, LUA_REGISTRYINDEX);
	}

	template<std::same_as<const char*> T, std::size_t N>
	inline int lua_rawget_create_if_nil(lua_State* L, int index, const T(&keys)[N]) {
		static_assert(N > 0, "keys are required");

		lua_pushvalue(L, index);
		int ref = luaL_ref(L, LUA_REGISTRYINDEX); // save last value

		lua_pushnil(L); // push next key

		for (const T& key : keys) {
			lua_pop(L, 1); // pop previous key
			lua_rawgeti(L, LUA_REGISTRYINDEX, ref); // push next table
			luaL_unref(L, LUA_REGISTRYINDEX, ref);

			if (!lua_istable(L, -1)) {
				ref = LUA_REFNIL;
				lua_pushnil(L); // push next key
				break;
			}

			lua_pushstring(L, key); // push next key
			lua_rawget(L, -2); // push next value

			if (lua_isnil(L, -1)) {
				lua_pop(L, 1); // pop nil value

				// create table and set field key as the newly created table
				lua_pushstring(L, key);
				lua_newtable(L);
				lua_rawset(L, -3);

				// push the newly created table on top of the stack
				lua_pushstring(L, key);
				lua_rawget(L, -2);
			}

			ref = luaL_ref(L, LUA_REGISTRYINDEX); // save last value
		}

		lua_pop(L, 1); // pop next key

		if (ref == LUA_REFNIL) {
			return 0;
		}

		lua_rawgeti(L, LUA_REGISTRYINDEX, ref); // push last value
		luaL_unref(L, LUA_REGISTRYINDEX, ref);
		return 1;
	}

	// ================================
	// __eq__
	// ================================

	template<typename T>
	inline bool __eq__(const T& o1, const T& o2) {
		if constexpr (requires(const T & a, const T & b) { static_cast<bool>(a == b); }) {
			return static_cast<bool>(o1 == o2);
		}
		else {
			return &o1 == &o2;
		}
	}

	template<typename T>
	inline bool __eq__(const std::shared_ptr<T>& p1, const std::shared_ptr<T>& p2) {
		if (static_cast<bool>(p1) && static_cast<bool>(p2)) {
			return __eq__(*p1, *p2);
		}
		return !static_cast<bool>(p1) && !static_cast<bool>(p2);
	}

	template<typename K, typename V>
	inline bool __eq__(const std::map<K, V>& m1, const std::map<K, V>& m2) {
		if (m1.size() != m2.size()) {
			return false;
		}

		for (const auto& [key, value] : m1) {
			if (!m2.count(key) || !__eq__(value, m2.at(key))) {
				return false;
			}
		}

		return true;
	}

	template<typename T1, typename T2>
	inline bool __eq__(const std::pair<T1, T2>& p1, const std::pair<T1, T2>& p2) {
		return __eq__(p1.first, p2.first) && __eq__(p1.second, p2.second);
	}

	template<class T, class Allocator>
	inline bool __eq__(const std::vector<T, Allocator>& v1, const std::vector<T, Allocator>& v2) {
		if (v1.size() != v2.size()) {
			return false;
		}
		const auto mismatched = std::mismatch(v1.begin(), v1.end(), v2.begin(), static_cast<bool(*)(const T&, const T&)>(__eq__));
		return mismatched.first == v1.end();
	}

	// ================================
}
