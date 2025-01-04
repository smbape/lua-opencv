#pragma once

#include <lua_bridge_common.hdr.hpp>

namespace LUA_MODULE_NAME {
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
	// exxplicit: lua_is, lua_to, lua_push
	// ================================

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

	template<typename T, typename V>
	inline decltype(auto) extract_holder(T& holder, V*) {
		if constexpr (std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<V>>) {
			return holder;
		}
		else if constexpr (std::is_enum_v<V>) {
			return static_cast<V>(holder);
		}
		else {
			return *holder;
		}
	}

	template<typename T, typename V>
	inline decltype(auto) extract_holder(T& holder, V& defval, bool empty) {
		if constexpr (std::is_same_v<T, V>) {
			return empty ? defval : holder;
		}
		else if constexpr (std::is_enum_v<V>) {
			return empty ? defval : static_cast<V>(holder);
		}
		else {
			return empty ? defval : *holder;
		}
	}

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
		if (lua_type(L, index) != LUA_TNUMBER) {
			return false;
		}
#if LUA_VERSION_NUM >= 503
		// Lua 5.3 and greater checks for numeric precision
		if (!lua_isinteger(L, index)) {
			return false;
		}

		const auto v = lua_tointeger(L, index);
#else
		const lua_Number v = lua_tonumber(L, index);
		// https://stackoverflow.com/questions/1521607/check-double-variable-if-it-contains-an-integer-and-not-floating-point/1521682#1521682
		static double intpart;
		if (std::modf(v, &intpart) != 0.0) {
			return false;
		}
#endif
		using Integer = std::remove_cvref_t<decltype(*ptr)>;
		return v >= std::numeric_limits<Integer>::min() && v <= std::numeric_limits<Integer>::max();
	}

	inline auto lua_to(lua_State* L, int index, std::integral auto* ptr) {
#if LUA_VERSION_NUM >= 503
		// Lua 5.3 and greater checks for numeric precision
		return static_cast<std::remove_cvref_t<decltype(*ptr)>>(lua_tointeger(L, index));
#else
		const lua_Number v = lua_tonumber(L, index);
		return static_cast<std::remove_cvref_t<decltype(*ptr)>>(v);
#endif
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
		return static_cast<std::remove_cvref_t<decltype(*ptr)>>(lua_tonumber(L, index));
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

	inline bool lua_is(lua_State* L, int index, Keywords* ptr) {
		return lua_istable(L, index) && lua_userdata_is(L, index, ptr);
	}

	inline int lua_push(lua_State* L, const char* s) {
		lua_pushstring(L, s);
		return 1;
	}

	template<typename T>
	inline void usertype_push_metatable(lua_State* L) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, usertype_info<T>::metatable);
	}

	// ================================
	// templated: lua_is, lua_to, lua_push
	// ================================

	template<typename T>
	inline bool lua_userdata_is(lua_State* L, int index, T*) {
		if (!lua_getmetatable(L, index)) {
			return false;
		}
		auto signature = lua_topointer(L, -1);
		lua_pop(L, 1);

		auto result = signature == usertype_info<T>::signature;

		if constexpr (requires(const void* signature) { usertype_info<T>::derives.find(signature); }) {
			if (!result && usertype_info<T>::derives.find(signature) != usertype_info<T>::derives.end()) {
				result = true;
			}
		}

		return result;
	}

	template<typename T>
	inline bool lua_is(lua_State* L, int index, T* ptr) {
		if constexpr (std::is_enum_v<T>) {
			return lua_is(L, index, static_cast<int*>(nullptr));
		}
		else {
			return lua_isuserdata(L, index) && lua_userdata_is(L, index, ptr);
		}
	}

	template<typename T>
	inline std::shared_ptr<T>& lua_userdata_to(lua_State* L, int index, T*) {
		return *static_cast<std::shared_ptr<T>*>(lua_touserdata(L, index));
	}

	template<typename T>
	inline typename std::enable_if<std::is_enum_v<T>, int>::type lua_to(lua_State* L, int index, T* ptr) {
		return lua_to(L, index, static_cast<int*>(nullptr));
	}

	template<typename T>
	inline typename std::enable_if<!std::is_enum_v<T>, std::shared_ptr<T>>::type lua_to(lua_State* L, int index, T* ptr) {
		return lua_userdata_to(L, index, ptr);
	}

	template<typename T>
	inline bool lua_is(lua_State* L, int index, std::shared_ptr<T>*) {
		return lua_is(L, index, static_cast<T*>(nullptr));
	}

	template<typename T>
	inline std::shared_ptr<T> lua_to(lua_State* L, int index, std::shared_ptr<T>*) {
		return lua_to(L, index, static_cast<T*>(nullptr));
	}

	template<typename T>
	inline int lua_push(lua_State* L, const std::shared_ptr<T>& ptr) {
		using SharedPtr = std::shared_ptr<T>;
		auto userdata_ptr = static_cast<SharedPtr*>(lua_newuserdata(L, sizeof(SharedPtr)));
		new(userdata_ptr) SharedPtr(ptr); // userdata = new std::shared_ptr<T>(ptr)

		usertype_push_metatable<T>(L);
		lua_setmetatable(L, -2);

		return 1; // return userdata
	}

	template<typename T>
	inline int lua_push(lua_State* L, T* ptr) {
		return lua_push(L, reference_internal(ptr));
	}

	template<typename T>
	inline typename std::enable_if<is_usertype_v<T>, bool>::type lua_is(lua_State* L, int index, T**) {
		return lua_is(L, index, static_cast<T*>(nullptr));
	}

	template<typename T>
	inline typename std::enable_if<is_usertype_v<T>, T*>::type lua_to(lua_State* L, int index, T**) {
		return lua_to(L, index, static_cast<T*>(nullptr)).get();
	}

	template<typename T>
	inline typename std::enable_if<is_usertype_v<T>, int>::type lua_push(lua_State* L, T&& obj) {
		return lua_push(L, std::make_shared<T>(std::move(obj)));
	}

	template<typename T>
	inline typename std::enable_if<is_usertype_v<T>, int>::type lua_push(lua_State* L, const T& obj) {
		return lua_push(L, std::make_shared<T>(obj));
	}

	template<typename T>
	inline typename std::enable_if<std::is_enum_v<T>, int>::type lua_push(lua_State* L, const T& value) {
		return lua_push(L, static_cast<int>(value));
	}

	// ================================
	// std::map
	// ================================

	template<typename K, typename V>
	inline bool lua_is(lua_State* L, int index, std::map<K, V>* ptr) {
		if (lua_isuserdata(L, index)) {
			return lua_userdata_is(L, index, ptr);
		}

		if (!lua_istable(L, index)) {
			return false;
		}

		// Push another reference to the table on top of the stack (so we know
		// where it is, and this function can work for negative, positive and
		// pseudo indices
		lua_pushvalue(L, index);
		// stack now contains: -1 => table

		lua_pushnil(L);
		// stack now contains: -1 => nil; -2 => table

		while (lua_next(L, -2)) {
			// stack now contains: -1 => value; -2 => key; -3 => table

			const auto is_valid = lua_is(L, -2, static_cast<K*>(nullptr)) || !lua_is(L, -1, static_cast<V*>(nullptr));

			// pop value
			lua_pop(L, 1);
			// stack now contains: -1 => key; -2 => table

			if (!is_valid) {
				// pop key
				lua_pop(L, 1);
				// stack now contains: -1 => table
				break;
			}
		}

		// stack now contains: -1 => table (when lua_next returns 0 it pops the key
		// but does not push anything.)
		// Pop table
		lua_pop(L, 1);
		// Stack is now the same as it was on entry to this function

		return true;
	}

	template<typename K, typename V>
	inline std::shared_ptr<std::map<K, V>> lua_to(lua_State* L, int index, std::map<K, V>* ptr) {
		if (lua_isuserdata(L, index)) {
			return lua_userdata_to(L, index, ptr);
		}

		if (index < 0) {
			index += lua_gettop(L) + 1;
		}

		static std::map<K, V> res;
		res.clear();

		// Push another reference to the table on top of the stack (so we know
		// where it is, and this function can work for negative, positive and
		// pseudo indices
		lua_pushvalue(L, index);
		// stack now contains: -1 => table

		lua_pushnil(L);
		// stack now contains: -1 => nil; -2 => table

		while (lua_next(L, -2)) {
			// stack now contains: -1 => value; -2 => key; -3 => table
			const auto key = lua_to(L, -2, static_cast<K*>(nullptr));
			const auto value = lua_to(L, -1, static_cast<V*>(nullptr));

			if constexpr (std::is_same_v<V, std::remove_cvref_t<decltype(value)>>) {
				res.insert_or_assign(key, value);
			}
			else {
				res.insert_or_assign(key, *value);
			}

			// pop value
			lua_pop(L, 1);
			// stack now contains: -1 => key; -2 => table
		}

		// stack now contains: -1 => table (when lua_next returns 0 it pops the key
		// but does not push anything.)
		// Pop table
		lua_pop(L, 1);
		// Stack is now the same as it was on entry to this function

		return std::make_shared<std::map<K, V>>(res);
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
	inline bool lua_is(lua_State* L, int index, std::optional<T>*) {
		return lua_isnil(L, index) || lua_is(L, index, static_cast<T*>(nullptr));
	}

	template<typename T>
	inline std::optional<T> lua_to(lua_State* L, int index, std::optional<T>*) {
		if (lua_isnil(L, index)) {
			return std::nullopt;
		}
		return lua_to(L, index, static_cast<T*>(nullptr));
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
	inline bool lua_is(lua_State* L, int index, std::pair<T1, T2>* ptr) {
		if (!lua_istable(L, index)) {
			return false;
		}

		auto size = lua_rawlen(L, index);
		if (size != 2) {
			return false;
		}

		if (index < 0) {
			index += lua_gettop(L) + 1;
		}

		for (auto i = 1; i <= size; ++i) {
			lua_pushnumber(L, i);
			lua_rawget(L, index);
			auto is_valid = i == 1 ? lua_is(L, -1, static_cast<T1*>(nullptr)) : lua_is(L, -1, static_cast<T2*>(nullptr));
			lua_pop(L, 1);
			if (!is_valid) {
				return false;
			}
		}

		return true;
	}

	template<typename T1, typename T2>
	inline std::shared_ptr<std::pair<T1, T2>> lua_to(lua_State* L, int index, std::pair<T1, T2>* ptr) {
		if (index < 0) {
			index += lua_gettop(L) + 1;
		}

		auto size = lua_rawlen(L, index);
		static std::pair<T1, T2> res;

		for (auto i = 1; i <= size; ++i) {
			lua_pushnumber(L, i);
			lua_rawget(L, index);
			if (i == 1) {
				auto value = lua_to(L, -1, static_cast<T1*>(nullptr));
				if constexpr (std::is_same_v<T1, std::remove_cvref_t<decltype(value)>>) {
					res.first = value;
				}
				else {
					res.first = *value;
				}
			}
			else {
				auto value = lua_to(L, -1, static_cast<T2*>(nullptr));
				if constexpr (std::is_same_v<T2, std::remove_cvref_t<decltype(value)>>) {
					res.second = value;
				}
				else {
					res.second = *value;
				}
			}
			lua_pop(L, 1);
		}

		return std::make_shared<std::pair<T1, T2>>(res.first, res.second);
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
	inline bool _lua_is(lua_State* L, int index, std::tuple<_Ts...>* ptr) {
		using _Tuple = typename std::tuple<_Ts...>;
		using T = typename std::tuple_element<I, _Tuple>::type;

		lua_pushnumber(L, I);
		lua_rawget(L, index);
		auto is_valid = lua_is(L, -1, static_cast<T*>(nullptr));
		lua_pop(L, 1);

		if constexpr (I == sizeof...(_Ts) - 1) {
			return is_valid;
		}
		else {
			if (!is_valid) {
				return false;
			}
			return _lua_is<I + 1, _Ts...>(L, index, ptr);
		}
	}

	template<typename... _Ts>
	inline bool lua_is(lua_State* L, int index, std::tuple<_Ts...>* ptr) {
		if (!lua_istable(L, index)) {
			return false;
		}

		if (index < 0) {
			index += lua_gettop(L) + 1;
		}

		return _lua_is(L, index, ptr);
	}

	template<std::size_t I = 0, typename... _Ts>
	inline void _lua_to(lua_State* L, int index, std::tuple<_Ts...>& res) {
		using _Tuple = typename std::tuple<_Ts...>;
		using T = typename std::tuple_element<I, _Tuple>::type;

		lua_pushnumber(L, I + 1);
		lua_rawget(L, index);

		auto value = lua_to(L, -1, static_cast<T*>(nullptr));

		if constexpr (std::is_same_v<T, std::remove_cvref_t<decltype(value)>>) {
			std::get<I>(res) = value;
		}
		else {
			std::get<I>(res) = *value;
		}

		lua_pop(L, 1);

		if constexpr (I != sizeof...(_Ts) - 1) {
			_lua_to<I + 1, _Ts...>(L, index, res);
		}
	}

	template<typename... _Ts>
	inline std::shared_ptr<std::tuple<_Ts...>> lua_to(lua_State* L, int index, std::tuple<_Ts...>* ptr) {
		if (index < 0) {
			index += lua_gettop(L) + 1;
		}

		static std::tuple<_Ts...> res;
		_lua_to(L, index, res);
		return std::make_shared<std::tuple<_Ts...>>(res);
	}

	template<std::size_t I = 0, typename... _Ts>
	inline void _lua_push(lua_State* L, int index, const std::tuple<_Ts...>& value) {
		using _Tuple = typename std::tuple<_Ts...>;
		using T = typename std::tuple_element<I, _Tuple>::type;

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
	inline bool _lua_is(lua_State* L, int index, std::variant<_Ts...>* ptr) {
		using _Tuple = typename std::tuple<_Ts...>;
		using T = typename std::tuple_element<I, _Tuple>::type;

		if constexpr (!std::is_same_v<std::monostate, T>) {
			if (lua_is(L, index, static_cast<T*>(nullptr))) {
				return true;
			}
		}

		if constexpr (I == sizeof...(_Ts) - 1) {
			return false;
		}
		else {
			return _lua_is<I + 1, _Ts...>(L, index, ptr);
		}
	}

	template<typename... _Ts>
	inline bool lua_is(lua_State* L, int index, std::variant<_Ts...>* ptr) {
		return _lua_is(L, index, ptr);
	}

	template<std::size_t I = 0, typename... _Ts>
	inline std::variant<_Ts...> _lua_to(lua_State* L, int index, std::variant<_Ts...>* ptr) {
		using _Tuple = typename std::tuple<_Ts...>;
		using T = typename std::tuple_element<I, _Tuple>::type;

		if constexpr (!std::is_same_v<std::monostate, T>) {
			if (lua_is(L, index, static_cast<T*>(nullptr))) {
				auto holder = lua_to(L, index, static_cast<T*>(nullptr));
				return extract_holder(holder, static_cast<T*>(nullptr));
			}
		}

		if constexpr (I == sizeof...(_Ts) - 1) {
			using Arg = typename std::variant<_Ts...>;
			return Arg();
		}
		else {
			return _lua_to<I + 1, _Ts...>(L, index, ptr);
		}
	}

	template<typename... _Ts>
	inline std::variant<_Ts...> lua_to(lua_State* L, int index, std::variant<_Ts...>* ptr) {
		return _lua_to(L, index, ptr);
	}

	template<std::size_t I = 0, typename... _Ts>
	inline int _lua_push(lua_State* L, const std::variant<_Ts...>& value) {
		using _Tuple = typename std::tuple<_Ts...>;
		using T = typename std::tuple_element<I, _Tuple>::type;

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
	// std::vector
	// ================================

	template<typename T>
	inline bool lua_is(lua_State* L, int index, std::vector<T>* ptr, size_t len, bool loose) {
		if (lua_isuserdata(L, index)) {
			return lua_userdata_is(L, index, ptr) && (len == 0 || lua_userdata_to(L, index, ptr)->size() <= len);
		}

		if (!lua_istable(L, index)) {
			return false;
		}

		auto size = lua_rawlen(L, index);
		if (len != 0 && (loose ? size > len : size != len)) {
			return false;
		}

		if (index < 0) {
			index += lua_gettop(L) + 1;
		}

		for (auto i = 1; i <= size; ++i) {
			lua_pushnumber(L, i);
			lua_rawget(L, index);
			auto is_valid = lua_is(L, -1, static_cast<T*>(nullptr));
			lua_pop(L, 1);
			if (!is_valid) {
				return false;
			}
		}

		return true;
	}

	template<typename T>
	inline void lua_to(lua_State* L, int index, std::vector<T>& out) {
		if (lua_isuserdata(L, index)) {
			out = *lua_userdata_to(L, index, static_cast<std::vector<T>*>(nullptr));
			return;
		}

		if (index < 0) {
			index += lua_gettop(L) + 1;
		}

		auto size = lua_rawlen(L, index);
		out.resize(size);

		for (auto i = 1; i <= size; ++i) {
			lua_pushnumber(L, i);
			lua_rawget(L, index);
			auto value = lua_to(L, -1, static_cast<T*>(nullptr));
			out[i - 1] = extract_holder(value, static_cast<T*>(nullptr));
			lua_pop(L, 1);
		}
	}

	template<typename T>
	inline std::shared_ptr<std::vector<T>> lua_to(lua_State* L, int index, std::vector<T>* ptr) {
		if (lua_isuserdata(L, index)) {
			return lua_userdata_to(L, index, ptr);
		}

		static std::vector<T> res;
		lua_to(L, index, res);
		return std::make_shared<std::vector<T>>(res);
	}

	template<typename T>
	inline int lua_push(lua_State* L, std::vector<T>&& vec) {
		std::vector<T> _vec(std::move(vec));
		return lua_push(L, _vec);
	}

	template<typename T>
	inline int lua_push(lua_State* L, const std::vector<T>& vec) {
		lua_newtable(L);
		int index = lua_gettop(L);
		int i = 0;
		for (const auto& v : vec) {
			lua_push(L, v);
			lua_rawseti(L, index, ++i);
		}
		return 1;
	}

	template<typename T>
	int lua_method_isinstance(lua_State* L) {
		return lua_push(L, lua_gettop(L) == 1 && lua_is(L, 1, static_cast<T*>(nullptr)));
	}

	template<typename T>
	int lua_method__gc(lua_State* L) {
		using SharedPtr = std::shared_ptr<T>;
		auto userdata_ptr = static_cast<SharedPtr*>(lua_touserdata(L, 1));
		userdata_ptr->reset();
		userdata_ptr->~SharedPtr();
		return 0;
	}

	template<std::size_t I = 0, typename... _Ts>
	int lua_class__index(lua_State* L) {
		using _Tuple = typename std::tuple<_Ts...>;
		using T = typename std::tuple_element<I, _Tuple>::type;

		if constexpr (is_usertype_v<T>) {
			// for instantiable classes: lookup in the raw porperties of the metatable
			// =================================================
			usertype_push_metatable<T>(L); // push the metatable
			lua_pushvalue(L, 2); // push the key
			lua_rawget(L, -2);
			lua_remove(L, -2); // remove metatable

			if (!lua_isnil(L, -1)) {
				return 1; // return metatable[key]
			}

			lua_pop(L, 1); // pop nil

			// for instantiable classes: lookup in the registered getters of the class 
			// =================================================
			if (lua_is(L, 2, static_cast<std::string*>(nullptr))) {
				const std::string k = lua_to(L, 2, static_cast<std::string*>(nullptr));
				if (usertype_info<T>::getters.count(k)) {
					return usertype_info<T>::getters.at(k)(L);
				}
			}

			if constexpr (I == sizeof...(_Ts) - 1) {
				lua_pushnil(L);
			}
		}
		else {
			// for static classes: lookup in the porperties of the metatable
			// =================================================
			basetype_info<T>::push(L); // push the metatable
			lua_pushvalue(L, 2); // push the key
			lua_gettable(L, -2); // pop the key, push metatable[key]
			lua_remove(L, -2); // remove metatable

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
		using T = typename std::tuple_element<I, _Tuple>::type;

		if constexpr (is_usertype_v<T>) {
			// call the setter if defined
			if (lua_is(L, 2, static_cast<std::string*>(nullptr))) {
				const std::string k = lua_to(L, 2, static_cast<std::string*>(nullptr));
				if (usertype_info<T>::setters.count(k)) {
					return usertype_info<T>::setters.at(k)(L);
				}
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
		return lua_push(L, lua_userdata_to(L, 1, static_cast<T*>(nullptr))->to_string());
	}

	/**
	 * https://github.com/ThePhD/sol2/blob/v3.3.0/include/sol/stack_core.hpp#L1352
	 */
	template <typename T>
	int adl_default_to_string(lua_State* L) {
		return lua_push(L, std::to_string(*lua_userdata_to(L, 1, static_cast<T*>(nullptr))));
	}

	/**
	 * https://github.com/ThePhD/sol2/blob/v3.3.0/include/sol/stack_core.hpp#L1364
	 */
	template<typename T>
	int oss_default_to_string(lua_State* L) {
		std::ostringstream oss;
		oss << *lua_userdata_to(L, 1, static_cast<T*>(nullptr));
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
		if (index == 0 || index > vec.size()) {
			luaL_error(L, "index out of range");
		}
		return vec.at(index - 1);
	}

	/**
	 * https://en.cppreference.com/w/cpp/string/byte/atoi
	 */
	template<typename T>
	decltype(auto) lua_vector_method__index(lua_State* L, const std::vector<T>& vec, const std::string& s) {
		const char* str = s.c_str();
		auto len = s.length();

		size_t index = 0;
		decltype(len) i = 0;

		for (; i < len && std::isdigit(static_cast<unsigned char>(*str)); ++str) {
			int digit = *str - '0';
			index *= 10;
			index += digit;
			i++;
		}

		if (i != len) {
			luaL_error(L, "invalid index %s", s.c_str());
		}

		return lua_vector_method__index(L, vec, index);
	}

	template<typename T>
	void lua_vector_method__newindex(lua_State* L, std::vector<T>& vec, size_t index, const T& value) {
		if (index != vec.size() + 1) {
			luaL_error(L, "index out of range");
		}
		vec.push_back(value);
	}

	template<typename T>
	void lua_vector_method__newindex(lua_State* L, std::vector<T>& vec, const std::string& s, const T& value) {
		const char* str = s.c_str();
		auto len = s.length();

		size_t index = 0;
		decltype(len) i = 0;

		for (; i < len && std::isdigit(static_cast<unsigned char>(*str)); ++str) {
			int digit = *str - '0';
			index *= 10;
			index += digit;
			i++;
		}

		if (i != len) {
			luaL_error(L, "invalid index %s", s.c_str());
		}

		lua_vector_method__newindex(L, vec, index, value);
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

		// methods
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
