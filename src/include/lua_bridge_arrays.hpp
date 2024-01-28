#pragma once

#include <lua_bridge_common.hpp>
#include <opencv2/core.hpp>

namespace LUA_MODULE_NAME {
	// InputArray, outputArray, InputOutputArray
	// InputArrayOfArrays, outputArrayOfArrays, InputOutputArrayOfArrays
	template<typename Array, bool is_arrays_type>
	struct _OptionalArray
	{
		static const bool is_valid(lua_State* L, int index) {
			if (lua_isnil(L, index)) {
				return true;
			}

			if (lua_isuserdata(L, index) && lua_is(L, index, static_cast<Array*>(nullptr))) {
				return true;
			}

			if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {
				if (lua_is(L, index, static_cast<bool*>(nullptr))) {
					return true;
				}
			}

			if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {
				if (lua_is(L, index, static_cast<double*>(nullptr))) {
					return true;
				}
			}

			if constexpr (!is_arrays_type) {
				if (lua_is(L, index, static_cast<cv::cuda::GpuMat*>(nullptr))) {
					return true;
				}
			}

			if constexpr (!is_arrays_type) {
				if (lua_is(L, index, static_cast<cv::Mat*>(nullptr))) {
					return true;
				}
			}

			if constexpr (!is_arrays_type) {
				if (lua_is(L, index, static_cast<cv::UMat*>(nullptr))) {
					return true;
				}
			}

			if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {
				if (lua_is(L, index, static_cast<cv::Scalar*>(nullptr))) {
					return true;
				}
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Mat>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::UMat>*>(nullptr))) {
				return true;
			}

			if constexpr (std::is_same_v<Array, cv::_InputArray>) {
				if (lua_is(L, index, static_cast<std::vector<bool>*>(nullptr))) {
					return true;
				}
			}

			if (lua_is(L, index, static_cast<std::vector<cv::RotatedRect>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Range>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Moments>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<uchar>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<schar>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<char>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<ushort>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<short>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<int>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<float>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<double>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Point3i>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Point3f>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Point3d>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Point2f>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Point2d>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Point>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Rect2f>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Rect2d>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Rect>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Size2f>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Size2d>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Size>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec2b>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec3b>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec4b>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec2s>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec3s>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec4s>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec2w>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec3w>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec4w>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec2i>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec3i>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec4i>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec6i>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec8i>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec2f>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec3f>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec4f>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec6f>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec2d>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec3d>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec4d>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec6d>*>(nullptr))) {
				return true;
			}

			if constexpr (std::is_same_v<Array, cv::_InputOutputArray>) {
				if (lua_is(L, index, static_cast<std::vector<std::vector<bool>>*>(nullptr))) {
					return true;
				}
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::RotatedRect>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Range>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Moments>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<uchar>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<schar>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<char>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<ushort>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<short>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<int>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<float>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<double>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Point3i>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Point3f>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Point3d>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Point2f>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Point2d>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Point>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Rect2f>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Rect2d>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Rect>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Size2f>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Size2d>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Size>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec2b>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec3b>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec4b>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec2s>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec3s>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec4s>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec2w>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec3w>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec4w>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec2i>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec3i>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec4i>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec6i>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec8i>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec2f>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec3f>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec4f>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec6f>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec2d>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec3d>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec4d>>*>(nullptr))) {
				return true;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec6d>>*>(nullptr))) {
				return true;
			}

			return false;
		}

		_OptionalArray() {
			if constexpr (is_arrays_type) {
				setField(*this, *this, 101);
			}
			else {
				setField(*this, *this, 102);
			}
		};

		_OptionalArray(lua_State* L, int index) {
			if (lua_isnil(L, index)) {
				if constexpr (is_arrays_type) {
					setField(*this, *this, 101);
				}
				else {
					setField(*this, *this, 102);
				}
				return;
			}

			if (lua_isuserdata(L, index) && lua_is(L, index, static_cast<Array*>(nullptr))) {
				ptr = lua_to(L, index, static_cast<Array*>(nullptr));
				return;
			}

			if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {
				if (lua_is(L, index, static_cast<bool*>(nullptr))) {
					auto value = lua_to(L, index, static_cast<bool*>(nullptr));
					assign_maybe_shared(this->obj, value, static_cast<bool*>(nullptr));
					setField(*this, *this, 1);
					return;
				}
			}

			if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {
				if (lua_is(L, index, static_cast<double*>(nullptr))) {
					auto value = lua_to(L, index, static_cast<double*>(nullptr));
					assign_maybe_shared(this->obj, value, static_cast<double*>(nullptr));
					setField(*this, *this, 2);
					return;
				}
			}

			if constexpr (!is_arrays_type) {
				if (lua_is(L, index, static_cast<cv::cuda::GpuMat*>(nullptr))) {
					auto value = lua_to(L, index, static_cast<cv::cuda::GpuMat*>(nullptr));
					assign_maybe_shared(this->obj, value, static_cast<cv::cuda::GpuMat*>(nullptr));
					setField(*this, *this, 3);
					return;
				}
			}

			if constexpr (!is_arrays_type) {
				if (lua_is(L, index, static_cast<cv::Mat*>(nullptr))) {
					auto value = lua_to(L, index, static_cast<cv::Mat*>(nullptr));
					assign_maybe_shared(this->obj, value, static_cast<cv::Mat*>(nullptr));
					setField(*this, *this, 4);
					return;
				}
			}

			if constexpr (!is_arrays_type) {
				if (lua_is(L, index, static_cast<cv::UMat*>(nullptr))) {
					auto value = lua_to(L, index, static_cast<cv::UMat*>(nullptr));
					assign_maybe_shared(this->obj, value, static_cast<cv::UMat*>(nullptr));
					setField(*this, *this, 5);
					return;
				}
			}

			if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {
				if (lua_is(L, index, static_cast<cv::Scalar*>(nullptr))) {
					auto value = lua_to(L, index, static_cast<cv::Scalar*>(nullptr));
					assign_maybe_shared(this->obj, value, static_cast<cv::Scalar*>(nullptr));
					setField(*this, *this, 6);
					return;
				}
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Mat>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Mat>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Mat>*>(nullptr));
				setField(*this, *this, 7);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::UMat>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::UMat>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::UMat>*>(nullptr));
				setField(*this, *this, 8);
				return;
			}

			if constexpr (std::is_same_v<Array, cv::_InputArray>) {
				if (lua_is(L, index, static_cast<std::vector<bool>*>(nullptr))) {
					auto value = lua_to(L, index, static_cast<std::vector<bool>*>(nullptr));
					assign_maybe_shared(this->obj, value, static_cast<std::vector<bool>*>(nullptr));
					setField(*this, *this, 9);
					return;
				}
			}

			if (lua_is(L, index, static_cast<std::vector<cv::RotatedRect>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::RotatedRect>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::RotatedRect>*>(nullptr));
				setField(*this, *this, 10);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Range>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Range>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Range>*>(nullptr));
				setField(*this, *this, 11);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Moments>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Moments>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Moments>*>(nullptr));
				setField(*this, *this, 12);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<uchar>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<uchar>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<uchar>*>(nullptr));
				setField(*this, *this, 13);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<schar>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<schar>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<schar>*>(nullptr));
				setField(*this, *this, 14);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<char>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<char>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<char>*>(nullptr));
				setField(*this, *this, 15);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<ushort>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<ushort>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<ushort>*>(nullptr));
				setField(*this, *this, 16);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<short>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<short>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<short>*>(nullptr));
				setField(*this, *this, 17);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<int>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<int>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<int>*>(nullptr));
				setField(*this, *this, 18);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<float>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<float>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<float>*>(nullptr));
				setField(*this, *this, 19);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<double>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<double>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<double>*>(nullptr));
				setField(*this, *this, 20);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Point3i>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Point3i>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Point3i>*>(nullptr));
				setField(*this, *this, 21);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Point3f>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Point3f>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Point3f>*>(nullptr));
				setField(*this, *this, 22);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Point3d>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Point3d>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Point3d>*>(nullptr));
				setField(*this, *this, 23);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Point2f>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Point2f>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Point2f>*>(nullptr));
				setField(*this, *this, 24);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Point2d>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Point2d>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Point2d>*>(nullptr));
				setField(*this, *this, 25);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Point>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Point>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Point>*>(nullptr));
				setField(*this, *this, 26);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Rect2f>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Rect2f>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Rect2f>*>(nullptr));
				setField(*this, *this, 27);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Rect2d>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Rect2d>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Rect2d>*>(nullptr));
				setField(*this, *this, 28);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Rect>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Rect>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Rect>*>(nullptr));
				setField(*this, *this, 29);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Size2f>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Size2f>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Size2f>*>(nullptr));
				setField(*this, *this, 30);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Size2d>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Size2d>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Size2d>*>(nullptr));
				setField(*this, *this, 31);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Size>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Size>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Size>*>(nullptr));
				setField(*this, *this, 32);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec2b>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Vec2b>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Vec2b>*>(nullptr));
				setField(*this, *this, 33);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec3b>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Vec3b>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Vec3b>*>(nullptr));
				setField(*this, *this, 34);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec4b>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Vec4b>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Vec4b>*>(nullptr));
				setField(*this, *this, 35);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec2s>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Vec2s>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Vec2s>*>(nullptr));
				setField(*this, *this, 36);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec3s>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Vec3s>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Vec3s>*>(nullptr));
				setField(*this, *this, 37);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec4s>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Vec4s>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Vec4s>*>(nullptr));
				setField(*this, *this, 38);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec2w>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Vec2w>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Vec2w>*>(nullptr));
				setField(*this, *this, 39);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec3w>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Vec3w>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Vec3w>*>(nullptr));
				setField(*this, *this, 40);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec4w>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Vec4w>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Vec4w>*>(nullptr));
				setField(*this, *this, 41);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec2i>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Vec2i>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Vec2i>*>(nullptr));
				setField(*this, *this, 42);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec3i>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Vec3i>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Vec3i>*>(nullptr));
				setField(*this, *this, 43);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec4i>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Vec4i>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Vec4i>*>(nullptr));
				setField(*this, *this, 44);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec6i>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Vec6i>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Vec6i>*>(nullptr));
				setField(*this, *this, 45);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec8i>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Vec8i>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Vec8i>*>(nullptr));
				setField(*this, *this, 46);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec2f>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Vec2f>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Vec2f>*>(nullptr));
				setField(*this, *this, 47);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec3f>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Vec3f>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Vec3f>*>(nullptr));
				setField(*this, *this, 48);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec4f>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Vec4f>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Vec4f>*>(nullptr));
				setField(*this, *this, 49);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec6f>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Vec6f>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Vec6f>*>(nullptr));
				setField(*this, *this, 50);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec2d>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Vec2d>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Vec2d>*>(nullptr));
				setField(*this, *this, 51);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec3d>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Vec3d>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Vec3d>*>(nullptr));
				setField(*this, *this, 52);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec4d>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Vec4d>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Vec4d>*>(nullptr));
				setField(*this, *this, 53);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<cv::Vec6d>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<cv::Vec6d>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<cv::Vec6d>*>(nullptr));
				setField(*this, *this, 54);
				return;
			}

			if constexpr (std::is_same_v<Array, cv::_InputOutputArray>) {
				if (lua_is(L, index, static_cast<std::vector<std::vector<bool>>*>(nullptr))) {
					auto value = lua_to(L, index, static_cast<std::vector<std::vector<bool>>*>(nullptr));
					assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<bool>>*>(nullptr));
					setField(*this, *this, 55);
					return;
				}
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::RotatedRect>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::RotatedRect>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::RotatedRect>>*>(nullptr));
				setField(*this, *this, 56);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Range>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Range>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Range>>*>(nullptr));
				setField(*this, *this, 57);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Moments>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Moments>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Moments>>*>(nullptr));
				setField(*this, *this, 58);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<uchar>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<uchar>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<uchar>>*>(nullptr));
				setField(*this, *this, 59);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<schar>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<schar>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<schar>>*>(nullptr));
				setField(*this, *this, 60);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<char>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<char>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<char>>*>(nullptr));
				setField(*this, *this, 61);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<ushort>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<ushort>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<ushort>>*>(nullptr));
				setField(*this, *this, 62);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<short>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<short>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<short>>*>(nullptr));
				setField(*this, *this, 63);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<int>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<int>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<int>>*>(nullptr));
				setField(*this, *this, 64);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<float>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<float>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<float>>*>(nullptr));
				setField(*this, *this, 65);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<double>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<double>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<double>>*>(nullptr));
				setField(*this, *this, 66);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Point3i>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Point3i>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Point3i>>*>(nullptr));
				setField(*this, *this, 67);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Point3f>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Point3f>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Point3f>>*>(nullptr));
				setField(*this, *this, 68);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Point3d>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Point3d>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Point3d>>*>(nullptr));
				setField(*this, *this, 69);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Point2f>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Point2f>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Point2f>>*>(nullptr));
				setField(*this, *this, 70);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Point2d>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Point2d>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Point2d>>*>(nullptr));
				setField(*this, *this, 71);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Point>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Point>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Point>>*>(nullptr));
				setField(*this, *this, 72);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Rect2f>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Rect2f>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Rect2f>>*>(nullptr));
				setField(*this, *this, 73);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Rect2d>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Rect2d>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Rect2d>>*>(nullptr));
				setField(*this, *this, 74);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Rect>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Rect>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Rect>>*>(nullptr));
				setField(*this, *this, 75);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Size2f>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Size2f>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Size2f>>*>(nullptr));
				setField(*this, *this, 76);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Size2d>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Size2d>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Size2d>>*>(nullptr));
				setField(*this, *this, 77);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Size>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Size>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Size>>*>(nullptr));
				setField(*this, *this, 78);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec2b>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec2b>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Vec2b>>*>(nullptr));
				setField(*this, *this, 79);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec3b>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec3b>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Vec3b>>*>(nullptr));
				setField(*this, *this, 80);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec4b>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec4b>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Vec4b>>*>(nullptr));
				setField(*this, *this, 81);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec2s>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec2s>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Vec2s>>*>(nullptr));
				setField(*this, *this, 82);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec3s>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec3s>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Vec3s>>*>(nullptr));
				setField(*this, *this, 83);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec4s>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec4s>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Vec4s>>*>(nullptr));
				setField(*this, *this, 84);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec2w>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec2w>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Vec2w>>*>(nullptr));
				setField(*this, *this, 85);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec3w>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec3w>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Vec3w>>*>(nullptr));
				setField(*this, *this, 86);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec4w>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec4w>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Vec4w>>*>(nullptr));
				setField(*this, *this, 87);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec2i>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec2i>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Vec2i>>*>(nullptr));
				setField(*this, *this, 88);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec3i>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec3i>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Vec3i>>*>(nullptr));
				setField(*this, *this, 89);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec4i>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec4i>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Vec4i>>*>(nullptr));
				setField(*this, *this, 90);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec6i>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec6i>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Vec6i>>*>(nullptr));
				setField(*this, *this, 91);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec8i>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec8i>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Vec8i>>*>(nullptr));
				setField(*this, *this, 92);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec2f>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec2f>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Vec2f>>*>(nullptr));
				setField(*this, *this, 93);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec3f>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec3f>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Vec3f>>*>(nullptr));
				setField(*this, *this, 94);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec4f>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec4f>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Vec4f>>*>(nullptr));
				setField(*this, *this, 95);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec6f>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec6f>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Vec6f>>*>(nullptr));
				setField(*this, *this, 96);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec2d>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec2d>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Vec2d>>*>(nullptr));
				setField(*this, *this, 97);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec3d>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec3d>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Vec3d>>*>(nullptr));
				setField(*this, *this, 98);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec4d>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec4d>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Vec4d>>*>(nullptr));
				setField(*this, *this, 99);
				return;
			}

			if (lua_is(L, index, static_cast<std::vector<std::vector<cv::Vec6d>>*>(nullptr))) {
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec6d>>*>(nullptr));
				assign_maybe_shared(this->obj, value, static_cast<std::vector<std::vector<cv::Vec6d>>*>(nullptr));
				setField(*this, *this, 100);
				return;
			}
		}

		_OptionalArray(const std::shared_ptr<Array>& ptr) : ptr(ptr) {}

		_OptionalArray(const _OptionalArray& other) {
			other.setField(other, *this, other.field);
		}

		_OptionalArray& operator=(const _OptionalArray& other) {
			other.setField(other, *this, other.field);
			return *this;
		}

		_OptionalArray(_OptionalArray&& other) noexcept :
			ptr(std::move(other.ptr)),
			mval(std::move(other.mval)),
			vval(std::move(other.vval))
		{
			switch (field) {
			case 1:
				if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {
					this->obj = new std::shared_ptr<bool>(std::move(*reinterpret_cast<std::shared_ptr<bool>*>(other.obj)));
				}
				break;
			case 2:
				if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {
					this->obj = new std::shared_ptr<double>(std::move(*reinterpret_cast<std::shared_ptr<double>*>(other.obj)));
				}
				break;
			case 3:
				if constexpr (!is_arrays_type) {
					this->obj = new std::shared_ptr<cv::cuda::GpuMat>(std::move(*reinterpret_cast<std::shared_ptr<cv::cuda::GpuMat>*>(other.obj)));
				}
				break;
			case 4:
				if constexpr (!is_arrays_type) {
					this->obj = new std::shared_ptr<cv::Mat>(std::move(*reinterpret_cast<std::shared_ptr<cv::Mat>*>(other.obj)));
				}
				break;
			case 5:
				if constexpr (!is_arrays_type) {
					this->obj = new std::shared_ptr<cv::UMat>(std::move(*reinterpret_cast<std::shared_ptr<cv::UMat>*>(other.obj)));
				}
				break;
			case 6:
				if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {
					this->obj = new std::shared_ptr<cv::Scalar>(std::move(*reinterpret_cast<std::shared_ptr<cv::Scalar>*>(other.obj)));
				}
				break;
			case 7:
				this->obj = new std::shared_ptr<std::vector<cv::Mat>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Mat>>*>(other.obj)));
				break;
			case 8:
				this->obj = new std::shared_ptr<std::vector<cv::UMat>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::UMat>>*>(other.obj)));
				break;
			case 9:
				if constexpr (std::is_same_v<Array, cv::_InputArray>) {
					this->obj = new std::shared_ptr<std::vector<bool>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<bool>>*>(other.obj)));
				}
				break;
			case 10:
				this->obj = new std::shared_ptr<std::vector<cv::RotatedRect>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::RotatedRect>>*>(other.obj)));
				break;
			case 11:
				this->obj = new std::shared_ptr<std::vector<cv::Range>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Range>>*>(other.obj)));
				break;
			case 12:
				this->obj = new std::shared_ptr<std::vector<cv::Moments>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Moments>>*>(other.obj)));
				break;
			case 13:
				this->obj = new std::shared_ptr<std::vector<uchar>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<uchar>>*>(other.obj)));
				break;
			case 14:
				this->obj = new std::shared_ptr<std::vector<schar>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<schar>>*>(other.obj)));
				break;
			case 15:
				this->obj = new std::shared_ptr<std::vector<char>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<char>>*>(other.obj)));
				break;
			case 16:
				this->obj = new std::shared_ptr<std::vector<ushort>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<ushort>>*>(other.obj)));
				break;
			case 17:
				this->obj = new std::shared_ptr<std::vector<short>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<short>>*>(other.obj)));
				break;
			case 18:
				this->obj = new std::shared_ptr<std::vector<int>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<int>>*>(other.obj)));
				break;
			case 19:
				this->obj = new std::shared_ptr<std::vector<float>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<float>>*>(other.obj)));
				break;
			case 20:
				this->obj = new std::shared_ptr<std::vector<double>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<double>>*>(other.obj)));
				break;
			case 21:
				this->obj = new std::shared_ptr<std::vector<cv::Point3i>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point3i>>*>(other.obj)));
				break;
			case 22:
				this->obj = new std::shared_ptr<std::vector<cv::Point3f>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point3f>>*>(other.obj)));
				break;
			case 23:
				this->obj = new std::shared_ptr<std::vector<cv::Point3d>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point3d>>*>(other.obj)));
				break;
			case 24:
				this->obj = new std::shared_ptr<std::vector<cv::Point2f>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point2f>>*>(other.obj)));
				break;
			case 25:
				this->obj = new std::shared_ptr<std::vector<cv::Point2d>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point2d>>*>(other.obj)));
				break;
			case 26:
				this->obj = new std::shared_ptr<std::vector<cv::Point>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point>>*>(other.obj)));
				break;
			case 27:
				this->obj = new std::shared_ptr<std::vector<cv::Rect2f>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2f>>*>(other.obj)));
				break;
			case 28:
				this->obj = new std::shared_ptr<std::vector<cv::Rect2d>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2d>>*>(other.obj)));
				break;
			case 29:
				this->obj = new std::shared_ptr<std::vector<cv::Rect>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Rect>>*>(other.obj)));
				break;
			case 30:
				this->obj = new std::shared_ptr<std::vector<cv::Size2f>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Size2f>>*>(other.obj)));
				break;
			case 31:
				this->obj = new std::shared_ptr<std::vector<cv::Size2d>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Size2d>>*>(other.obj)));
				break;
			case 32:
				this->obj = new std::shared_ptr<std::vector<cv::Size>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Size>>*>(other.obj)));
				break;
			case 33:
				this->obj = new std::shared_ptr<std::vector<cv::Vec2b>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2b>>*>(other.obj)));
				break;
			case 34:
				this->obj = new std::shared_ptr<std::vector<cv::Vec3b>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3b>>*>(other.obj)));
				break;
			case 35:
				this->obj = new std::shared_ptr<std::vector<cv::Vec4b>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4b>>*>(other.obj)));
				break;
			case 36:
				this->obj = new std::shared_ptr<std::vector<cv::Vec2s>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2s>>*>(other.obj)));
				break;
			case 37:
				this->obj = new std::shared_ptr<std::vector<cv::Vec3s>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3s>>*>(other.obj)));
				break;
			case 38:
				this->obj = new std::shared_ptr<std::vector<cv::Vec4s>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4s>>*>(other.obj)));
				break;
			case 39:
				this->obj = new std::shared_ptr<std::vector<cv::Vec2w>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2w>>*>(other.obj)));
				break;
			case 40:
				this->obj = new std::shared_ptr<std::vector<cv::Vec3w>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3w>>*>(other.obj)));
				break;
			case 41:
				this->obj = new std::shared_ptr<std::vector<cv::Vec4w>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4w>>*>(other.obj)));
				break;
			case 42:
				this->obj = new std::shared_ptr<std::vector<cv::Vec2i>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2i>>*>(other.obj)));
				break;
			case 43:
				this->obj = new std::shared_ptr<std::vector<cv::Vec3i>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3i>>*>(other.obj)));
				break;
			case 44:
				this->obj = new std::shared_ptr<std::vector<cv::Vec4i>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4i>>*>(other.obj)));
				break;
			case 45:
				this->obj = new std::shared_ptr<std::vector<cv::Vec6i>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6i>>*>(other.obj)));
				break;
			case 46:
				this->obj = new std::shared_ptr<std::vector<cv::Vec8i>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec8i>>*>(other.obj)));
				break;
			case 47:
				this->obj = new std::shared_ptr<std::vector<cv::Vec2f>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2f>>*>(other.obj)));
				break;
			case 48:
				this->obj = new std::shared_ptr<std::vector<cv::Vec3f>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3f>>*>(other.obj)));
				break;
			case 49:
				this->obj = new std::shared_ptr<std::vector<cv::Vec4f>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4f>>*>(other.obj)));
				break;
			case 50:
				this->obj = new std::shared_ptr<std::vector<cv::Vec6f>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6f>>*>(other.obj)));
				break;
			case 51:
				this->obj = new std::shared_ptr<std::vector<cv::Vec2d>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2d>>*>(other.obj)));
				break;
			case 52:
				this->obj = new std::shared_ptr<std::vector<cv::Vec3d>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3d>>*>(other.obj)));
				break;
			case 53:
				this->obj = new std::shared_ptr<std::vector<cv::Vec4d>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4d>>*>(other.obj)));
				break;
			case 54:
				this->obj = new std::shared_ptr<std::vector<cv::Vec6d>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6d>>*>(other.obj)));
				break;
			case 55:
				if constexpr (std::is_same_v<Array, cv::_InputOutputArray>) {
					this->obj = new std::shared_ptr<std::vector<std::vector<bool>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<bool>>>*>(other.obj)));
				}
				break;
			case 56:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::RotatedRect>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::RotatedRect>>>*>(other.obj)));
				break;
			case 57:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Range>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Range>>>*>(other.obj)));
				break;
			case 58:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Moments>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Moments>>>*>(other.obj)));
				break;
			case 59:
				this->obj = new std::shared_ptr<std::vector<std::vector<uchar>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<uchar>>>*>(other.obj)));
				break;
			case 60:
				this->obj = new std::shared_ptr<std::vector<std::vector<schar>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<schar>>>*>(other.obj)));
				break;
			case 61:
				this->obj = new std::shared_ptr<std::vector<std::vector<char>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<char>>>*>(other.obj)));
				break;
			case 62:
				this->obj = new std::shared_ptr<std::vector<std::vector<ushort>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<ushort>>>*>(other.obj)));
				break;
			case 63:
				this->obj = new std::shared_ptr<std::vector<std::vector<short>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<short>>>*>(other.obj)));
				break;
			case 64:
				this->obj = new std::shared_ptr<std::vector<std::vector<int>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<int>>>*>(other.obj)));
				break;
			case 65:
				this->obj = new std::shared_ptr<std::vector<std::vector<float>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<float>>>*>(other.obj)));
				break;
			case 66:
				this->obj = new std::shared_ptr<std::vector<std::vector<double>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<double>>>*>(other.obj)));
				break;
			case 67:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Point3i>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3i>>>*>(other.obj)));
				break;
			case 68:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Point3f>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3f>>>*>(other.obj)));
				break;
			case 69:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Point3d>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3d>>>*>(other.obj)));
				break;
			case 70:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Point2f>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2f>>>*>(other.obj)));
				break;
			case 71:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Point2d>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2d>>>*>(other.obj)));
				break;
			case 72:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Point>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point>>>*>(other.obj)));
				break;
			case 73:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Rect2f>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2f>>>*>(other.obj)));
				break;
			case 74:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Rect2d>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2d>>>*>(other.obj)));
				break;
			case 75:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Rect>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect>>>*>(other.obj)));
				break;
			case 76:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Size2f>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2f>>>*>(other.obj)));
				break;
			case 77:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Size2d>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2d>>>*>(other.obj)));
				break;
			case 78:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Size>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size>>>*>(other.obj)));
				break;
			case 79:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2b>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2b>>>*>(other.obj)));
				break;
			case 80:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3b>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3b>>>*>(other.obj)));
				break;
			case 81:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4b>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4b>>>*>(other.obj)));
				break;
			case 82:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2s>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2s>>>*>(other.obj)));
				break;
			case 83:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3s>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3s>>>*>(other.obj)));
				break;
			case 84:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4s>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4s>>>*>(other.obj)));
				break;
			case 85:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2w>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2w>>>*>(other.obj)));
				break;
			case 86:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3w>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3w>>>*>(other.obj)));
				break;
			case 87:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4w>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4w>>>*>(other.obj)));
				break;
			case 88:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2i>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2i>>>*>(other.obj)));
				break;
			case 89:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3i>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3i>>>*>(other.obj)));
				break;
			case 90:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4i>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4i>>>*>(other.obj)));
				break;
			case 91:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec6i>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6i>>>*>(other.obj)));
				break;
			case 92:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec8i>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec8i>>>*>(other.obj)));
				break;
			case 93:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2f>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2f>>>*>(other.obj)));
				break;
			case 94:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3f>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3f>>>*>(other.obj)));
				break;
			case 95:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4f>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4f>>>*>(other.obj)));
				break;
			case 96:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec6f>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6f>>>*>(other.obj)));
				break;
			case 97:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2d>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2d>>>*>(other.obj)));
				break;
			case 98:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3d>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3d>>>*>(other.obj)));
				break;
			case 99:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4d>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4d>>>*>(other.obj)));
				break;
			case 100:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec6d>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6d>>>*>(other.obj)));
				break;
			default:
				// Nothind do do
				break;
			}
		}

		~_OptionalArray() {
			switch (field) {
			case 1:
				if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {
					delete reinterpret_cast<std::shared_ptr<bool>*>(obj);
					obj = nullptr;
				}
				break;
			case 2:
				if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {
					delete reinterpret_cast<std::shared_ptr<double>*>(obj);
					obj = nullptr;
				}
				break;
			case 3:
				if constexpr (!is_arrays_type) {
					delete reinterpret_cast<std::shared_ptr<cv::cuda::GpuMat>*>(obj);
					obj = nullptr;
				}
				break;
			case 4:
				if constexpr (!is_arrays_type) {
					delete reinterpret_cast<std::shared_ptr<cv::Mat>*>(obj);
					obj = nullptr;
				}
				break;
			case 5:
				if constexpr (!is_arrays_type) {
					delete reinterpret_cast<std::shared_ptr<cv::UMat>*>(obj);
					obj = nullptr;
				}
				break;
			case 6:
				if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {
					delete reinterpret_cast<std::shared_ptr<cv::Scalar>*>(obj);
					obj = nullptr;
				}
				break;
			case 7:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Mat>>*>(obj);
				obj = nullptr;
				break;
			case 8:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::UMat>>*>(obj);
				obj = nullptr;
				break;
			case 9:
				if constexpr (std::is_same_v<Array, cv::_InputArray>) {
					delete reinterpret_cast<std::shared_ptr<std::vector<bool>>*>(obj);
					obj = nullptr;
				}
				break;
			case 10:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::RotatedRect>>*>(obj);
				obj = nullptr;
				break;
			case 11:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Range>>*>(obj);
				obj = nullptr;
				break;
			case 12:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Moments>>*>(obj);
				obj = nullptr;
				break;
			case 13:
				delete reinterpret_cast<std::shared_ptr<std::vector<uchar>>*>(obj);
				obj = nullptr;
				break;
			case 14:
				delete reinterpret_cast<std::shared_ptr<std::vector<schar>>*>(obj);
				obj = nullptr;
				break;
			case 15:
				delete reinterpret_cast<std::shared_ptr<std::vector<char>>*>(obj);
				obj = nullptr;
				break;
			case 16:
				delete reinterpret_cast<std::shared_ptr<std::vector<ushort>>*>(obj);
				obj = nullptr;
				break;
			case 17:
				delete reinterpret_cast<std::shared_ptr<std::vector<short>>*>(obj);
				obj = nullptr;
				break;
			case 18:
				delete reinterpret_cast<std::shared_ptr<std::vector<int>>*>(obj);
				obj = nullptr;
				break;
			case 19:
				delete reinterpret_cast<std::shared_ptr<std::vector<float>>*>(obj);
				obj = nullptr;
				break;
			case 20:
				delete reinterpret_cast<std::shared_ptr<std::vector<double>>*>(obj);
				obj = nullptr;
				break;
			case 21:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Point3i>>*>(obj);
				obj = nullptr;
				break;
			case 22:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Point3f>>*>(obj);
				obj = nullptr;
				break;
			case 23:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Point3d>>*>(obj);
				obj = nullptr;
				break;
			case 24:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Point2f>>*>(obj);
				obj = nullptr;
				break;
			case 25:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Point2d>>*>(obj);
				obj = nullptr;
				break;
			case 26:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Point>>*>(obj);
				obj = nullptr;
				break;
			case 27:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2f>>*>(obj);
				obj = nullptr;
				break;
			case 28:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2d>>*>(obj);
				obj = nullptr;
				break;
			case 29:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Rect>>*>(obj);
				obj = nullptr;
				break;
			case 30:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Size2f>>*>(obj);
				obj = nullptr;
				break;
			case 31:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Size2d>>*>(obj);
				obj = nullptr;
				break;
			case 32:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Size>>*>(obj);
				obj = nullptr;
				break;
			case 33:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2b>>*>(obj);
				obj = nullptr;
				break;
			case 34:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3b>>*>(obj);
				obj = nullptr;
				break;
			case 35:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4b>>*>(obj);
				obj = nullptr;
				break;
			case 36:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2s>>*>(obj);
				obj = nullptr;
				break;
			case 37:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3s>>*>(obj);
				obj = nullptr;
				break;
			case 38:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4s>>*>(obj);
				obj = nullptr;
				break;
			case 39:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2w>>*>(obj);
				obj = nullptr;
				break;
			case 40:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3w>>*>(obj);
				obj = nullptr;
				break;
			case 41:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4w>>*>(obj);
				obj = nullptr;
				break;
			case 42:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2i>>*>(obj);
				obj = nullptr;
				break;
			case 43:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3i>>*>(obj);
				obj = nullptr;
				break;
			case 44:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4i>>*>(obj);
				obj = nullptr;
				break;
			case 45:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6i>>*>(obj);
				obj = nullptr;
				break;
			case 46:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec8i>>*>(obj);
				obj = nullptr;
				break;
			case 47:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2f>>*>(obj);
				obj = nullptr;
				break;
			case 48:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3f>>*>(obj);
				obj = nullptr;
				break;
			case 49:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4f>>*>(obj);
				obj = nullptr;
				break;
			case 50:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6f>>*>(obj);
				obj = nullptr;
				break;
			case 51:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2d>>*>(obj);
				obj = nullptr;
				break;
			case 52:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3d>>*>(obj);
				obj = nullptr;
				break;
			case 53:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4d>>*>(obj);
				obj = nullptr;
				break;
			case 54:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6d>>*>(obj);
				obj = nullptr;
				break;
			case 55:
				if constexpr (std::is_same_v<Array, cv::_InputOutputArray>) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<bool>>>*>(obj);
					obj = nullptr;
				}
				break;
			case 56:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::RotatedRect>>>*>(obj);
				obj = nullptr;
				break;
			case 57:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Range>>>*>(obj);
				obj = nullptr;
				break;
			case 58:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Moments>>>*>(obj);
				obj = nullptr;
				break;
			case 59:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<uchar>>>*>(obj);
				obj = nullptr;
				break;
			case 60:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<schar>>>*>(obj);
				obj = nullptr;
				break;
			case 61:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<char>>>*>(obj);
				obj = nullptr;
				break;
			case 62:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<ushort>>>*>(obj);
				obj = nullptr;
				break;
			case 63:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<short>>>*>(obj);
				obj = nullptr;
				break;
			case 64:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<int>>>*>(obj);
				obj = nullptr;
				break;
			case 65:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<float>>>*>(obj);
				obj = nullptr;
				break;
			case 66:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<double>>>*>(obj);
				obj = nullptr;
				break;
			case 67:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3i>>>*>(obj);
				obj = nullptr;
				break;
			case 68:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3f>>>*>(obj);
				obj = nullptr;
				break;
			case 69:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3d>>>*>(obj);
				obj = nullptr;
				break;
			case 70:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2f>>>*>(obj);
				obj = nullptr;
				break;
			case 71:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2d>>>*>(obj);
				obj = nullptr;
				break;
			case 72:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point>>>*>(obj);
				obj = nullptr;
				break;
			case 73:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2f>>>*>(obj);
				obj = nullptr;
				break;
			case 74:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2d>>>*>(obj);
				obj = nullptr;
				break;
			case 75:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect>>>*>(obj);
				obj = nullptr;
				break;
			case 76:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2f>>>*>(obj);
				obj = nullptr;
				break;
			case 77:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2d>>>*>(obj);
				obj = nullptr;
				break;
			case 78:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size>>>*>(obj);
				obj = nullptr;
				break;
			case 79:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2b>>>*>(obj);
				obj = nullptr;
				break;
			case 80:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3b>>>*>(obj);
				obj = nullptr;
				break;
			case 81:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4b>>>*>(obj);
				obj = nullptr;
				break;
			case 82:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2s>>>*>(obj);
				obj = nullptr;
				break;
			case 83:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3s>>>*>(obj);
				obj = nullptr;
				break;
			case 84:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4s>>>*>(obj);
				obj = nullptr;
				break;
			case 85:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2w>>>*>(obj);
				obj = nullptr;
				break;
			case 86:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3w>>>*>(obj);
				obj = nullptr;
				break;
			case 87:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4w>>>*>(obj);
				obj = nullptr;
				break;
			case 88:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2i>>>*>(obj);
				obj = nullptr;
				break;
			case 89:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3i>>>*>(obj);
				obj = nullptr;
				break;
			case 90:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4i>>>*>(obj);
				obj = nullptr;
				break;
			case 91:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6i>>>*>(obj);
				obj = nullptr;
				break;
			case 92:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec8i>>>*>(obj);
				obj = nullptr;
				break;
			case 93:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2f>>>*>(obj);
				obj = nullptr;
				break;
			case 94:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3f>>>*>(obj);
				obj = nullptr;
				break;
			case 95:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4f>>>*>(obj);
				obj = nullptr;
				break;
			case 96:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6f>>>*>(obj);
				obj = nullptr;
				break;
			case 97:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2d>>>*>(obj);
				obj = nullptr;
				break;
			case 98:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3d>>>*>(obj);
				obj = nullptr;
				break;
			case 99:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4d>>>*>(obj);
				obj = nullptr;
				break;
			case 100:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6d>>>*>(obj);
				obj = nullptr;
				break;
			default:
				// Nothind do do
				break;
			}
		}

		template<typename T>
		inline void reset(T& obj) {
			ptr = std::make_shared<Array>(obj);
		}

		template<typename T>
		inline void reset(const T& obj) {
			ptr = std::make_shared<Array>(obj);
		}

		static void setField(const _OptionalArray& src, _OptionalArray& dst, std::uint8_t _field) {
			dst.field = _field;

			switch (_field) {
			case 1:
				if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {

					if (&src != &dst) {
						if (dst.obj) {
							*reinterpret_cast<std::shared_ptr<bool>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<bool>*>(src.obj);
						}
						else {
							dst.obj = new std::shared_ptr<bool>(*reinterpret_cast<std::shared_ptr<bool>*>(src.obj));
						}
					}
					if (dst.obj) {
						dst.reset(**reinterpret_cast<std::shared_ptr<bool>*>(dst.obj));
					}

				}
				break;
			case 2:
				if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {

					if (&src != &dst) {
						if (dst.obj) {
							*reinterpret_cast<std::shared_ptr<double>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<double>*>(src.obj);
						}
						else {
							dst.obj = new std::shared_ptr<double>(*reinterpret_cast<std::shared_ptr<double>*>(src.obj));
						}
					}
					if (dst.obj) {
						dst.reset(**reinterpret_cast<std::shared_ptr<double>*>(dst.obj));
					}

				}
				break;
			case 3:
				if constexpr (!is_arrays_type) {

					if (&src != &dst) {
						if (dst.obj) {
							*reinterpret_cast<std::shared_ptr<cv::cuda::GpuMat>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<cv::cuda::GpuMat>*>(src.obj);
						}
						else {
							dst.obj = new std::shared_ptr<cv::cuda::GpuMat>(*reinterpret_cast<std::shared_ptr<cv::cuda::GpuMat>*>(src.obj));
						}
					}
					if (dst.obj) {
						dst.reset(**reinterpret_cast<std::shared_ptr<cv::cuda::GpuMat>*>(dst.obj));
					}

				}
				break;
			case 4:
				if constexpr (!is_arrays_type) {

					if (&src != &dst) {
						if (dst.obj) {
							*reinterpret_cast<std::shared_ptr<cv::Mat>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<cv::Mat>*>(src.obj);
						}
						else {
							dst.obj = new std::shared_ptr<cv::Mat>(*reinterpret_cast<std::shared_ptr<cv::Mat>*>(src.obj));
						}
					}
					if (dst.obj) {
						dst.reset(**reinterpret_cast<std::shared_ptr<cv::Mat>*>(dst.obj));
					}

				}
				break;
			case 5:
				if constexpr (!is_arrays_type) {

					if (&src != &dst) {
						if (dst.obj) {
							*reinterpret_cast<std::shared_ptr<cv::UMat>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<cv::UMat>*>(src.obj);
						}
						else {
							dst.obj = new std::shared_ptr<cv::UMat>(*reinterpret_cast<std::shared_ptr<cv::UMat>*>(src.obj));
						}
					}
					if (dst.obj) {
						dst.reset(**reinterpret_cast<std::shared_ptr<cv::UMat>*>(dst.obj));
					}

				}
				break;
			case 6:
				if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {

					if (&src != &dst) {
						if (dst.obj) {
							*reinterpret_cast<std::shared_ptr<cv::Scalar>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<cv::Scalar>*>(src.obj);
						}
						else {
							dst.obj = new std::shared_ptr<cv::Scalar>(*reinterpret_cast<std::shared_ptr<cv::Scalar>*>(src.obj));
						}
					}
					if (dst.obj) {
						dst.reset(**reinterpret_cast<std::shared_ptr<cv::Scalar>*>(dst.obj));
					}

				}
				break;
			case 7:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Mat>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Mat>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Mat>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Mat>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Mat>>*>(dst.obj));
				}

				break;
			case 8:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::UMat>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::UMat>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::UMat>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::UMat>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::UMat>>*>(dst.obj));
				}

				break;
			case 9:
				if constexpr (std::is_same_v<Array, cv::_InputArray>) {

					if (&src != &dst) {
						if (dst.obj) {
							*reinterpret_cast<std::shared_ptr<std::vector<bool>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<bool>>*>(src.obj);
						}
						else {
							dst.obj = new std::shared_ptr<std::vector<bool>>(*reinterpret_cast<std::shared_ptr<std::vector<bool>>*>(src.obj));
						}
					}
					if (dst.obj) {
						dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<bool>>*>(dst.obj));
					}

				}
				break;
			case 10:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::RotatedRect>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::RotatedRect>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::RotatedRect>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::RotatedRect>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::RotatedRect>>*>(dst.obj));
				}

				break;
			case 11:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Range>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Range>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Range>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Range>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Range>>*>(dst.obj));
				}

				break;
			case 12:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Moments>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Moments>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Moments>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Moments>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Moments>>*>(dst.obj));
				}

				break;
			case 13:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<uchar>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<uchar>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<uchar>>(*reinterpret_cast<std::shared_ptr<std::vector<uchar>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<uchar>>*>(dst.obj));
				}

				break;
			case 14:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<schar>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<schar>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<schar>>(*reinterpret_cast<std::shared_ptr<std::vector<schar>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<schar>>*>(dst.obj));
				}

				break;
			case 15:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<char>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<char>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<char>>(*reinterpret_cast<std::shared_ptr<std::vector<char>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<char>>*>(dst.obj));
				}

				break;
			case 16:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<ushort>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<ushort>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<ushort>>(*reinterpret_cast<std::shared_ptr<std::vector<ushort>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<ushort>>*>(dst.obj));
				}

				break;
			case 17:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<short>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<short>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<short>>(*reinterpret_cast<std::shared_ptr<std::vector<short>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<short>>*>(dst.obj));
				}

				break;
			case 18:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<int>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<int>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<int>>(*reinterpret_cast<std::shared_ptr<std::vector<int>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<int>>*>(dst.obj));
				}

				break;
			case 19:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<float>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<float>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<float>>(*reinterpret_cast<std::shared_ptr<std::vector<float>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<float>>*>(dst.obj));
				}

				break;
			case 20:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<double>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<double>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<double>>(*reinterpret_cast<std::shared_ptr<std::vector<double>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<double>>*>(dst.obj));
				}

				break;
			case 21:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Point3i>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Point3i>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Point3i>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point3i>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Point3i>>*>(dst.obj));
				}

				break;
			case 22:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Point3f>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Point3f>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Point3f>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point3f>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Point3f>>*>(dst.obj));
				}

				break;
			case 23:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Point3d>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Point3d>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Point3d>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point3d>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Point3d>>*>(dst.obj));
				}

				break;
			case 24:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Point2f>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Point2f>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Point2f>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point2f>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Point2f>>*>(dst.obj));
				}

				break;
			case 25:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Point2d>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Point2d>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Point2d>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point2d>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Point2d>>*>(dst.obj));
				}

				break;
			case 26:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Point>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Point>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Point>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Point>>*>(dst.obj));
				}

				break;
			case 27:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2f>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2f>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Rect2f>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2f>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2f>>*>(dst.obj));
				}

				break;
			case 28:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2d>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2d>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Rect2d>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2d>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2d>>*>(dst.obj));
				}

				break;
			case 29:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Rect>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Rect>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Rect>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Rect>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Rect>>*>(dst.obj));
				}

				break;
			case 30:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Size2f>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Size2f>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Size2f>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Size2f>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Size2f>>*>(dst.obj));
				}

				break;
			case 31:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Size2d>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Size2d>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Size2d>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Size2d>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Size2d>>*>(dst.obj));
				}

				break;
			case 32:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Size>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Size>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Size>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Size>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Size>>*>(dst.obj));
				}

				break;
			case 33:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2b>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2b>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec2b>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2b>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2b>>*>(dst.obj));
				}

				break;
			case 34:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3b>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3b>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec3b>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3b>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3b>>*>(dst.obj));
				}

				break;
			case 35:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4b>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4b>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec4b>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4b>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4b>>*>(dst.obj));
				}

				break;
			case 36:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2s>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2s>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec2s>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2s>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2s>>*>(dst.obj));
				}

				break;
			case 37:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3s>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3s>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec3s>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3s>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3s>>*>(dst.obj));
				}

				break;
			case 38:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4s>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4s>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec4s>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4s>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4s>>*>(dst.obj));
				}

				break;
			case 39:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2w>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2w>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec2w>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2w>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2w>>*>(dst.obj));
				}

				break;
			case 40:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3w>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3w>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec3w>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3w>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3w>>*>(dst.obj));
				}

				break;
			case 41:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4w>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4w>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec4w>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4w>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4w>>*>(dst.obj));
				}

				break;
			case 42:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2i>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2i>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec2i>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2i>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2i>>*>(dst.obj));
				}

				break;
			case 43:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3i>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3i>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec3i>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3i>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3i>>*>(dst.obj));
				}

				break;
			case 44:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4i>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4i>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec4i>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4i>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4i>>*>(dst.obj));
				}

				break;
			case 45:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6i>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6i>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec6i>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6i>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6i>>*>(dst.obj));
				}

				break;
			case 46:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec8i>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec8i>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec8i>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec8i>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec8i>>*>(dst.obj));
				}

				break;
			case 47:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2f>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2f>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec2f>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2f>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2f>>*>(dst.obj));
				}

				break;
			case 48:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3f>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3f>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec3f>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3f>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3f>>*>(dst.obj));
				}

				break;
			case 49:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4f>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4f>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec4f>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4f>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4f>>*>(dst.obj));
				}

				break;
			case 50:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6f>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6f>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec6f>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6f>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6f>>*>(dst.obj));
				}

				break;
			case 51:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2d>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2d>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec2d>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2d>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2d>>*>(dst.obj));
				}

				break;
			case 52:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3d>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3d>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec3d>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3d>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3d>>*>(dst.obj));
				}

				break;
			case 53:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4d>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4d>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec4d>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4d>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4d>>*>(dst.obj));
				}

				break;
			case 54:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6d>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6d>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec6d>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6d>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6d>>*>(dst.obj));
				}

				break;
			case 55:
				if constexpr (std::is_same_v<Array, cv::_InputOutputArray>) {

					if (&src != &dst) {
						if (dst.obj) {
							*reinterpret_cast<std::shared_ptr<std::vector<std::vector<bool>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<bool>>>*>(src.obj);
						}
						else {
							dst.obj = new std::shared_ptr<std::vector<std::vector<bool>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<bool>>>*>(src.obj));
						}
					}
					if (dst.obj) {
						dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<bool>>>*>(dst.obj));
					}

				}
				break;
			case 56:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::RotatedRect>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::RotatedRect>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::RotatedRect>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::RotatedRect>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::RotatedRect>>>*>(dst.obj));
				}

				break;
			case 57:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Range>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Range>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Range>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Range>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Range>>>*>(dst.obj));
				}

				break;
			case 58:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Moments>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Moments>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Moments>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Moments>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Moments>>>*>(dst.obj));
				}

				break;
			case 59:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<uchar>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<uchar>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<uchar>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<uchar>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<uchar>>>*>(dst.obj));
				}

				break;
			case 60:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<schar>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<schar>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<schar>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<schar>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<schar>>>*>(dst.obj));
				}

				break;
			case 61:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<char>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<char>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<char>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<char>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<char>>>*>(dst.obj));
				}

				break;
			case 62:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<ushort>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<ushort>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<ushort>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<ushort>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<ushort>>>*>(dst.obj));
				}

				break;
			case 63:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<short>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<short>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<short>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<short>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<short>>>*>(dst.obj));
				}

				break;
			case 64:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<int>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<int>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<int>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<int>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<int>>>*>(dst.obj));
				}

				break;
			case 65:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<float>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<float>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<float>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<float>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<float>>>*>(dst.obj));
				}

				break;
			case 66:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<double>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<double>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<double>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<double>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<double>>>*>(dst.obj));
				}

				break;
			case 67:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3i>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3i>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Point3i>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3i>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3i>>>*>(dst.obj));
				}

				break;
			case 68:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3f>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3f>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Point3f>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3f>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3f>>>*>(dst.obj));
				}

				break;
			case 69:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3d>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3d>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Point3d>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3d>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3d>>>*>(dst.obj));
				}

				break;
			case 70:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2f>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2f>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Point2f>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2f>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2f>>>*>(dst.obj));
				}

				break;
			case 71:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2d>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2d>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Point2d>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2d>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2d>>>*>(dst.obj));
				}

				break;
			case 72:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Point>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point>>>*>(dst.obj));
				}

				break;
			case 73:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2f>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2f>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Rect2f>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2f>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2f>>>*>(dst.obj));
				}

				break;
			case 74:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2d>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2d>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Rect2d>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2d>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2d>>>*>(dst.obj));
				}

				break;
			case 75:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Rect>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect>>>*>(dst.obj));
				}

				break;
			case 76:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2f>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2f>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Size2f>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2f>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2f>>>*>(dst.obj));
				}

				break;
			case 77:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2d>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2d>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Size2d>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2d>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2d>>>*>(dst.obj));
				}

				break;
			case 78:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Size>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size>>>*>(dst.obj));
				}

				break;
			case 79:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2b>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2b>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2b>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2b>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2b>>>*>(dst.obj));
				}

				break;
			case 80:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3b>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3b>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3b>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3b>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3b>>>*>(dst.obj));
				}

				break;
			case 81:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4b>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4b>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4b>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4b>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4b>>>*>(dst.obj));
				}

				break;
			case 82:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2s>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2s>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2s>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2s>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2s>>>*>(dst.obj));
				}

				break;
			case 83:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3s>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3s>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3s>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3s>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3s>>>*>(dst.obj));
				}

				break;
			case 84:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4s>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4s>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4s>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4s>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4s>>>*>(dst.obj));
				}

				break;
			case 85:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2w>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2w>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2w>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2w>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2w>>>*>(dst.obj));
				}

				break;
			case 86:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3w>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3w>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3w>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3w>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3w>>>*>(dst.obj));
				}

				break;
			case 87:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4w>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4w>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4w>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4w>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4w>>>*>(dst.obj));
				}

				break;
			case 88:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2i>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2i>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2i>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2i>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2i>>>*>(dst.obj));
				}

				break;
			case 89:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3i>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3i>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3i>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3i>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3i>>>*>(dst.obj));
				}

				break;
			case 90:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4i>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4i>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4i>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4i>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4i>>>*>(dst.obj));
				}

				break;
			case 91:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6i>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6i>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec6i>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6i>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6i>>>*>(dst.obj));
				}

				break;
			case 92:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec8i>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec8i>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec8i>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec8i>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec8i>>>*>(dst.obj));
				}

				break;
			case 93:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2f>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2f>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2f>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2f>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2f>>>*>(dst.obj));
				}

				break;
			case 94:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3f>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3f>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3f>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3f>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3f>>>*>(dst.obj));
				}

				break;
			case 95:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4f>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4f>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4f>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4f>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4f>>>*>(dst.obj));
				}

				break;
			case 96:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6f>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6f>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec6f>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6f>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6f>>>*>(dst.obj));
				}

				break;
			case 97:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2d>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2d>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2d>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2d>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2d>>>*>(dst.obj));
				}

				break;
			case 98:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3d>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3d>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3d>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3d>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3d>>>*>(dst.obj));
				}

				break;
			case 99:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4d>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4d>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4d>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4d>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4d>>>*>(dst.obj));
				}

				break;
			case 100:

				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6d>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6d>>>*>(src.obj);
					}
					else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec6d>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6d>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6d>>>*>(dst.obj));
				}

				break;
			case 101:
				dst.reset(dst.vval);
				break;
			case 102:
				dst.reset(dst.mval);
				break;
			default:
				if (&src != &dst) {
					dst.ptr = src.ptr;
				}
			}
		}

		auto get() {
			return ptr;
		}

		operator bool() const {
			return static_cast<bool>(ptr);
		}

		auto& operator*() {
			return *ptr;
		}

		std::uint8_t field = 0;
		std::shared_ptr<Array> ptr;
		cv::Mat mval;
		std::vector<cv::Mat> vval;
		void* obj = nullptr;
	};

	template<typename Array, bool is_arrays_type>
	inline int lua_push(lua_State* L, const _OptionalArray<Array, is_arrays_type>& dst) {
		switch (dst.field) {
		case 1:
			if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {
				return lua_push(L, *reinterpret_cast<std::shared_ptr<bool>*>(dst.obj));
			}
			break;
		case 2:
			if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {
				return lua_push(L, *reinterpret_cast<std::shared_ptr<double>*>(dst.obj));
			}
			break;
		case 3:
			if constexpr (!is_arrays_type) {
				return lua_push(L, *reinterpret_cast<std::shared_ptr<cv::cuda::GpuMat>*>(dst.obj));
			}
			break;
		case 4:
			if constexpr (!is_arrays_type) {
				return lua_push(L, *reinterpret_cast<std::shared_ptr<cv::Mat>*>(dst.obj));
			}
			break;
		case 5:
			if constexpr (!is_arrays_type) {
				return lua_push(L, *reinterpret_cast<std::shared_ptr<cv::UMat>*>(dst.obj));
			}
			break;
		case 6:
			if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {
				return lua_push(L, *reinterpret_cast<std::shared_ptr<cv::Scalar>*>(dst.obj));
			}
			break;
		case 7:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Mat>>*>(dst.obj));
			break;
		case 8:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::UMat>>*>(dst.obj));
			break;
		case 9:
			if constexpr (std::is_same_v<Array, cv::_InputArray>) {
				return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<bool>>*>(dst.obj));
			}
			break;
		case 10:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::RotatedRect>>*>(dst.obj));
			break;
		case 11:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Range>>*>(dst.obj));
			break;
		case 12:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Moments>>*>(dst.obj));
			break;
		case 13:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<uchar>>*>(dst.obj));
			break;
		case 14:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<schar>>*>(dst.obj));
			break;
		case 15:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<char>>*>(dst.obj));
			break;
		case 16:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<ushort>>*>(dst.obj));
			break;
		case 17:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<short>>*>(dst.obj));
			break;
		case 18:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<int>>*>(dst.obj));
			break;
		case 19:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<float>>*>(dst.obj));
			break;
		case 20:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<double>>*>(dst.obj));
			break;
		case 21:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Point3i>>*>(dst.obj));
			break;
		case 22:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Point3f>>*>(dst.obj));
			break;
		case 23:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Point3d>>*>(dst.obj));
			break;
		case 24:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Point2f>>*>(dst.obj));
			break;
		case 25:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Point2d>>*>(dst.obj));
			break;
		case 26:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Point>>*>(dst.obj));
			break;
		case 27:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2f>>*>(dst.obj));
			break;
		case 28:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2d>>*>(dst.obj));
			break;
		case 29:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Rect>>*>(dst.obj));
			break;
		case 30:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Size2f>>*>(dst.obj));
			break;
		case 31:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Size2d>>*>(dst.obj));
			break;
		case 32:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Size>>*>(dst.obj));
			break;
		case 33:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2b>>*>(dst.obj));
			break;
		case 34:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3b>>*>(dst.obj));
			break;
		case 35:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4b>>*>(dst.obj));
			break;
		case 36:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2s>>*>(dst.obj));
			break;
		case 37:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3s>>*>(dst.obj));
			break;
		case 38:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4s>>*>(dst.obj));
			break;
		case 39:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2w>>*>(dst.obj));
			break;
		case 40:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3w>>*>(dst.obj));
			break;
		case 41:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4w>>*>(dst.obj));
			break;
		case 42:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2i>>*>(dst.obj));
			break;
		case 43:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3i>>*>(dst.obj));
			break;
		case 44:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4i>>*>(dst.obj));
			break;
		case 45:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6i>>*>(dst.obj));
			break;
		case 46:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec8i>>*>(dst.obj));
			break;
		case 47:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2f>>*>(dst.obj));
			break;
		case 48:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3f>>*>(dst.obj));
			break;
		case 49:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4f>>*>(dst.obj));
			break;
		case 50:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6f>>*>(dst.obj));
			break;
		case 51:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2d>>*>(dst.obj));
			break;
		case 52:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3d>>*>(dst.obj));
			break;
		case 53:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4d>>*>(dst.obj));
			break;
		case 54:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6d>>*>(dst.obj));
			break;
		case 55:
			if constexpr (std::is_same_v<Array, cv::_InputOutputArray>) {
				return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<bool>>>*>(dst.obj));
			}
			break;
		case 56:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::RotatedRect>>>*>(dst.obj));
			break;
		case 57:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Range>>>*>(dst.obj));
			break;
		case 58:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Moments>>>*>(dst.obj));
			break;
		case 59:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<uchar>>>*>(dst.obj));
			break;
		case 60:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<schar>>>*>(dst.obj));
			break;
		case 61:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<char>>>*>(dst.obj));
			break;
		case 62:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<ushort>>>*>(dst.obj));
			break;
		case 63:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<short>>>*>(dst.obj));
			break;
		case 64:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<int>>>*>(dst.obj));
			break;
		case 65:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<float>>>*>(dst.obj));
			break;
		case 66:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<double>>>*>(dst.obj));
			break;
		case 67:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3i>>>*>(dst.obj));
			break;
		case 68:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3f>>>*>(dst.obj));
			break;
		case 69:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3d>>>*>(dst.obj));
			break;
		case 70:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2f>>>*>(dst.obj));
			break;
		case 71:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2d>>>*>(dst.obj));
			break;
		case 72:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point>>>*>(dst.obj));
			break;
		case 73:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2f>>>*>(dst.obj));
			break;
		case 74:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2d>>>*>(dst.obj));
			break;
		case 75:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect>>>*>(dst.obj));
			break;
		case 76:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2f>>>*>(dst.obj));
			break;
		case 77:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2d>>>*>(dst.obj));
			break;
		case 78:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size>>>*>(dst.obj));
			break;
		case 79:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2b>>>*>(dst.obj));
			break;
		case 80:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3b>>>*>(dst.obj));
			break;
		case 81:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4b>>>*>(dst.obj));
			break;
		case 82:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2s>>>*>(dst.obj));
			break;
		case 83:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3s>>>*>(dst.obj));
			break;
		case 84:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4s>>>*>(dst.obj));
			break;
		case 85:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2w>>>*>(dst.obj));
			break;
		case 86:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3w>>>*>(dst.obj));
			break;
		case 87:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4w>>>*>(dst.obj));
			break;
		case 88:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2i>>>*>(dst.obj));
			break;
		case 89:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3i>>>*>(dst.obj));
			break;
		case 90:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4i>>>*>(dst.obj));
			break;
		case 91:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6i>>>*>(dst.obj));
			break;
		case 92:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec8i>>>*>(dst.obj));
			break;
		case 93:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2f>>>*>(dst.obj));
			break;
		case 94:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3f>>>*>(dst.obj));
			break;
		case 95:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4f>>>*>(dst.obj));
			break;
		case 96:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6f>>>*>(dst.obj));
			break;
		case 97:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2d>>>*>(dst.obj));
			break;
		case 98:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3d>>>*>(dst.obj));
			break;
		case 99:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4d>>>*>(dst.obj));
			break;
		case 100:
			return lua_push(L, *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6d>>>*>(dst.obj));
			break;
		case 101:
			return lua_push(L, dst.vval);
		case 102:
			return lua_push(L, dst.mval);
		default:
			// Nothind do do
			break;
		}

		lua_pushnil(L);
		return 1;
	}

	// InputArray, outputArray, InputOutputArray
	template<typename Array>
	struct OptionalArray : public _OptionalArray<Array, false> {
		using _OptionalArray<Array, false>::_OptionalArray;
	};

	// InputArrayOfArrays, outputArrayOfArrays, InputOutputArrayOfArrays
	template<typename Array>
	struct OptionalArrays : public _OptionalArray<Array, true> {
		using _OptionalArray<Array, true>::_OptionalArray;
	};

	template<typename Array>
	inline bool lua_isarrays(lua_State* L, int index, Array*) {
		return OptionalArrays<Array>::is_valid(L, index);
	}

	template<typename Array>
	inline decltype(auto) lua_toarrays(lua_State* L, int index, Array*) {
		return OptionalArrays<Array>(L, index);
	}

	template<typename Array>
	inline bool lua_isarray(lua_State* L, int index, Array*) {
		return OptionalArray<Array>::is_valid(L, index);
	}

	template<typename Array>
	inline decltype(auto) lua_toarray(lua_State* L, int index, Array*) {
		return OptionalArray<Array>(L, index);
	}
}