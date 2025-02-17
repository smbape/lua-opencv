#pragma once

#include <luadef.hpp>
#include <Keywords.hpp>

namespace LUA_MODULE_NAME {
	lua_State* get_global_state();
	void init_global_state(lua_State* L);

	using Callback = std::function<void(lua_State*, void*)>;
	int registerCallback(Callback callback, void* userdata = nullptr, std::optional<std::function<void(int)>> onRegistration = std::nullopt);
	int registerCallbackOnce(Callback callback, void* userdata = nullptr, std::optional<std::function<void(int)>> onRegistration = std::nullopt);
	bool unregisterCallback(int callback_id);
	int notifyCallbacks(lua_State* L);

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

	template<typename T, typename V>
	inline void assign_maybe_shared(void*& obj, T& value, V*) {
		using SharedPtr = std::shared_ptr<V>;
		if constexpr (std::is_same_v<T, SharedPtr>) {
			obj = new SharedPtr(value);
		}
		else {
			obj = new SharedPtr(new V(value));
		}
	}

	// ================================
	// exxplicit: lua_is, lua_to, lua_push
	// ================================

	template<typename T>
	inline bool lua_userdata_is(lua_State* L, int index, T*);

	template<typename T>
	inline std::shared_ptr<T> lua_userdata_to(lua_State* L, int index, T*);

	inline bool lua_is(lua_State* L, int index, bool*) {
		return lua_isboolean(L, index);
	}

	inline auto lua_to(lua_State* L, int index, bool*) {
		return !!lua_toboolean(L, index);
	}

	inline int lua_push(lua_State* L, bool value) {
		lua_pushboolean(L, value);
		return 1;
	}

	/**
	 * https://en.cppreference.com/w/cpp/types/numeric_limits
	 */
	inline bool lua_is(lua_State* L, int index, std::integral auto* ptr) {
		using Integer = std::decay<decltype(*ptr)>::type;

		if (lua_type(L, index) != LUA_TNUMBER) {
			return false;
		}
#if LUA_VERSION_NUM >= 503
		// Lua 5.3 and greater checks for numeric precision
		if (lua_isinteger(L, index)) {
			lua_Integer v = lua_tointeger(L, index);
			return v >= std::numeric_limits<Integer>::min() && v <= std::numeric_limits<Integer>::max();
		}
#endif
		const lua_Number v = lua_tonumber(L, index);
		// https://stackoverflow.com/questions/1521607/check-double-variable-if-it-contains-an-integer-and-not-floating-point/1521682#1521682
		static double intpart;
		if (std::modf(v, &intpart) != 0.0) {
			return false;
		}

		return v >= std::numeric_limits<Integer>::min() && v <= std::numeric_limits<Integer>::max();
	}

	inline auto lua_to(lua_State* L, int index, std::integral auto* ptr) {
		using Integer = std::decay<decltype(*ptr)>::type;

#if LUA_VERSION_NUM >= 503
		// Lua 5.3 and greater checks for numeric precision
		if (lua_isinteger(L, index)) {
			return static_cast<Integer>(lua_tointeger(L, index));
		}
#endif
		const lua_Number v = lua_tonumber(L, index);
		return static_cast<Integer>(v);
	}

	inline int lua_push(lua_State* L, std::integral auto n) {
#if LUA_VERSION_NUM >= 503
		// Lua 5.3 and greater checks for numeric precision
		lua_pushinteger(L, n);
#else
		lua_pushnumber(L, (lua_Number)n);
#endif
		return 1;
	}

	inline bool lua_is(lua_State* L, int index, std::floating_point auto*) {
		return lua_type(L, index) == LUA_TNUMBER;
	}

	inline auto lua_to(lua_State* L, int index, std::floating_point auto* ptr) {
		using Float = std::decay<decltype(*ptr)>::type;
		return static_cast<Float>(lua_tonumber(L, index));
	}

	inline int lua_push(lua_State* L, std::floating_point auto n) {
		lua_pushnumber(L, n);
		return 1;
	}

	inline bool lua_is(lua_State* L, int index, std::string*) {
		return lua_type(L, index) == LUA_TSTRING || lua_isnil(L, index);
	}

	inline std::string lua_to(lua_State* L, int index, std::string*) {
		if (lua_isnil(L, index)) {
			return std::string();
		}
		size_t len;
		auto str = lua_tolstring(L, index, &len);
		return std::string(str, len);
	}

	inline int lua_push(lua_State* L, const std::string& str) {
		lua_pushlstring(L, str.c_str(), str.size());
		return 1;
	}

	inline bool lua_is(lua_State* L, int index, unsigned char**) {
		return lua_islightuserdata(L, index);
	}

	inline unsigned char* lua_to(lua_State* L, int index, unsigned char**) {
		return reinterpret_cast<unsigned char*>(lua_touserdata(L, index));
	}

	inline int lua_push(lua_State* L, unsigned char* ptr) {
		lua_pushlightuserdata(L, ptr);
		return 1;
	}

	inline int lua_push(lua_State* L, const unsigned char* ptr) {
		lua_pushlightuserdata(L, const_cast<unsigned char*>(ptr));
		return 1;
	}

	inline bool lua_is(lua_State* L, int index, void**) {
		return lua_islightuserdata(L, index);
	}

	inline void* lua_to(lua_State* L, int index, void**) {
		return reinterpret_cast<void*>(lua_touserdata(L, index));
	}

	inline int lua_push(lua_State* L, void* ptr) {
		lua_pushlightuserdata(L, ptr);
		return 1;
	}

	inline int lua_push(lua_State* L, const void* ptr) {
		lua_pushlightuserdata(L, const_cast<void*>(ptr));
		return 1;
	}

	inline bool lua_is(lua_State* L, int index, Keywords* ptr) {
		return lua_istable(L, index) && lua_userdata_is(L, index, ptr);
	}

	inline int lua_push(lua_State* L, const char* s) {
		lua_pushstring(L, s);
		return 1;
	}


	// ================================
	// Object
	// ================================

	template<int Kind>
	struct _Object {
		_Object() = default;
		~_Object() {
			reset();
		}

		_Object(lua_State* L, int index) {
			init(L, index);
		}

		_Object(const _Object& other) {
			*this = other;
		}

		template<typename T>
		_Object(const T& any);

		void init(lua_State* L_, int index) {
			L = L_;
			if (L != nullptr) {
				lua_pushvalue(L, index);
				ref = luaL_ref(L, LUA_REGISTRYINDEX);
			}
		}

		_Object& operator=(const _Object& other) {
			// Guard self assignment
			if (this == &other) {
				return *this;
			}

			reset();

			if (other.L != nullptr) {
				L = other.L;
				lua_rawgeti(other.L, LUA_REGISTRYINDEX, other.ref);
				ref = luaL_ref(other.L, LUA_REGISTRYINDEX);
			}

			return *this;
		}

		_Object(_Object&& other) noexcept {
			*this = std::move(other);
		}

		_Object& operator=(_Object&& other) noexcept {
			// Guard self assignment
			if (this == &other) {
				return *this;
			}

			reset();

			L = std::exchange(other.L, nullptr); // leave other in valid state
			ref = std::exchange(other.ref, LUA_REFNIL);
			return *this;
		}

		void reset() {
			if (ref != LUA_REFNIL) {
				if (get_global_state()) {
					luaL_unref(L, LUA_REGISTRYINDEX, ref);
				}
				free();
			}
		}

		void free() {
			L = nullptr;
			ref = LUA_REFNIL;
		}

		void assign(lua_State* L, const _Object& other) {
			reset();

			if (L != nullptr) {
				this->L = L;
				lua_push(L, other);
				ref = luaL_ref(L, LUA_REGISTRYINDEX);
			}
		}

		const bool isnil() const;

		inline bool operator==(const _Object& rhs) const {
			return L == rhs.L && ref == rhs.ref;
		}

		inline bool operator!=(const _Object& rhs) {
			return !(*this == rhs);
		}

		lua_State* L = nullptr;
		int ref = LUA_REFNIL;
	};

	using Object = _Object<0>;
	using Table = _Object<1>;
	using Function = _Object<2>;

	const Object lua_nil;

	struct PushGuard {
		lua_State* L;

		template<typename T>
		PushGuard(lua_State* L, const T& any);

		~PushGuard() {
			lua_pop(L, 1);
		}
	};

	template<int Kind>
	inline _Object<Kind> lua_to(lua_State* L, int index, _Object<Kind>*) {
		return _Object<Kind>(L, index);
	}

	template<int Kind>
	inline int lua_push(lua_State* L, const _Object<Kind>& o) {
		if (o.L == nullptr) {
			lua_pushnil(L);
			return 1;
		}

		lua_rawgeti(o.L, LUA_REGISTRYINDEX, o.ref);

		if (o.L != L) {
			lua_xmove(o.L, L, 1);
		}

		return 1;
	}

	template<int Kind>
	inline int lua_push(lua_State* L, const _Object<Kind>* o) {
		return lua_push(L, *o);
	}

	inline bool lua_is(lua_State* L, int index, Object*) {
		if (index < 0) {
			index += lua_gettop(L) + 1;
		}
		return lua_gettop(L) >= index;
	}

	inline bool lua_is(lua_State* L, int index, Table*) {
		return lua_istable(L, index);
	}

	inline bool lua_is(lua_State* L, int index, Function*) {
		return lua_isfunction(L, index);
	}

	// ================================
	// templated: lua_is, lua_to, lua_push
	// ================================


	// ================================
	// T
	// ================================

	template<typename T>
	inline bool lua_is(lua_State* L, int index, T* ptr);

	template<typename T>
	inline typename std::enable_if<std::is_enum_v<T>, int>::type lua_to(lua_State* L, int index, T* ptr);

	template<typename T>
	inline typename std::enable_if<!std::is_enum_v<T>, std::shared_ptr<T>>::type lua_to(lua_State* L, int index, T* ptr);

	template<typename T>
	inline int lua_push(lua_State* L, T* ptr);

	template<typename T>
	inline typename std::enable_if<is_usertype_v<T>, int>::type lua_push(lua_State* L, T&& obj);

	template<typename T>
	inline typename std::enable_if<is_usertype_v<T>, int>::type lua_push(lua_State* L, const T& obj);

	template<typename T>
	inline typename std::enable_if<std::is_enum_v<T>, int>::type lua_push(lua_State* L, const T& value);


	// ================================
	// T*
	// ================================

	template<typename T>
	inline typename std::enable_if<is_usertype_v<T>, bool>::type lua_is(lua_State* L, int index, T**);

	template<typename T>
	inline typename std::enable_if<is_usertype_v<T>, T*>::type lua_to(lua_State* L, int index, T**);


	// ================================
	// std::shared_ptr
	// ================================

	template<typename T>
	inline bool lua_is(lua_State* L, int index, std::shared_ptr<T>*);

	template<typename T>
	inline std::shared_ptr<T> lua_to(lua_State* L, int index, std::shared_ptr<T>*);

	template<typename T>
	inline int lua_push(lua_State* L, const std::shared_ptr<T>& ptr);


	// ================================
	// std::map
	// ================================

	template<typename K, typename V>
	inline bool lua_is(lua_State* L, int index, std::map<K, V>* ptr);

	template<typename K, typename V>
	inline void lua_to(lua_State* L, int index, std::map<K, V>& out);

	template<typename K, typename V>
	inline std::shared_ptr<std::map<K, V>> lua_to(lua_State* L, int index, std::map<K, V>* ptr);

	template<typename K, typename V>
	inline int lua_push(lua_State* L, std::map<K, V>&& kv);

	template<typename K, typename V>
	inline int lua_push(lua_State* L, const std::map<K, V>& kv);


	// ================================
	// std::optional
	// ================================

	template<typename T>
	inline bool lua_is(lua_State* L, int index, std::optional<T>*);

	template<typename T>
	inline std::optional<T> lua_to(lua_State* L, int index, std::optional<T>*);

	template<typename T>
	inline int lua_push(lua_State* L, const std::optional<T>& p);


	// ================================
	// std::pair
	// ================================

	template<typename T1, typename T2>
	inline bool lua_is(lua_State* L, int index, std::pair<T1, T2>* ptr);

	template<typename T1, typename T2>
	inline std::shared_ptr<std::pair<T1, T2>> lua_to(lua_State* L, int index, std::pair<T1, T2>* ptr);

	template<typename T1, typename T2>
	inline int lua_push(lua_State* L, const std::pair<T1, T2>& p);


	// ================================
	// std::tuple
	// ================================

	template<typename... _Ts>
	inline bool lua_is(lua_State* L, int index, std::tuple<_Ts...>* ptr);

	template<typename... _Ts>
	inline std::shared_ptr<std::tuple<_Ts...>> lua_to(lua_State* L, int index, std::tuple<_Ts...>* ptr);

	template<typename... _Ts>
	inline int lua_push(lua_State* L, const std::tuple<_Ts...>& value);


	// ================================
	// std::variant
	// ================================

	template<typename... _Ts>
	inline bool lua_is(lua_State* L, int index, std::variant<_Ts...>* ptr);

	template<typename... _Ts>
	inline std::variant<_Ts...> lua_to(lua_State* L, int index, std::variant<_Ts...>* ptr);

	template<typename... _Ts>
	inline int lua_push(lua_State* L, const std::variant<_Ts...>& value);


	// ================================
	// stl container
	// ================================

	template<template<typename> typename Container, typename... _Ts>
	inline bool _stl_container_lua_is(lua_State* L, int index, Container<_Ts...>* ptr, size_t len, bool loose);

	template<template<typename> typename Container, typename... _Ts>
	inline void _stl_container_lua_to(lua_State* L, int index, Container<_Ts...>& out);

	template<template<typename> typename Container, typename... _Ts>
	inline std::shared_ptr<Container<_Ts...>> _stl_container_lua_to(lua_State* L, int index, Container<_Ts...>* ptr);

	template<template<typename> typename Container, typename... _Ts>
	inline int _stl_container_lua_push(lua_State* L, Container<_Ts...>&& container);

	template<template<typename> typename Container, typename... _Ts>
	inline int _stl_container_lua_push(lua_State* L, const Container<_Ts...>& container);


	// ================================
	// std::vector
	// ================================

	template<class T, class Allocator = std::allocator<T>>
	inline bool lua_is(lua_State* L, int index, std::vector<T, Allocator>* ptr, size_t len = 0, bool loose = false);

	template<class T, class Allocator = std::allocator<T>>
	inline void lua_to(lua_State* L, int index, std::vector<T, Allocator>& out);

	template<class T, class Allocator = std::allocator<T>>
	inline std::shared_ptr<std::vector<T, Allocator>> lua_to(lua_State* L, int index, std::vector<T, Allocator>* ptr);

	template<class T, class Allocator = std::allocator<T>>
	inline int lua_push(lua_State* L, std::vector<T, Allocator>&& vec);

	template<class T, class Allocator = std::allocator<T>>
	inline int lua_push(lua_State* L, const std::vector<T, Allocator>& vec);


	// ================================
	// std::function
	// ================================

	template<class R, class... Args>
	inline bool lua_is(lua_State* L, int index, std::function<R(Args...)>*);

	template<class R, class... Args>
	inline std::function<R(Args...)> lua_to(lua_State* L, int index, std::function<R(Args...)>*);
}
