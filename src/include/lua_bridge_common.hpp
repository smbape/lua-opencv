#pragma once

#include <lua_bridge_common.hdr.hpp>
#include <Keywords.hpp>

namespace LUA_MODULE_NAME {
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
	// _Object
	// ================================

	template<typename T>
	PushGuard::PushGuard(lua_State* L, const T& any) : L(L) {
		lua_push(L, any);
	}

	template<int Kind>
	template<typename T>
	_Object<Kind>::_Object(const T& any) {
		lua_State* L = get_global_state();
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

	template<std::size_t I, typename T, typename... _Ts>
	inline std::shared_ptr<T> lua_userdata_signature_to(lua_State* L, int index, const void* signature, bool& is_valid) {
		using SharedPtr = std::shared_ptr<T>;
		using _Tuple = typename std::tuple<_Ts...>;

		if constexpr (I == 0) {
			if (signature == usertype_info<T>::signature) {
				is_valid = true;
				return *static_cast<SharedPtr*>(lua_touserdata(L, index));
			}

			if constexpr (requires(const void* signature) { usertype_info<T>::derives.count(signature); }) {
				// Upcasting
				if (usertype_info<T>::derives.count(signature)) {
					is_valid = true;

					// https://stackoverflow.com/questions/40336882/c-reinterpret-cast-of-stdshared-ptr-reference-to-optimize#40345780
					// Shared pointer to derived type is implicitly convertible (1) to a shared pointer to base class.
					return *static_cast<SharedPtr*>(lua_touserdata(L, index));
				}
			}
		}
		else {
			using Base = std::tuple_element_t<I - 1, _Tuple>;
			if constexpr (is_usertype_v<Base>) {
				if (signature == usertype_info<Base>::signature) {
					is_valid = true;

					// Downcasting
					auto ptr = static_cast<std::shared_ptr<Base>*>(lua_touserdata(L, index));
					if (std::type_index(typeid(**ptr)) == std::type_index(typeid(T))) {
						return std::reinterpret_pointer_cast<T>(*ptr);
					}
				}
			}
		}

		if constexpr (I != sizeof...(_Ts)) {
			return lua_userdata_signature_to<I + 1, T, _Ts...>(L, index, signature, is_valid);
		}
		else {
			is_valid = false;
			return SharedPtr();
		}
	}

	template<typename T, typename... _Ts>
	inline std::shared_ptr<T> lua_userdata_signature_to(lua_State* L, int index, bool& is_valid) {
		if (!lua_isuserdata(L, index) || !lua_getmetatable(L, index)) {
			is_valid = false;
			return std::shared_ptr<T>();
		}

		auto signature = lua_topointer(L, -1);
		lua_pop(L, 1);

		return lua_userdata_signature_to<0, T, _Ts...>(L, index, signature, is_valid);
	}

	template<typename T>
	inline void usertype_push_metatable(lua_State* L) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, usertype_info<T>::metatable);
	}


	// ================================
	// T
	// ================================

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
	inline int lua_push(lua_State* L, T* ptr, void (*d)(T*)) {
		return lua_push(L, std::make_shared<T>(ptr, CFunctionDeleter(d)));
	}


	// ================================
	// T if is_usertype_v<T>
	// ================================

	template<typename T>
	inline std::enable_if_t<is_usertype_v<T>, std::shared_ptr<T>> lua_to(lua_State* L, int index, T* ptr, bool& is_valid) {
		if constexpr (requires(lua_State * L, const size_t __top__, bool& is_valid) { usertype_info<T>::Lua_new(L, __top__, is_valid); }) {
			auto value = lua_userdata_to(L, index, ptr, is_valid);
			if (is_valid) {
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

			return value;
		}
		else {
			return lua_userdata_to(L, index, ptr, is_valid);
		}
	}

	template<typename T>
	inline std::enable_if_t<is_usertype_v<T>, T*> lua_to(lua_State* L, int index, T**, bool& is_valid) {
		is_valid = lua_isnil(L, index);
		if (is_valid) {
			return static_cast<T*>(nullptr);
		}
		return lua_to(L, index, static_cast<T*>(nullptr), is_valid).get();
	}

	template<typename T>
	inline std::enable_if_t<is_usertype_v<T>, int> lua_push(lua_State* L, T* ptr) {
		if (!ptr) {
			lua_pushnil(L);
			return 1;
		}
		return lua_push(L, reference_internal(ptr));
	}

	template<typename T>
	inline std::enable_if_t<is_usertype_v<T>, int> lua_push(lua_State* L, T&& obj) {
		return lua_push(L, std::make_shared<T>(std::move(obj)));
	}

	template<typename T>
	inline std::enable_if_t<is_usertype_v<T>, int> lua_push(lua_State* L, const T& obj) {
		return lua_push(L, std::make_shared<T>(obj));
	}


	// ================================
	// std::shared_ptr
	// ================================

	template<typename T>
	inline std::shared_ptr<T> lua_to(lua_State* L, int index, std::shared_ptr<T>*, bool& is_valid) {
		is_valid = lua_isnil(L, index);
		if (is_valid) {
			return std::shared_ptr<T>();
		}
		return lua_to(L, index, static_cast<T*>(nullptr), is_valid);
	}

	template<typename T>
	inline int lua_push(lua_State* L, const std::shared_ptr<T>& ptr) {
		if (!ptr) {
			lua_pushnil(L);
			return 1;
		}

		if constexpr (requires(const std::type_index & index) { usertype_info<T>::derives_pushers.count(index); }) {
			// Downcasting
			if (auto search = usertype_info<T>::derives_pushers.find(std::type_index(typeid(*ptr))); search != usertype_info<T>::derives_pushers.end()) {
				return search->second(L, ptr);
			}
		}

		using SharedPtr = std::shared_ptr<T>;
		auto userdata_ptr = static_cast<SharedPtr*>(lua_newuserdata(L, sizeof(SharedPtr)));
		new(userdata_ptr) SharedPtr(ptr); // userdata = new std::shared_ptr<T>(ptr)

		usertype_push_metatable<T>(L);
		lua_setmetatable(L, -2);

		return 1; // return userdata
	}


	// ================================
	// std::map
	// ================================

	template<typename K, typename V>
	inline void lua_to(lua_State* L, int index, std::map<K, V>& out, bool& is_valid) {
		using Map = std::map<K, V>;

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

			auto value_holder = lua_to(L, -1, static_cast<V*>(nullptr), is_valid);

			/* removes 'value'; keeps 'key' for the next iteration */
			lua_pop(L, 1);
			// stack now contains: -1 => key

			if (!is_valid) {
				/* removes 'key'; break iteration */
				lua_pop(L, 1);
				// stack is now the same as it was on entry to this function

				break;
			}

			decltype(auto) value = extract_holder(value_holder, static_cast<V*>(nullptr));
			out.insert_or_assign(key, value);
		}
	}

	template<typename K, typename V>
	inline std::shared_ptr<std::map<K, V>> lua_to(lua_State* L, int index, std::map<K, V>* ptr, bool& is_valid) {
		if (lua_isuserdata(L, index)) {
			return lua_userdata_to(L, index, ptr, is_valid);
		}

		if (index < 0) {
			index += lua_gettop(L) + 1;
		}

		auto out = std::make_shared<std::map<K, V>>();
		lua_to(L, index, *out, is_valid);
		return out;
	}

	template<typename K, typename V>
	inline int lua_push(lua_State* L, std::map<K, V>&& kv) {
		std::map<K, V> _kv(kv);
		return lua_push(L, _kv);
	}

	template<typename K, typename V>
	inline int lua_push(lua_State* L, const std::map<K, V>& kv) {
		lua_newtable(L);
		int index = lua_gettop(L);
		int i = 0;
		for (const auto& [k, v] : kv) {
			lua_push(L, k);
			lua_push(L, v);
			lua_rawset(L, index);
		}
		return 1;
	}


	// ================================
	// std::optional
	// ================================

	template<typename T>
	inline std::optional<T> lua_to(lua_State* L, int index, std::optional<T>*, bool& is_valid) {
		is_valid = lua_isnil(L, index);
		if (is_valid) {
			return std::nullopt;
		}

		auto value_holder = lua_to(L, index, static_cast<T*>(nullptr), is_valid);
		if (!is_valid) {
			return std::nullopt;
		}

		decltype(auto) value = extract_holder(value_holder, static_cast<T*>(nullptr));
		return value;
	}

	template<typename T>
	inline int lua_push(lua_State* L, const std::optional<T>& p) {
		if (p) {
			return lua_push(L, p.value());
		}

		lua_pushnil(L);
		return 1;
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
	inline int lua_push(lua_State* L, const std::pair<T1, T2>& p) {
		lua_newtable(L);
		int index = lua_gettop(L);

		lua_push(L, p.first);
		lua_rawset(L, 1);

		lua_push(L, p.second);
		lua_rawset(L, 2);

		return 1;
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
	inline int lua_push(lua_State* L, const std::tuple<_Ts...>& value) {
		lua_newtable(L);
		int index = lua_gettop(L);
		_lua_push(L, index, value);
		return 1;
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
	inline int _lua_push(lua_State* L, const std::variant<_Ts...>& value) {
		using _Tuple = typename std::tuple<_Ts...>;
		using T = std::tuple_element_t<I, _Tuple>;

		if constexpr (!std::is_same_v<std::monostate, T>) {
			if (std::holds_alternative<T>(value)) {
				return lua_push(L, std::get<T>(value));
			}
		}

		if constexpr (I == sizeof...(_Ts) - 1) {
			return 0;
		}
		else {
			return _lua_push<I + 1, _Ts...>(L, value);
		}
	}

	template<typename... _Ts>
	inline int lua_push(lua_State* L, const std::variant<_Ts...>& value) {
		return _lua_push(L, value);
	}


	// ================================
	// stl container
	// ================================

	template<template<typename> typename Container, typename... _Ts>
	inline void _stl_container_lua_to(lua_State* L, int index, Container<_Ts...>& out, bool& is_valid, size_t len, bool loose) {
		using _Tuple = typename std::tuple<_Ts...>;
		using T = std::tuple_element_t<0, _Tuple>;

		if (lua_isuserdata(L, index)) {
			out = *lua_userdata_to(L, index, static_cast<Container<_Ts...>*>(nullptr), is_valid);
			if (is_valid) {
				const auto& size = out.size();
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
			out.Clear();
		}
		else {
			out.clear();
		}

		if constexpr (requires(Container<_Ts...>&container, int new_size) { container.Reserve(new_size); }) {
			out.Reserve(size);
		}
		else {
			out.reserve(size);
		}

		for (auto i = 1; i <= size; ++i) {
			lua_pushnumber(L, i);
			lua_rawget(L, index);
			auto value_holder = lua_to(L, -1, static_cast<T*>(nullptr), is_valid);
			lua_pop(L, 1);

			if (!is_valid) {
				break;
			}

			decltype(auto) value = extract_holder(value_holder, static_cast<T*>(nullptr));

			if constexpr (requires(Container<_Ts...>&out, const T & value) { out.Add(value); }) {
				out.Add(value);
			}
			else if constexpr (requires(Container<_Ts...>&out, T && value) { out.Add(std::move(value)); }) {
				out.Add(std::move(value));
			}
			else {
				out.push_back(value);
			}
		}
	}

	template<template<typename> typename Container, typename... _Ts>
	inline std::shared_ptr<Container<_Ts...>> _stl_container_lua_to(lua_State* L, int index, Container<_Ts...>* ptr, bool& is_valid, size_t len, bool loose) {
		if (lua_isuserdata(L, index)) {
			auto out = lua_userdata_to(L, index, ptr, is_valid);
			if (!is_valid) {
				return std::shared_ptr<Container<_Ts...>>();
			}

			const auto& size = out->size();
			is_valid = len == 0 || (loose ? size <= len : size == len);
			return out;
		}

		auto out = std::make_shared<Container<_Ts...>>();
		lua_to(L, index, *out, is_valid, len, loose);
		return out;
	}

	template<template<typename> typename Container, typename... _Ts>
	inline int _stl_container_lua_push(lua_State* L, Container<_Ts...>&& container) {
		Container<_Ts...> _container(std::move(container));
		return lua_push(L, _container);
	}

	template<template<typename> typename Container, typename... _Ts>
	inline int _stl_container_lua_push(lua_State* L, const Container<_Ts...>& container) {
		lua_newtable(L);
		int index = lua_gettop(L);
		int i = 0;
		for (const auto& v : container) {
			lua_push(L, v);
			lua_rawseti(L, index, ++i);
		}
		return 1;
	}


	// ================================
	// std::vector
	// ================================

	template<class T, class Allocator>
	inline void lua_to(lua_State* L, int index, std::vector<T, Allocator>& out, bool& is_valid, size_t len, bool loose) {
		return _stl_container_lua_to<std::vector, T, Allocator>(L, index, out, is_valid, len, loose);
	}

	template<class T, class Allocator>
	inline std::shared_ptr<std::vector<T, Allocator>> lua_to(lua_State* L, int index, std::vector<T, Allocator>* ptr, bool& is_valid, size_t len, bool loose) {
		return _stl_container_lua_to<std::vector, T, Allocator>(L, index, ptr, is_valid, len, loose);
	}

	template<class T, class Allocator>
	inline int lua_push(lua_State* L, std::vector<T, Allocator>&& vec) {
		return _stl_container_lua_push<std::vector, T, Allocator>(L, std::move(vec));
	}

	template<class T, class Allocator>
	inline int lua_push(lua_State* L, const std::vector<T, Allocator>& vec) {
		return _stl_container_lua_push<std::vector, T, Allocator>(L, vec);
	}


	// ================================
	// std::function
	// ================================

	// sol2/include/sol/function.hpp:79
	namespace detail {
		template<class R, class... Args>
		struct FunctionInvoker {
			std::thread::id thread_id;
			Function fn;

			FunctionInvoker() = default;

			FunctionInvoker(lua_State* L, int index, bool& is_valid) {
				this->thread_id = std::this_thread::get_id();
				auto fn = lua_to(L, index, static_cast<Function*>(nullptr), is_valid);
				if (is_valid) {
					this->fn.assign(L, fn);
				}
			}

			~FunctionInvoker() = default;

			template <class... _Ts>
			static void invoke(lua_State* L, Function& fn, _Ts&&... args) {
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
			}

			R operator()(Args&&... args) {
				const auto async = thread_id != std::this_thread::get_id();
				GilLock lock(async);

				auto& L = fn.L;

				invoke(L, fn, std::forward<Args>(args)...);

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
						bool is_valid = lua_islightuserdata(L, -1) || lua_isuserdata(L, -1);
						if (!is_valid) {
							luaL_typeerror(L, -1, internal::GetTypeName<R>());
						}

						auto ptr = static_cast<R*>(lua_touserdata(L, -1));
						lua_pop(L, 1);
						return ptr;
					}
					else {
						bool is_valid;
						auto value_holder = lua_to(L, -1, static_cast<R*>(nullptr), is_valid);
						if (!is_valid) {
							luaL_typeerror(L, -1, internal::GetTypeName<R>());
						}

						lua_pop(L, 1);
						decltype(auto) value = extract_holder(value_holder, static_cast<R*>(nullptr));
						return value;
					}
				}
			}
		};
	} // namespace detail

	template<class R, class... Args>
	inline std::function<R(Args...)> lua_to(lua_State* L, int index, std::function<R(Args...)>*, bool& is_valid) {
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
		return lua_push(L, is_valid);
	}

	template<typename T>
	int lua_method__self(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc == 0) {
			return luaL_error(L, "self is not defined");
		}

		if (vargc != 1) {
			return luaL_error(L, "too many arguments");
		}

		bool is_valid;
		const auto userdata = lua_userdata_to(L, 1, static_cast<T*>(nullptr), is_valid);
		if (!is_valid) {
			lua_pushnil(L);
			return 1;
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

		return lua_push(L, static_cast<T*>(lua_touserdata(L, 1)));
	}

	template<typename T>
	int lua_method__gc(lua_State* L) {
		using SharedPtr = std::shared_ptr<T>;
		auto userdata_ptr = static_cast<SharedPtr*>(lua_touserdata(L, 1));
		userdata_ptr->~SharedPtr();
		return 0;
	}

	template<std::size_t I = 0, typename... _Ts>
	void lua_inherit_methods(lua_State* L) {
		if constexpr (I != sizeof...(_Ts) - 1) {
			lua_inherit_methods<I + 1, _Ts...>(L);
		}

		using _Tuple = typename std::tuple<_Ts...>;
		using T = std::tuple_element_t<I, _Tuple>;
		if constexpr (I != 0 && is_usertype_v<T>) {
			lua_pushfuncs(L, usertype_info<T>::methods);
		}
	}

	template<std::size_t I = 0, typename... _Ts>
	int lua_class__index(lua_State* L) {
		using _Tuple = typename std::tuple<_Ts...>;
		using T = std::tuple_element_t<I, _Tuple>;

		if constexpr (is_usertype_v<T>) {
			// for instantiable classes: lookup in the raw porperties of the metatable
			// =================================================
			usertype_push_metatable<T>(L); // push the metatable
			lua_pushvalue(L, 2); // push the key
			lua_rawget(L, -2);
			lua_remove(L, -2); // remove the metatable

			if (!lua_isnil(L, -1)) {
				return 1; // return metatable[key]
			}

			lua_pop(L, 1); // pop nil

			// for instantiable classes: lookup in the registered getters of the class 
			// =================================================
			bool is_valid;
			const std::string k = lua_to(L, 2, static_cast<std::string*>(nullptr), is_valid);
			if (is_valid && usertype_info<T>::getters.count(k)) {
				return usertype_info<T>::getters.at(k)(L);
			}

			if constexpr (I == sizeof...(_Ts) - 1) {
				lua_pushnil(L);
			}
		}
		else if constexpr (is_basetype_v<T>) {
			// for static classes: lookup in the porperties of the metatable
			// =================================================
			basetype_info<T>::push(L); // push the metatable
			lua_pushvalue(L, 2); // push the key
			lua_gettable(L, -2); // pop the key, push metatable[key]
			lua_remove(L, -2); // remove the metatable

			if (!lua_isnil(L, -1)) {
				return 1; // return metatable[key]
			}

			if constexpr (I != sizeof...(_Ts) - 1) {
				lua_pop(L, 1); // pop nil
			}
		}

		if constexpr (I == sizeof...(_Ts) - 1) {
			// if there are no parent class: return nil
			// =================================================
			return 1;
		}
		else {
			// otherwise: lookup in the parent class
			// =================================================
			return lua_class__index<I + 1, _Ts...>(L);
		}
	}

	template<std::size_t I = 0, typename... _Ts>
	int lua_class__newindex(lua_State* L) {
		using _Tuple = typename std::tuple<_Ts...>;
		using T = std::tuple_element_t<I, _Tuple>;

		if constexpr (is_usertype_v<T>) {
			// call the setter if defined
			bool is_valid;
			const std::string k = lua_to(L, 2, static_cast<std::string*>(nullptr), is_valid);
			if (is_valid && usertype_info<T>::setters.count(k)) {
				return usertype_info<T>::setters.at(k)(L);
			}
		}

		if constexpr (I == sizeof...(_Ts) - 1) {
			// set the value on the instance
			lua_pushvalue(L, 2); // push the key
			lua_pushvalue(L, 3); // push the value
			lua_rawset(L, 1);
			return 0;
		}
		else {
			// check the parent class setter
			return lua_class__newindex<I + 1, _Ts...>(L);
		}
	}

	template<std::size_t I = 0, typename... _Ts>
	int lua_instance__index(lua_State* L) {
		return lua_class__index<I, _Ts...>(L);
	}

	template<std::size_t I = 0, typename... _Ts>
	int lua_instance__newindex(lua_State* L) {
		return lua_class__newindex<I, _Ts...>(L);
	}

	/**
	 * https://github.com/ThePhD/sol2/blob/v3.3.0/include/sol/stack_core.hpp#L1338
	 */
	template<typename T>
	int member_default_to_string(lua_State* L) {
		bool is_valid;
		return lua_push(L, lua_userdata_to(L, 1, static_cast<T*>(nullptr), is_valid)->to_string());
	}

	/**
	 * https://github.com/ThePhD/sol2/blob/v3.3.0/include/sol/stack_core.hpp#L1352
	 */
	template <typename T>
	int adl_default_to_string(lua_State* L) {
		bool is_valid;
		return lua_push(L, std::to_string(*lua_userdata_to(L, 1, static_cast<T*>(nullptr), is_valid)));
	}

	/**
	 * https://github.com/ThePhD/sol2/blob/v3.3.0/include/sol/stack_core.hpp#L1364
	 */
	template<typename T>
	int oss_default_to_string(lua_State* L) {
		std::ostringstream oss; bool is_valid;
		oss << *lua_userdata_to(L, 1, static_cast<T*>(nullptr), is_valid);
		return lua_push(L, oss.str());
	}

	template<typename K, typename V>
	std::shared_ptr<std::map<K, V>> lua_map_new(const std::vector<std::pair<K, V>>& pairs) {
		std::shared_ptr<std::map<K, V>> res(new std::map<K, V>());
		for (const auto& pair_ : pairs) {
			res->insert_or_assign(pair_.first, pair_.second);
		}
		return res;
	}

	template<typename K, typename V>
	int lua_map_method__index(lua_State* L, const std::map<K, V>& m, K key) {
		if (m.count(key)) {
			lua_push(L, m.at(key));
		}
		else {
			lua_pushnil(L);
		}
		return 1;
	}

	template<typename K, typename V>
	void lua_map_method_keys(const std::map<K, V>& m, std::vector<K>& keys) {
		for (auto it = m.begin(); it != m.end(); ++it) {
			keys.push_back(it->first);
		}
	}

	template<typename T>
	decltype(auto) lua_vector_method__index(lua_State* L, const std::vector<T>& vec, size_t index) {
		if (index >= vec.size()) {
			luaL_error(L, "index %d is out of range. Expecting a number between 0 and %d.", index, vec.size() - 1);
		}
		return vec.at(index);
	}

	size_t atosize_t(lua_State* L, const std::string& s);
	int atoi(lua_State* L, const std::string& s);

	template<typename T>
	decltype(auto) lua_vector_method__index(lua_State* L, const std::vector<T>& vec, const std::string& s) {
		return lua_vector_method__index(L, vec, atosize_t(L, s));
	}

	template<typename T>
	void lua_vector_method__newindex(lua_State* L, std::vector<T>& vec, size_t index, const T& value) {
		if (index > vec.size()) {
			luaL_error(L, "index %d is out of range. Expecting a number between 0 and %d.", index, vec.size());
		}

		if (index == vec.size()) {
			vec.push_back(value);
		}
		else {
			vec.at(index) = value;
		}
	}

	template<typename T>
	void lua_vector_method__newindex(lua_State* L, std::vector<T>& vec, const std::string& s, const T& value) {
		lua_vector_method__newindex(L, vec, atosize_t(L, s), value);
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

		lua_inherit_methods<0, T, _Ts...>(L);

		// class index methods
		const struct luaL_Reg lua_instance_index_methods[] = {
			{"__index", lua_instance__index<0, T, _Ts...>}, // when we access an absent field in an instance
			{"__newindex", lua_instance__newindex<0, T, _Ts...>}, // when we assign a value to an absent field in an instance
			{NULL, NULL} // Sentinel
		};
		lua_pushfuncs(L, lua_instance_index_methods);

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
			lua_pushfuncs(L, lua_tostring_methods);
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
			lua_pushfuncs(L, lua_tostring_methods);
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
			lua_pushfuncs(L, lua_tostring_methods);
		}

		if constexpr (requires(lua_State * L, int index, bool& is_valid) { usertype_info<T>::lua_userdata_to(L, index, is_valid); }) {
			// class Garbage-Collection and introspection methods
			const struct luaL_Reg lua_instance_misc_methods[] = {
				{"__self", lua_method__self<T>}, // For ffi purpose
				{"__cast", lua_method__cast<T>}, // For ffi purpose
				{"isinstance", lua_method_isinstance<T>},
				{"__gc", lua_method__gc<T>},
				{NULL, NULL} // Sentinel
			};
			lua_pushfuncs(L, lua_instance_misc_methods);

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

		// class index methods
		const struct luaL_Reg lua_class_index_methods[] = {
			{"__index", lua_class__index<0, T, _Ts...>}, // when we access an absent field in the class
			{"__newindex", lua_class__newindex<0, T, _Ts...>}, // when we assign a value to an absent field in the class
			{NULL, NULL} // Sentinel
		};
		lua_pushfuncs(L, lua_class_index_methods);

		lua_pushfuncs(L, usertype_info<T>::meta_methods);

		// setmetatable(cls, metatable)
		lua_setmetatable(L, -2);

		usertype_info<T>::signature = lua_topointer(L, -1);
		usertype_info<T>::metatable = luaL_ref(L, LUA_REGISTRYINDEX);
	}

	template<std::same_as<const char*> T, std::size_t N>
	inline int lua_rawget_create_if_nil(lua_State* L, const T(&keys)[N]) {
		static_assert(N > 0, "keys are required");

		lua_pushvalue(L, -1);
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
}
