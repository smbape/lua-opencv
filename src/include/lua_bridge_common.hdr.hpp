#pragma once

#include <luadef.hpp>
#include <Keywords.hpp>
#include <lua_generated_include.hpp>
#include <register_all.hpp>

namespace LUA_MODULE_NAME {
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

	template<typename T>
	inline bool lua_userdata_is(lua_State* L, int index, T*);

	template<typename T>
	inline std::shared_ptr<T>& lua_userdata_to(lua_State* L, int index, T*);

	template<typename T>
	inline bool lua_is(lua_State* L, int index, T* ptr);

	template<typename T>
	inline bool lua_is(lua_State* L, int index, std::shared_ptr<T>*);

	template<typename K, typename V>
	inline bool lua_is(lua_State* L, int index, std::map<K, V>* ptr);

	template<typename T>
	inline bool lua_is(lua_State* L, int index, std::optional<T>*);

	template<typename T1, typename T2>
	inline bool lua_is(lua_State* L, int index, std::pair<T1, T2>* ptr);

	template<typename... _Ts>
	inline bool lua_is(lua_State* L, int index, std::tuple<_Ts...>* ptr);

	template<typename... _Ts>
	inline bool lua_is(lua_State* L, int index, std::variant<_Ts...>* ptr);

	template<typename T>
	inline bool lua_is(lua_State* L, int index, std::vector<T>* ptr, size_t len = 0, bool loose = false);


	template<typename T>
	inline typename std::enable_if<std::is_enum_v<T>, int>::type lua_to(lua_State* L, int index, T* ptr);

	template<typename T>
	inline typename std::enable_if<!std::is_enum_v<T>, std::shared_ptr<T>>::type lua_to(lua_State* L, int index, T* ptr);

	template<typename T>
	inline std::shared_ptr<T> lua_to(lua_State* L, int index, std::shared_ptr<T>*);

	template<typename T>
	inline typename std::enable_if<is_usertype_v<T>, bool>::type lua_is(lua_State* L, int index, T**);

	template<typename T>
	inline typename std::enable_if<is_usertype_v<T>, T*>::type lua_to(lua_State* L, int index, T**);

	template<typename K, typename V>
	inline std::shared_ptr<std::map<K, V>> lua_to(lua_State* L, int index, std::map<K, V>* ptr);

	template<typename T>
	inline std::optional<T> lua_to(lua_State* L, int index, std::optional<T>*);

	template<typename T1, typename T2>
	inline std::shared_ptr<std::pair<T1, T2>> lua_to(lua_State* L, int index, std::pair<T1, T2>* ptr);

	template<typename... _Ts>
	inline std::shared_ptr<std::tuple<_Ts...>> lua_to(lua_State* L, int index, std::tuple<_Ts...>* ptr);

	template<typename... _Ts>
	inline std::variant<_Ts...> lua_to(lua_State* L, int index, std::variant<_Ts...>* ptr);

	template<typename T>
	inline void lua_to(lua_State* L, int index, std::vector<T>& out);

	template<typename T>
	inline std::shared_ptr<std::vector<T>> lua_to(lua_State* L, int index, std::vector<T>* ptr);


	template<typename T>
	inline int lua_push(lua_State* L, const std::shared_ptr<T>& ptr);

	template<typename T>
	inline int lua_push(lua_State* L, T* ptr);

	template<typename T>
	inline typename std::enable_if<is_usertype_v<T>, int>::type lua_push(lua_State* L, T&& obj);

	template<typename T>
	inline typename std::enable_if<is_usertype_v<T>, int>::type lua_push(lua_State* L, const T& obj);

	template<typename T>
	inline typename std::enable_if<std::is_enum_v<T>, int>::type lua_push(lua_State* L, const T& value);

	template<typename K, typename V>
	inline int lua_push(lua_State* L, std::map<K, V>&& kv);

	template<typename K, typename V>
	inline int lua_push(lua_State* L, const std::map<K, V>& kv);

	template<typename T>
	inline int lua_push(lua_State* L, const std::optional<T>& p);

	template<typename T1, typename T2>
	inline int lua_push(lua_State* L, const std::pair<T1, T2>& p);

	template<typename... _Ts>
	inline int lua_push(lua_State* L, const std::tuple<_Ts...>& value);

	template<typename... _Ts>
	inline int lua_push(lua_State* L, const std::variant<_Ts...>& value);

	template<typename T>
	inline int lua_push(lua_State* L, std::vector<T>&& vec);

	template<typename T>
	inline int lua_push(lua_State* L, const std::vector<T>& vec);
}
