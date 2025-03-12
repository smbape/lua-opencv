#pragma once

#include <lua_bridge_common.hpp>
#include <opencv2/opencv.hpp>

/**
 * Internal class used to create
 *   InputArray, OutputArray, InputOutputArray
 *   InputArrayOfArrays, OutputArrayOfArrays, InputOutputArrayOfArrays
 * It holds the Array data until destruction.
 * Array defaults to cv::Mat for *Array and std::vector<cv::Mat> for *Arrays
 */

namespace LUA_MODULE_NAME {
	template<typename Array, bool is_arrays_type>
	class _OptionalArray {
	public:
		_OptionalArray();
		~_OptionalArray();

		_OptionalArray(_OptionalArray& other);
		_OptionalArray(_OptionalArray&& other) noexcept;
		_OptionalArray(const _OptionalArray& other) = delete;

		_OptionalArray& operator=(_OptionalArray& other);
		_OptionalArray& operator=(_OptionalArray&& other) noexcept;
		_OptionalArray& operator=(const _OptionalArray& other) = delete;

		// Array parameter default value
		//
		// cv.omnidir.undistortImage void
		//	 InputArray distorted
		//	 OutputArray undistorted /O
		//	 InputArray K
		//	 InputArray D
		//	 InputArray xi
		//	 int flags
		//	 InputArray Knew cv::noArray()
		//	 Size new_size Size()/C; /Ref
		//	 InputArray R Mat::eye(3, 3, CV_64F)
		template<typename T>
		typename std::enable_if<!std::is_same_v<T, _OptionalArray>, _OptionalArray&>::type operator=(T& value) {
			reset(value);
			return *this;
		}

		_OptionalArray(lua_State* L, int index, bool& is_valid, const bool nd_mat = false);

		void reset();

		operator bool() const {
			return static_cast<bool>(m_ptr);
		}

		decltype(auto) get() {
			return m_ptr.get();
		}

		decltype(auto) operator*() {
			return m_ptr.operator*();
		}

		const int index() const {
			return m_index;
		}

		const void* data() const {
			return m_data;
		}

		const bool is_shared_ptr() const {
			return m_is_shared_ptr;
		}

	private:
		void initialize();
		void free();

		template<typename T>
		void reset(T& value);

		template<typename T>
		void reset(T&& value);

		int m_index { -1 };
		void* m_data { nullptr };
		bool m_is_shared_ptr { false };
		std::shared_ptr<Array> m_ptr;
	};

	template<typename Array, bool is_arrays_type>
	int lua_push(lua_State* L, const _OptionalArray<Array, is_arrays_type>& opt_array);

	// InputArray, OutputArray, InputOutputArray
	template<typename Array>
	using OptionalArray = _OptionalArray<Array, false>;

	// InputArrayOfArrays, OutputArrayOfArrays, InputOutputArrayOfArrays
	template<typename Array>
	using OptionalArrays = _OptionalArray<Array, true>;

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
