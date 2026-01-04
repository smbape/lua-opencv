#pragma once

namespace LUA_MODULE_NAME {
	template<typename Array, bool is_arrays_type>
	struct _OptionalArray;

	template<typename Array, bool is_arrays_type>
	inline void lua_push(lua_State* L, const _OptionalArray<Array, is_arrays_type>& opt_array);
}
