#pragma once

namespace LUA_MODULE_NAME {
	template<typename Array, bool is_arrays_type>
	struct _OptionalArray;

	// InputArray, OutputArray, InputOutputArray
	template<typename Array>
	using OptionalArray = _OptionalArray<Array, false>;

	// InputArrayOfArrays, OutputArrayOfArrays, InputOutputArrayOfArrays
	template<typename Array>
	using OptionalArrays = _OptionalArray<Array, true>;

	template<typename Array, bool is_arrays_type>
	void lua_push(lua_State* L, const _OptionalArray<Array, is_arrays_type>& opt_array);

	template<typename Array>
	inline auto lua_toarray(lua_State* L, int index, Array*, bool& is_valid) {
		// COPY or MOVE constructor/assignment
		return OptionalArray<Array>(L, index, is_valid);
	}

	template<typename Array>
	inline auto lua_toarrays(lua_State* L, int index, Array*, bool& is_valid, const bool nd_mat = false) {
		// COPY or MOVE constructor/assignment
		return OptionalArrays<Array>(L, index, is_valid, nd_mat);
	}
}
