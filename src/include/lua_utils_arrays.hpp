#pragma once

#include <lua_utils.hpp>
#include <lua_generated_include.hpp>

namespace LUA_MODULE_NAME {
	// InputArray, outputArray, InputOutputArray
	// InputArrayOfArrays, outputArrayOfArrays, InputOutputArrayOfArrays
	template<typename Array, typename _To, bool is_arrays_type>
	struct _OptionalArray
	{
		_OptionalArray() = default;

		_OptionalArray(_To& obj) {
			if (obj == sol::lua_nil) {
				if constexpr (is_arrays_type) {
					setField(*this, *this, 99);
				} else {
					setField(*this, *this, 100);
				}
				return;
			}

			auto maybe_Array = maybe_impl(obj, static_cast<Array*>(nullptr));
			if (maybe_Array) {
				ptr = reference_internal(*maybe_Array);
				return;
			}

			if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {
				auto maybe_Bool = maybe_impl(obj, static_cast<bool*>(nullptr));
				if (maybe_Bool) {
					this->obj = new std::shared_ptr<bool>(maybe_Bool);
					setField(*this, *this, 1);
					return;
				}
			}

			if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {
				auto maybe_Double = maybe_impl(obj, static_cast<double*>(nullptr));
				if (maybe_Double) {
					this->obj = new std::shared_ptr<double>(maybe_Double);
					setField(*this, *this, 2);
					return;
				}
			}

			if constexpr (!is_arrays_type) {
				auto maybe_Mat = maybe_impl(obj, static_cast<cv::Mat*>(nullptr));
				if (maybe_Mat) {
					this->obj = new std::shared_ptr<cv::Mat>(maybe_Mat);
					setField(*this, *this, 3);
					return;
				}
			}

			if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {
				auto maybe_Scalar = maybe_impl(obj, static_cast<cv::Scalar*>(nullptr));
				if (maybe_Scalar) {
					this->obj = new std::shared_ptr<cv::Scalar>(maybe_Scalar);
					setField(*this, *this, 4);
					return;
				}
			}

			auto maybe_VectorOfMat = maybe_impl(obj, static_cast<std::vector<cv::Mat>*>(nullptr));
			if (maybe_VectorOfMat) {
				this->obj = new std::shared_ptr<std::vector<cv::Mat>>(maybe_VectorOfMat);
				setField(*this, *this, 5);
				return;
			}

			auto maybe_VectorOfUMat = maybe_impl(obj, static_cast<std::vector<cv::UMat>*>(nullptr));
			if (maybe_VectorOfUMat) {
				this->obj = new std::shared_ptr<std::vector<cv::UMat>>(maybe_VectorOfUMat);
				setField(*this, *this, 6);
				return;
			}

			if constexpr (std::is_same_v<Array, cv::_InputArray>) {
				auto maybe_VectorOfBool = maybe_impl(obj, static_cast<std::vector<bool>*>(nullptr));
				if (maybe_VectorOfBool) {
					this->obj = new std::shared_ptr<std::vector<bool>>(maybe_VectorOfBool);
					setField(*this, *this, 7);
					return;
				}
			}

			auto maybe_VectorOfRotatedRect = maybe_impl(obj, static_cast<std::vector<cv::RotatedRect>*>(nullptr));
			if (maybe_VectorOfRotatedRect) {
				this->obj = new std::shared_ptr<std::vector<cv::RotatedRect>>(maybe_VectorOfRotatedRect);
				setField(*this, *this, 8);
				return;
			}

			auto maybe_VectorOfRange = maybe_impl(obj, static_cast<std::vector<cv::Range>*>(nullptr));
			if (maybe_VectorOfRange) {
				this->obj = new std::shared_ptr<std::vector<cv::Range>>(maybe_VectorOfRange);
				setField(*this, *this, 9);
				return;
			}

			auto maybe_VectorOfMoments = maybe_impl(obj, static_cast<std::vector<cv::Moments>*>(nullptr));
			if (maybe_VectorOfMoments) {
				this->obj = new std::shared_ptr<std::vector<cv::Moments>>(maybe_VectorOfMoments);
				setField(*this, *this, 10);
				return;
			}

			auto maybe_VectorOfUchar = maybe_impl(obj, static_cast<std::vector<uchar>*>(nullptr));
			if (maybe_VectorOfUchar) {
				this->obj = new std::shared_ptr<std::vector<uchar>>(maybe_VectorOfUchar);
				setField(*this, *this, 11);
				return;
			}

			auto maybe_VectorOfSchar = maybe_impl(obj, static_cast<std::vector<schar>*>(nullptr));
			if (maybe_VectorOfSchar) {
				this->obj = new std::shared_ptr<std::vector<schar>>(maybe_VectorOfSchar);
				setField(*this, *this, 12);
				return;
			}

			auto maybe_VectorOfChar = maybe_impl(obj, static_cast<std::vector<char>*>(nullptr));
			if (maybe_VectorOfChar) {
				this->obj = new std::shared_ptr<std::vector<char>>(maybe_VectorOfChar);
				setField(*this, *this, 13);
				return;
			}

			auto maybe_VectorOfUshort = maybe_impl(obj, static_cast<std::vector<ushort>*>(nullptr));
			if (maybe_VectorOfUshort) {
				this->obj = new std::shared_ptr<std::vector<ushort>>(maybe_VectorOfUshort);
				setField(*this, *this, 14);
				return;
			}

			auto maybe_VectorOfShort = maybe_impl(obj, static_cast<std::vector<short>*>(nullptr));
			if (maybe_VectorOfShort) {
				this->obj = new std::shared_ptr<std::vector<short>>(maybe_VectorOfShort);
				setField(*this, *this, 15);
				return;
			}

			auto maybe_VectorOfInt = maybe_impl(obj, static_cast<std::vector<int>*>(nullptr));
			if (maybe_VectorOfInt) {
				this->obj = new std::shared_ptr<std::vector<int>>(maybe_VectorOfInt);
				setField(*this, *this, 16);
				return;
			}

			auto maybe_VectorOfFloat = maybe_impl(obj, static_cast<std::vector<float>*>(nullptr));
			if (maybe_VectorOfFloat) {
				this->obj = new std::shared_ptr<std::vector<float>>(maybe_VectorOfFloat);
				setField(*this, *this, 17);
				return;
			}

			auto maybe_VectorOfDouble = maybe_impl(obj, static_cast<std::vector<double>*>(nullptr));
			if (maybe_VectorOfDouble) {
				this->obj = new std::shared_ptr<std::vector<double>>(maybe_VectorOfDouble);
				setField(*this, *this, 18);
				return;
			}

			auto maybe_VectorOfPoint3i = maybe_impl(obj, static_cast<std::vector<cv::Point3i>*>(nullptr));
			if (maybe_VectorOfPoint3i) {
				this->obj = new std::shared_ptr<std::vector<cv::Point3i>>(maybe_VectorOfPoint3i);
				setField(*this, *this, 19);
				return;
			}

			auto maybe_VectorOfPoint3f = maybe_impl(obj, static_cast<std::vector<cv::Point3f>*>(nullptr));
			if (maybe_VectorOfPoint3f) {
				this->obj = new std::shared_ptr<std::vector<cv::Point3f>>(maybe_VectorOfPoint3f);
				setField(*this, *this, 20);
				return;
			}

			auto maybe_VectorOfPoint3d = maybe_impl(obj, static_cast<std::vector<cv::Point3d>*>(nullptr));
			if (maybe_VectorOfPoint3d) {
				this->obj = new std::shared_ptr<std::vector<cv::Point3d>>(maybe_VectorOfPoint3d);
				setField(*this, *this, 21);
				return;
			}

			auto maybe_VectorOfPoint2f = maybe_impl(obj, static_cast<std::vector<cv::Point2f>*>(nullptr));
			if (maybe_VectorOfPoint2f) {
				this->obj = new std::shared_ptr<std::vector<cv::Point2f>>(maybe_VectorOfPoint2f);
				setField(*this, *this, 22);
				return;
			}

			auto maybe_VectorOfPoint2d = maybe_impl(obj, static_cast<std::vector<cv::Point2d>*>(nullptr));
			if (maybe_VectorOfPoint2d) {
				this->obj = new std::shared_ptr<std::vector<cv::Point2d>>(maybe_VectorOfPoint2d);
				setField(*this, *this, 23);
				return;
			}

			auto maybe_VectorOfPoint = maybe_impl(obj, static_cast<std::vector<cv::Point>*>(nullptr));
			if (maybe_VectorOfPoint) {
				this->obj = new std::shared_ptr<std::vector<cv::Point>>(maybe_VectorOfPoint);
				setField(*this, *this, 24);
				return;
			}

			auto maybe_VectorOfRect2f = maybe_impl(obj, static_cast<std::vector<cv::Rect2f>*>(nullptr));
			if (maybe_VectorOfRect2f) {
				this->obj = new std::shared_ptr<std::vector<cv::Rect2f>>(maybe_VectorOfRect2f);
				setField(*this, *this, 25);
				return;
			}

			auto maybe_VectorOfRect2d = maybe_impl(obj, static_cast<std::vector<cv::Rect2d>*>(nullptr));
			if (maybe_VectorOfRect2d) {
				this->obj = new std::shared_ptr<std::vector<cv::Rect2d>>(maybe_VectorOfRect2d);
				setField(*this, *this, 26);
				return;
			}

			auto maybe_VectorOfRect = maybe_impl(obj, static_cast<std::vector<cv::Rect>*>(nullptr));
			if (maybe_VectorOfRect) {
				this->obj = new std::shared_ptr<std::vector<cv::Rect>>(maybe_VectorOfRect);
				setField(*this, *this, 27);
				return;
			}

			auto maybe_VectorOfSize2f = maybe_impl(obj, static_cast<std::vector<cv::Size2f>*>(nullptr));
			if (maybe_VectorOfSize2f) {
				this->obj = new std::shared_ptr<std::vector<cv::Size2f>>(maybe_VectorOfSize2f);
				setField(*this, *this, 28);
				return;
			}

			auto maybe_VectorOfSize2d = maybe_impl(obj, static_cast<std::vector<cv::Size2d>*>(nullptr));
			if (maybe_VectorOfSize2d) {
				this->obj = new std::shared_ptr<std::vector<cv::Size2d>>(maybe_VectorOfSize2d);
				setField(*this, *this, 29);
				return;
			}

			auto maybe_VectorOfSize = maybe_impl(obj, static_cast<std::vector<cv::Size>*>(nullptr));
			if (maybe_VectorOfSize) {
				this->obj = new std::shared_ptr<std::vector<cv::Size>>(maybe_VectorOfSize);
				setField(*this, *this, 30);
				return;
			}

			auto maybe_VectorOfVec2b = maybe_impl(obj, static_cast<std::vector<cv::Vec2b>*>(nullptr));
			if (maybe_VectorOfVec2b) {
				this->obj = new std::shared_ptr<std::vector<cv::Vec2b>>(maybe_VectorOfVec2b);
				setField(*this, *this, 31);
				return;
			}

			auto maybe_VectorOfVec3b = maybe_impl(obj, static_cast<std::vector<cv::Vec3b>*>(nullptr));
			if (maybe_VectorOfVec3b) {
				this->obj = new std::shared_ptr<std::vector<cv::Vec3b>>(maybe_VectorOfVec3b);
				setField(*this, *this, 32);
				return;
			}

			auto maybe_VectorOfVec4b = maybe_impl(obj, static_cast<std::vector<cv::Vec4b>*>(nullptr));
			if (maybe_VectorOfVec4b) {
				this->obj = new std::shared_ptr<std::vector<cv::Vec4b>>(maybe_VectorOfVec4b);
				setField(*this, *this, 33);
				return;
			}

			auto maybe_VectorOfVec2s = maybe_impl(obj, static_cast<std::vector<cv::Vec2s>*>(nullptr));
			if (maybe_VectorOfVec2s) {
				this->obj = new std::shared_ptr<std::vector<cv::Vec2s>>(maybe_VectorOfVec2s);
				setField(*this, *this, 34);
				return;
			}

			auto maybe_VectorOfVec3s = maybe_impl(obj, static_cast<std::vector<cv::Vec3s>*>(nullptr));
			if (maybe_VectorOfVec3s) {
				this->obj = new std::shared_ptr<std::vector<cv::Vec3s>>(maybe_VectorOfVec3s);
				setField(*this, *this, 35);
				return;
			}

			auto maybe_VectorOfVec4s = maybe_impl(obj, static_cast<std::vector<cv::Vec4s>*>(nullptr));
			if (maybe_VectorOfVec4s) {
				this->obj = new std::shared_ptr<std::vector<cv::Vec4s>>(maybe_VectorOfVec4s);
				setField(*this, *this, 36);
				return;
			}

			auto maybe_VectorOfVec2w = maybe_impl(obj, static_cast<std::vector<cv::Vec2w>*>(nullptr));
			if (maybe_VectorOfVec2w) {
				this->obj = new std::shared_ptr<std::vector<cv::Vec2w>>(maybe_VectorOfVec2w);
				setField(*this, *this, 37);
				return;
			}

			auto maybe_VectorOfVec3w = maybe_impl(obj, static_cast<std::vector<cv::Vec3w>*>(nullptr));
			if (maybe_VectorOfVec3w) {
				this->obj = new std::shared_ptr<std::vector<cv::Vec3w>>(maybe_VectorOfVec3w);
				setField(*this, *this, 38);
				return;
			}

			auto maybe_VectorOfVec4w = maybe_impl(obj, static_cast<std::vector<cv::Vec4w>*>(nullptr));
			if (maybe_VectorOfVec4w) {
				this->obj = new std::shared_ptr<std::vector<cv::Vec4w>>(maybe_VectorOfVec4w);
				setField(*this, *this, 39);
				return;
			}

			auto maybe_VectorOfVec2i = maybe_impl(obj, static_cast<std::vector<cv::Vec2i>*>(nullptr));
			if (maybe_VectorOfVec2i) {
				this->obj = new std::shared_ptr<std::vector<cv::Vec2i>>(maybe_VectorOfVec2i);
				setField(*this, *this, 40);
				return;
			}

			auto maybe_VectorOfVec3i = maybe_impl(obj, static_cast<std::vector<cv::Vec3i>*>(nullptr));
			if (maybe_VectorOfVec3i) {
				this->obj = new std::shared_ptr<std::vector<cv::Vec3i>>(maybe_VectorOfVec3i);
				setField(*this, *this, 41);
				return;
			}

			auto maybe_VectorOfVec4i = maybe_impl(obj, static_cast<std::vector<cv::Vec4i>*>(nullptr));
			if (maybe_VectorOfVec4i) {
				this->obj = new std::shared_ptr<std::vector<cv::Vec4i>>(maybe_VectorOfVec4i);
				setField(*this, *this, 42);
				return;
			}

			auto maybe_VectorOfVec6i = maybe_impl(obj, static_cast<std::vector<cv::Vec6i>*>(nullptr));
			if (maybe_VectorOfVec6i) {
				this->obj = new std::shared_ptr<std::vector<cv::Vec6i>>(maybe_VectorOfVec6i);
				setField(*this, *this, 43);
				return;
			}

			auto maybe_VectorOfVec8i = maybe_impl(obj, static_cast<std::vector<cv::Vec8i>*>(nullptr));
			if (maybe_VectorOfVec8i) {
				this->obj = new std::shared_ptr<std::vector<cv::Vec8i>>(maybe_VectorOfVec8i);
				setField(*this, *this, 44);
				return;
			}

			auto maybe_VectorOfVec2f = maybe_impl(obj, static_cast<std::vector<cv::Vec2f>*>(nullptr));
			if (maybe_VectorOfVec2f) {
				this->obj = new std::shared_ptr<std::vector<cv::Vec2f>>(maybe_VectorOfVec2f);
				setField(*this, *this, 45);
				return;
			}

			auto maybe_VectorOfVec3f = maybe_impl(obj, static_cast<std::vector<cv::Vec3f>*>(nullptr));
			if (maybe_VectorOfVec3f) {
				this->obj = new std::shared_ptr<std::vector<cv::Vec3f>>(maybe_VectorOfVec3f);
				setField(*this, *this, 46);
				return;
			}

			auto maybe_VectorOfVec4f = maybe_impl(obj, static_cast<std::vector<cv::Vec4f>*>(nullptr));
			if (maybe_VectorOfVec4f) {
				this->obj = new std::shared_ptr<std::vector<cv::Vec4f>>(maybe_VectorOfVec4f);
				setField(*this, *this, 47);
				return;
			}

			auto maybe_VectorOfVec6f = maybe_impl(obj, static_cast<std::vector<cv::Vec6f>*>(nullptr));
			if (maybe_VectorOfVec6f) {
				this->obj = new std::shared_ptr<std::vector<cv::Vec6f>>(maybe_VectorOfVec6f);
				setField(*this, *this, 48);
				return;
			}

			auto maybe_VectorOfVec2d = maybe_impl(obj, static_cast<std::vector<cv::Vec2d>*>(nullptr));
			if (maybe_VectorOfVec2d) {
				this->obj = new std::shared_ptr<std::vector<cv::Vec2d>>(maybe_VectorOfVec2d);
				setField(*this, *this, 49);
				return;
			}

			auto maybe_VectorOfVec3d = maybe_impl(obj, static_cast<std::vector<cv::Vec3d>*>(nullptr));
			if (maybe_VectorOfVec3d) {
				this->obj = new std::shared_ptr<std::vector<cv::Vec3d>>(maybe_VectorOfVec3d);
				setField(*this, *this, 50);
				return;
			}

			auto maybe_VectorOfVec4d = maybe_impl(obj, static_cast<std::vector<cv::Vec4d>*>(nullptr));
			if (maybe_VectorOfVec4d) {
				this->obj = new std::shared_ptr<std::vector<cv::Vec4d>>(maybe_VectorOfVec4d);
				setField(*this, *this, 51);
				return;
			}

			auto maybe_VectorOfVec6d = maybe_impl(obj, static_cast<std::vector<cv::Vec6d>*>(nullptr));
			if (maybe_VectorOfVec6d) {
				this->obj = new std::shared_ptr<std::vector<cv::Vec6d>>(maybe_VectorOfVec6d);
				setField(*this, *this, 52);
				return;
			}

			if constexpr (std::is_same_v<Array, cv::_InputOutputArray>) {
				auto maybe_VectorOfVectorOfBool = maybe_impl(obj, static_cast<std::vector<std::vector<bool>>*>(nullptr));
				if (maybe_VectorOfVectorOfBool) {
					this->obj = new std::shared_ptr<std::vector<std::vector<bool>>>(maybe_VectorOfVectorOfBool);
					setField(*this, *this, 53);
					return;
				}
			}

			auto maybe_VectorOfVectorOfRotatedRect = maybe_impl(obj, static_cast<std::vector<std::vector<cv::RotatedRect>>*>(nullptr));
			if (maybe_VectorOfVectorOfRotatedRect) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::RotatedRect>>>(maybe_VectorOfVectorOfRotatedRect);
				setField(*this, *this, 54);
				return;
			}

			auto maybe_VectorOfVectorOfRange = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Range>>*>(nullptr));
			if (maybe_VectorOfVectorOfRange) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Range>>>(maybe_VectorOfVectorOfRange);
				setField(*this, *this, 55);
				return;
			}

			auto maybe_VectorOfVectorOfMoments = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Moments>>*>(nullptr));
			if (maybe_VectorOfVectorOfMoments) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Moments>>>(maybe_VectorOfVectorOfMoments);
				setField(*this, *this, 56);
				return;
			}

			auto maybe_VectorOfVectorOfUchar = maybe_impl(obj, static_cast<std::vector<std::vector<uchar>>*>(nullptr));
			if (maybe_VectorOfVectorOfUchar) {
				this->obj = new std::shared_ptr<std::vector<std::vector<uchar>>>(maybe_VectorOfVectorOfUchar);
				setField(*this, *this, 57);
				return;
			}

			auto maybe_VectorOfVectorOfSchar = maybe_impl(obj, static_cast<std::vector<std::vector<schar>>*>(nullptr));
			if (maybe_VectorOfVectorOfSchar) {
				this->obj = new std::shared_ptr<std::vector<std::vector<schar>>>(maybe_VectorOfVectorOfSchar);
				setField(*this, *this, 58);
				return;
			}

			auto maybe_VectorOfVectorOfChar = maybe_impl(obj, static_cast<std::vector<std::vector<char>>*>(nullptr));
			if (maybe_VectorOfVectorOfChar) {
				this->obj = new std::shared_ptr<std::vector<std::vector<char>>>(maybe_VectorOfVectorOfChar);
				setField(*this, *this, 59);
				return;
			}

			auto maybe_VectorOfVectorOfUshort = maybe_impl(obj, static_cast<std::vector<std::vector<ushort>>*>(nullptr));
			if (maybe_VectorOfVectorOfUshort) {
				this->obj = new std::shared_ptr<std::vector<std::vector<ushort>>>(maybe_VectorOfVectorOfUshort);
				setField(*this, *this, 60);
				return;
			}

			auto maybe_VectorOfVectorOfShort = maybe_impl(obj, static_cast<std::vector<std::vector<short>>*>(nullptr));
			if (maybe_VectorOfVectorOfShort) {
				this->obj = new std::shared_ptr<std::vector<std::vector<short>>>(maybe_VectorOfVectorOfShort);
				setField(*this, *this, 61);
				return;
			}

			auto maybe_VectorOfVectorOfInt = maybe_impl(obj, static_cast<std::vector<std::vector<int>>*>(nullptr));
			if (maybe_VectorOfVectorOfInt) {
				this->obj = new std::shared_ptr<std::vector<std::vector<int>>>(maybe_VectorOfVectorOfInt);
				setField(*this, *this, 62);
				return;
			}

			auto maybe_VectorOfVectorOfFloat = maybe_impl(obj, static_cast<std::vector<std::vector<float>>*>(nullptr));
			if (maybe_VectorOfVectorOfFloat) {
				this->obj = new std::shared_ptr<std::vector<std::vector<float>>>(maybe_VectorOfVectorOfFloat);
				setField(*this, *this, 63);
				return;
			}

			auto maybe_VectorOfVectorOfDouble = maybe_impl(obj, static_cast<std::vector<std::vector<double>>*>(nullptr));
			if (maybe_VectorOfVectorOfDouble) {
				this->obj = new std::shared_ptr<std::vector<std::vector<double>>>(maybe_VectorOfVectorOfDouble);
				setField(*this, *this, 64);
				return;
			}

			auto maybe_VectorOfVectorOfPoint3i = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Point3i>>*>(nullptr));
			if (maybe_VectorOfVectorOfPoint3i) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Point3i>>>(maybe_VectorOfVectorOfPoint3i);
				setField(*this, *this, 65);
				return;
			}

			auto maybe_VectorOfVectorOfPoint3f = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Point3f>>*>(nullptr));
			if (maybe_VectorOfVectorOfPoint3f) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Point3f>>>(maybe_VectorOfVectorOfPoint3f);
				setField(*this, *this, 66);
				return;
			}

			auto maybe_VectorOfVectorOfPoint3d = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Point3d>>*>(nullptr));
			if (maybe_VectorOfVectorOfPoint3d) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Point3d>>>(maybe_VectorOfVectorOfPoint3d);
				setField(*this, *this, 67);
				return;
			}

			auto maybe_VectorOfVectorOfPoint2f = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Point2f>>*>(nullptr));
			if (maybe_VectorOfVectorOfPoint2f) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Point2f>>>(maybe_VectorOfVectorOfPoint2f);
				setField(*this, *this, 68);
				return;
			}

			auto maybe_VectorOfVectorOfPoint2d = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Point2d>>*>(nullptr));
			if (maybe_VectorOfVectorOfPoint2d) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Point2d>>>(maybe_VectorOfVectorOfPoint2d);
				setField(*this, *this, 69);
				return;
			}

			auto maybe_VectorOfVectorOfPoint = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Point>>*>(nullptr));
			if (maybe_VectorOfVectorOfPoint) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Point>>>(maybe_VectorOfVectorOfPoint);
				setField(*this, *this, 70);
				return;
			}

			auto maybe_VectorOfVectorOfRect2f = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Rect2f>>*>(nullptr));
			if (maybe_VectorOfVectorOfRect2f) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Rect2f>>>(maybe_VectorOfVectorOfRect2f);
				setField(*this, *this, 71);
				return;
			}

			auto maybe_VectorOfVectorOfRect2d = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Rect2d>>*>(nullptr));
			if (maybe_VectorOfVectorOfRect2d) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Rect2d>>>(maybe_VectorOfVectorOfRect2d);
				setField(*this, *this, 72);
				return;
			}

			auto maybe_VectorOfVectorOfRect = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Rect>>*>(nullptr));
			if (maybe_VectorOfVectorOfRect) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Rect>>>(maybe_VectorOfVectorOfRect);
				setField(*this, *this, 73);
				return;
			}

			auto maybe_VectorOfVectorOfSize2f = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Size2f>>*>(nullptr));
			if (maybe_VectorOfVectorOfSize2f) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Size2f>>>(maybe_VectorOfVectorOfSize2f);
				setField(*this, *this, 74);
				return;
			}

			auto maybe_VectorOfVectorOfSize2d = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Size2d>>*>(nullptr));
			if (maybe_VectorOfVectorOfSize2d) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Size2d>>>(maybe_VectorOfVectorOfSize2d);
				setField(*this, *this, 75);
				return;
			}

			auto maybe_VectorOfVectorOfSize = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Size>>*>(nullptr));
			if (maybe_VectorOfVectorOfSize) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Size>>>(maybe_VectorOfVectorOfSize);
				setField(*this, *this, 76);
				return;
			}

			auto maybe_VectorOfVectorOfVec2b = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Vec2b>>*>(nullptr));
			if (maybe_VectorOfVectorOfVec2b) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2b>>>(maybe_VectorOfVectorOfVec2b);
				setField(*this, *this, 77);
				return;
			}

			auto maybe_VectorOfVectorOfVec3b = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Vec3b>>*>(nullptr));
			if (maybe_VectorOfVectorOfVec3b) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3b>>>(maybe_VectorOfVectorOfVec3b);
				setField(*this, *this, 78);
				return;
			}

			auto maybe_VectorOfVectorOfVec4b = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Vec4b>>*>(nullptr));
			if (maybe_VectorOfVectorOfVec4b) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4b>>>(maybe_VectorOfVectorOfVec4b);
				setField(*this, *this, 79);
				return;
			}

			auto maybe_VectorOfVectorOfVec2s = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Vec2s>>*>(nullptr));
			if (maybe_VectorOfVectorOfVec2s) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2s>>>(maybe_VectorOfVectorOfVec2s);
				setField(*this, *this, 80);
				return;
			}

			auto maybe_VectorOfVectorOfVec3s = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Vec3s>>*>(nullptr));
			if (maybe_VectorOfVectorOfVec3s) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3s>>>(maybe_VectorOfVectorOfVec3s);
				setField(*this, *this, 81);
				return;
			}

			auto maybe_VectorOfVectorOfVec4s = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Vec4s>>*>(nullptr));
			if (maybe_VectorOfVectorOfVec4s) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4s>>>(maybe_VectorOfVectorOfVec4s);
				setField(*this, *this, 82);
				return;
			}

			auto maybe_VectorOfVectorOfVec2w = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Vec2w>>*>(nullptr));
			if (maybe_VectorOfVectorOfVec2w) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2w>>>(maybe_VectorOfVectorOfVec2w);
				setField(*this, *this, 83);
				return;
			}

			auto maybe_VectorOfVectorOfVec3w = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Vec3w>>*>(nullptr));
			if (maybe_VectorOfVectorOfVec3w) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3w>>>(maybe_VectorOfVectorOfVec3w);
				setField(*this, *this, 84);
				return;
			}

			auto maybe_VectorOfVectorOfVec4w = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Vec4w>>*>(nullptr));
			if (maybe_VectorOfVectorOfVec4w) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4w>>>(maybe_VectorOfVectorOfVec4w);
				setField(*this, *this, 85);
				return;
			}

			auto maybe_VectorOfVectorOfVec2i = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Vec2i>>*>(nullptr));
			if (maybe_VectorOfVectorOfVec2i) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2i>>>(maybe_VectorOfVectorOfVec2i);
				setField(*this, *this, 86);
				return;
			}

			auto maybe_VectorOfVectorOfVec3i = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Vec3i>>*>(nullptr));
			if (maybe_VectorOfVectorOfVec3i) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3i>>>(maybe_VectorOfVectorOfVec3i);
				setField(*this, *this, 87);
				return;
			}

			auto maybe_VectorOfVectorOfVec4i = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Vec4i>>*>(nullptr));
			if (maybe_VectorOfVectorOfVec4i) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4i>>>(maybe_VectorOfVectorOfVec4i);
				setField(*this, *this, 88);
				return;
			}

			auto maybe_VectorOfVectorOfVec6i = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Vec6i>>*>(nullptr));
			if (maybe_VectorOfVectorOfVec6i) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec6i>>>(maybe_VectorOfVectorOfVec6i);
				setField(*this, *this, 89);
				return;
			}

			auto maybe_VectorOfVectorOfVec8i = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Vec8i>>*>(nullptr));
			if (maybe_VectorOfVectorOfVec8i) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec8i>>>(maybe_VectorOfVectorOfVec8i);
				setField(*this, *this, 90);
				return;
			}

			auto maybe_VectorOfVectorOfVec2f = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Vec2f>>*>(nullptr));
			if (maybe_VectorOfVectorOfVec2f) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2f>>>(maybe_VectorOfVectorOfVec2f);
				setField(*this, *this, 91);
				return;
			}

			auto maybe_VectorOfVectorOfVec3f = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Vec3f>>*>(nullptr));
			if (maybe_VectorOfVectorOfVec3f) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3f>>>(maybe_VectorOfVectorOfVec3f);
				setField(*this, *this, 92);
				return;
			}

			auto maybe_VectorOfVectorOfVec4f = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Vec4f>>*>(nullptr));
			if (maybe_VectorOfVectorOfVec4f) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4f>>>(maybe_VectorOfVectorOfVec4f);
				setField(*this, *this, 93);
				return;
			}

			auto maybe_VectorOfVectorOfVec6f = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Vec6f>>*>(nullptr));
			if (maybe_VectorOfVectorOfVec6f) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec6f>>>(maybe_VectorOfVectorOfVec6f);
				setField(*this, *this, 94);
				return;
			}

			auto maybe_VectorOfVectorOfVec2d = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Vec2d>>*>(nullptr));
			if (maybe_VectorOfVectorOfVec2d) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2d>>>(maybe_VectorOfVectorOfVec2d);
				setField(*this, *this, 95);
				return;
			}

			auto maybe_VectorOfVectorOfVec3d = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Vec3d>>*>(nullptr));
			if (maybe_VectorOfVectorOfVec3d) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3d>>>(maybe_VectorOfVectorOfVec3d);
				setField(*this, *this, 96);
				return;
			}

			auto maybe_VectorOfVectorOfVec4d = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Vec4d>>*>(nullptr));
			if (maybe_VectorOfVectorOfVec4d) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4d>>>(maybe_VectorOfVectorOfVec4d);
				setField(*this, *this, 97);
				return;
			}

			auto maybe_VectorOfVectorOfVec6d = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Vec6d>>*>(nullptr));
			if (maybe_VectorOfVectorOfVec6d) {
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec6d>>>(maybe_VectorOfVectorOfVec6d);
				setField(*this, *this, 98);
				return;
			}
		}

		_OptionalArray(const _To& obj) : _OptionalArray(const_cast<_To&>(obj)) {}

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
					this->obj = new std::shared_ptr<cv::Mat>(std::move(*reinterpret_cast<std::shared_ptr<cv::Mat>*>(other.obj)));
				}
				break;
			case 4:
				if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {
					this->obj = new std::shared_ptr<cv::Scalar>(std::move(*reinterpret_cast<std::shared_ptr<cv::Scalar>*>(other.obj)));
				}
				break;
			case 5:
				this->obj = new std::shared_ptr<std::vector<cv::Mat>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Mat>>*>(other.obj)));
				break;
			case 6:
				this->obj = new std::shared_ptr<std::vector<cv::UMat>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::UMat>>*>(other.obj)));
				break;
			case 7:
				if constexpr (std::is_same_v<Array, cv::_InputArray>) {
					this->obj = new std::shared_ptr<std::vector<bool>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<bool>>*>(other.obj)));
				}
				break;
			case 8:
				this->obj = new std::shared_ptr<std::vector<cv::RotatedRect>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::RotatedRect>>*>(other.obj)));
				break;
			case 9:
				this->obj = new std::shared_ptr<std::vector<cv::Range>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Range>>*>(other.obj)));
				break;
			case 10:
				this->obj = new std::shared_ptr<std::vector<cv::Moments>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Moments>>*>(other.obj)));
				break;
			case 11:
				this->obj = new std::shared_ptr<std::vector<uchar>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<uchar>>*>(other.obj)));
				break;
			case 12:
				this->obj = new std::shared_ptr<std::vector<schar>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<schar>>*>(other.obj)));
				break;
			case 13:
				this->obj = new std::shared_ptr<std::vector<char>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<char>>*>(other.obj)));
				break;
			case 14:
				this->obj = new std::shared_ptr<std::vector<ushort>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<ushort>>*>(other.obj)));
				break;
			case 15:
				this->obj = new std::shared_ptr<std::vector<short>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<short>>*>(other.obj)));
				break;
			case 16:
				this->obj = new std::shared_ptr<std::vector<int>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<int>>*>(other.obj)));
				break;
			case 17:
				this->obj = new std::shared_ptr<std::vector<float>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<float>>*>(other.obj)));
				break;
			case 18:
				this->obj = new std::shared_ptr<std::vector<double>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<double>>*>(other.obj)));
				break;
			case 19:
				this->obj = new std::shared_ptr<std::vector<cv::Point3i>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point3i>>*>(other.obj)));
				break;
			case 20:
				this->obj = new std::shared_ptr<std::vector<cv::Point3f>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point3f>>*>(other.obj)));
				break;
			case 21:
				this->obj = new std::shared_ptr<std::vector<cv::Point3d>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point3d>>*>(other.obj)));
				break;
			case 22:
				this->obj = new std::shared_ptr<std::vector<cv::Point2f>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point2f>>*>(other.obj)));
				break;
			case 23:
				this->obj = new std::shared_ptr<std::vector<cv::Point2d>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point2d>>*>(other.obj)));
				break;
			case 24:
				this->obj = new std::shared_ptr<std::vector<cv::Point>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point>>*>(other.obj)));
				break;
			case 25:
				this->obj = new std::shared_ptr<std::vector<cv::Rect2f>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2f>>*>(other.obj)));
				break;
			case 26:
				this->obj = new std::shared_ptr<std::vector<cv::Rect2d>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2d>>*>(other.obj)));
				break;
			case 27:
				this->obj = new std::shared_ptr<std::vector<cv::Rect>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Rect>>*>(other.obj)));
				break;
			case 28:
				this->obj = new std::shared_ptr<std::vector<cv::Size2f>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Size2f>>*>(other.obj)));
				break;
			case 29:
				this->obj = new std::shared_ptr<std::vector<cv::Size2d>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Size2d>>*>(other.obj)));
				break;
			case 30:
				this->obj = new std::shared_ptr<std::vector<cv::Size>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Size>>*>(other.obj)));
				break;
			case 31:
				this->obj = new std::shared_ptr<std::vector<cv::Vec2b>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2b>>*>(other.obj)));
				break;
			case 32:
				this->obj = new std::shared_ptr<std::vector<cv::Vec3b>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3b>>*>(other.obj)));
				break;
			case 33:
				this->obj = new std::shared_ptr<std::vector<cv::Vec4b>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4b>>*>(other.obj)));
				break;
			case 34:
				this->obj = new std::shared_ptr<std::vector<cv::Vec2s>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2s>>*>(other.obj)));
				break;
			case 35:
				this->obj = new std::shared_ptr<std::vector<cv::Vec3s>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3s>>*>(other.obj)));
				break;
			case 36:
				this->obj = new std::shared_ptr<std::vector<cv::Vec4s>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4s>>*>(other.obj)));
				break;
			case 37:
				this->obj = new std::shared_ptr<std::vector<cv::Vec2w>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2w>>*>(other.obj)));
				break;
			case 38:
				this->obj = new std::shared_ptr<std::vector<cv::Vec3w>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3w>>*>(other.obj)));
				break;
			case 39:
				this->obj = new std::shared_ptr<std::vector<cv::Vec4w>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4w>>*>(other.obj)));
				break;
			case 40:
				this->obj = new std::shared_ptr<std::vector<cv::Vec2i>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2i>>*>(other.obj)));
				break;
			case 41:
				this->obj = new std::shared_ptr<std::vector<cv::Vec3i>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3i>>*>(other.obj)));
				break;
			case 42:
				this->obj = new std::shared_ptr<std::vector<cv::Vec4i>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4i>>*>(other.obj)));
				break;
			case 43:
				this->obj = new std::shared_ptr<std::vector<cv::Vec6i>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6i>>*>(other.obj)));
				break;
			case 44:
				this->obj = new std::shared_ptr<std::vector<cv::Vec8i>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec8i>>*>(other.obj)));
				break;
			case 45:
				this->obj = new std::shared_ptr<std::vector<cv::Vec2f>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2f>>*>(other.obj)));
				break;
			case 46:
				this->obj = new std::shared_ptr<std::vector<cv::Vec3f>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3f>>*>(other.obj)));
				break;
			case 47:
				this->obj = new std::shared_ptr<std::vector<cv::Vec4f>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4f>>*>(other.obj)));
				break;
			case 48:
				this->obj = new std::shared_ptr<std::vector<cv::Vec6f>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6f>>*>(other.obj)));
				break;
			case 49:
				this->obj = new std::shared_ptr<std::vector<cv::Vec2d>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2d>>*>(other.obj)));
				break;
			case 50:
				this->obj = new std::shared_ptr<std::vector<cv::Vec3d>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3d>>*>(other.obj)));
				break;
			case 51:
				this->obj = new std::shared_ptr<std::vector<cv::Vec4d>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4d>>*>(other.obj)));
				break;
			case 52:
				this->obj = new std::shared_ptr<std::vector<cv::Vec6d>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6d>>*>(other.obj)));
				break;
			case 53:
				if constexpr (std::is_same_v<Array, cv::_InputOutputArray>) {
					this->obj = new std::shared_ptr<std::vector<std::vector<bool>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<bool>>>*>(other.obj)));
				}
				break;
			case 54:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::RotatedRect>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::RotatedRect>>>*>(other.obj)));
				break;
			case 55:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Range>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Range>>>*>(other.obj)));
				break;
			case 56:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Moments>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Moments>>>*>(other.obj)));
				break;
			case 57:
				this->obj = new std::shared_ptr<std::vector<std::vector<uchar>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<uchar>>>*>(other.obj)));
				break;
			case 58:
				this->obj = new std::shared_ptr<std::vector<std::vector<schar>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<schar>>>*>(other.obj)));
				break;
			case 59:
				this->obj = new std::shared_ptr<std::vector<std::vector<char>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<char>>>*>(other.obj)));
				break;
			case 60:
				this->obj = new std::shared_ptr<std::vector<std::vector<ushort>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<ushort>>>*>(other.obj)));
				break;
			case 61:
				this->obj = new std::shared_ptr<std::vector<std::vector<short>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<short>>>*>(other.obj)));
				break;
			case 62:
				this->obj = new std::shared_ptr<std::vector<std::vector<int>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<int>>>*>(other.obj)));
				break;
			case 63:
				this->obj = new std::shared_ptr<std::vector<std::vector<float>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<float>>>*>(other.obj)));
				break;
			case 64:
				this->obj = new std::shared_ptr<std::vector<std::vector<double>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<double>>>*>(other.obj)));
				break;
			case 65:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Point3i>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3i>>>*>(other.obj)));
				break;
			case 66:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Point3f>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3f>>>*>(other.obj)));
				break;
			case 67:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Point3d>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3d>>>*>(other.obj)));
				break;
			case 68:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Point2f>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2f>>>*>(other.obj)));
				break;
			case 69:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Point2d>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2d>>>*>(other.obj)));
				break;
			case 70:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Point>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point>>>*>(other.obj)));
				break;
			case 71:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Rect2f>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2f>>>*>(other.obj)));
				break;
			case 72:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Rect2d>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2d>>>*>(other.obj)));
				break;
			case 73:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Rect>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect>>>*>(other.obj)));
				break;
			case 74:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Size2f>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2f>>>*>(other.obj)));
				break;
			case 75:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Size2d>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2d>>>*>(other.obj)));
				break;
			case 76:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Size>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size>>>*>(other.obj)));
				break;
			case 77:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2b>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2b>>>*>(other.obj)));
				break;
			case 78:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3b>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3b>>>*>(other.obj)));
				break;
			case 79:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4b>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4b>>>*>(other.obj)));
				break;
			case 80:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2s>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2s>>>*>(other.obj)));
				break;
			case 81:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3s>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3s>>>*>(other.obj)));
				break;
			case 82:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4s>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4s>>>*>(other.obj)));
				break;
			case 83:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2w>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2w>>>*>(other.obj)));
				break;
			case 84:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3w>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3w>>>*>(other.obj)));
				break;
			case 85:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4w>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4w>>>*>(other.obj)));
				break;
			case 86:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2i>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2i>>>*>(other.obj)));
				break;
			case 87:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3i>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3i>>>*>(other.obj)));
				break;
			case 88:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4i>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4i>>>*>(other.obj)));
				break;
			case 89:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec6i>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6i>>>*>(other.obj)));
				break;
			case 90:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec8i>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec8i>>>*>(other.obj)));
				break;
			case 91:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2f>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2f>>>*>(other.obj)));
				break;
			case 92:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3f>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3f>>>*>(other.obj)));
				break;
			case 93:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4f>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4f>>>*>(other.obj)));
				break;
			case 94:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec6f>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6f>>>*>(other.obj)));
				break;
			case 95:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2d>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2d>>>*>(other.obj)));
				break;
			case 96:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3d>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3d>>>*>(other.obj)));
				break;
			case 97:
				this->obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4d>>>(std::move(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4d>>>*>(other.obj)));
				break;
			case 98:
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
					delete reinterpret_cast<std::shared_ptr<cv::Mat>*>(obj);
					obj = nullptr;
				}
				break;
			case 4:
				if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {
					delete reinterpret_cast<std::shared_ptr<cv::Scalar>*>(obj);
					obj = nullptr;
				}
				break;
			case 5:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Mat>>*>(obj);
				obj = nullptr;
				break;
			case 6:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::UMat>>*>(obj);
				obj = nullptr;
				break;
			case 7:
				if constexpr (std::is_same_v<Array, cv::_InputArray>) {
					delete reinterpret_cast<std::shared_ptr<std::vector<bool>>*>(obj);
					obj = nullptr;
				}
				break;
			case 8:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::RotatedRect>>*>(obj);
				obj = nullptr;
				break;
			case 9:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Range>>*>(obj);
				obj = nullptr;
				break;
			case 10:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Moments>>*>(obj);
				obj = nullptr;
				break;
			case 11:
				delete reinterpret_cast<std::shared_ptr<std::vector<uchar>>*>(obj);
				obj = nullptr;
				break;
			case 12:
				delete reinterpret_cast<std::shared_ptr<std::vector<schar>>*>(obj);
				obj = nullptr;
				break;
			case 13:
				delete reinterpret_cast<std::shared_ptr<std::vector<char>>*>(obj);
				obj = nullptr;
				break;
			case 14:
				delete reinterpret_cast<std::shared_ptr<std::vector<ushort>>*>(obj);
				obj = nullptr;
				break;
			case 15:
				delete reinterpret_cast<std::shared_ptr<std::vector<short>>*>(obj);
				obj = nullptr;
				break;
			case 16:
				delete reinterpret_cast<std::shared_ptr<std::vector<int>>*>(obj);
				obj = nullptr;
				break;
			case 17:
				delete reinterpret_cast<std::shared_ptr<std::vector<float>>*>(obj);
				obj = nullptr;
				break;
			case 18:
				delete reinterpret_cast<std::shared_ptr<std::vector<double>>*>(obj);
				obj = nullptr;
				break;
			case 19:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Point3i>>*>(obj);
				obj = nullptr;
				break;
			case 20:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Point3f>>*>(obj);
				obj = nullptr;
				break;
			case 21:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Point3d>>*>(obj);
				obj = nullptr;
				break;
			case 22:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Point2f>>*>(obj);
				obj = nullptr;
				break;
			case 23:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Point2d>>*>(obj);
				obj = nullptr;
				break;
			case 24:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Point>>*>(obj);
				obj = nullptr;
				break;
			case 25:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2f>>*>(obj);
				obj = nullptr;
				break;
			case 26:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2d>>*>(obj);
				obj = nullptr;
				break;
			case 27:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Rect>>*>(obj);
				obj = nullptr;
				break;
			case 28:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Size2f>>*>(obj);
				obj = nullptr;
				break;
			case 29:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Size2d>>*>(obj);
				obj = nullptr;
				break;
			case 30:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Size>>*>(obj);
				obj = nullptr;
				break;
			case 31:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2b>>*>(obj);
				obj = nullptr;
				break;
			case 32:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3b>>*>(obj);
				obj = nullptr;
				break;
			case 33:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4b>>*>(obj);
				obj = nullptr;
				break;
			case 34:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2s>>*>(obj);
				obj = nullptr;
				break;
			case 35:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3s>>*>(obj);
				obj = nullptr;
				break;
			case 36:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4s>>*>(obj);
				obj = nullptr;
				break;
			case 37:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2w>>*>(obj);
				obj = nullptr;
				break;
			case 38:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3w>>*>(obj);
				obj = nullptr;
				break;
			case 39:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4w>>*>(obj);
				obj = nullptr;
				break;
			case 40:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2i>>*>(obj);
				obj = nullptr;
				break;
			case 41:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3i>>*>(obj);
				obj = nullptr;
				break;
			case 42:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4i>>*>(obj);
				obj = nullptr;
				break;
			case 43:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6i>>*>(obj);
				obj = nullptr;
				break;
			case 44:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec8i>>*>(obj);
				obj = nullptr;
				break;
			case 45:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2f>>*>(obj);
				obj = nullptr;
				break;
			case 46:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3f>>*>(obj);
				obj = nullptr;
				break;
			case 47:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4f>>*>(obj);
				obj = nullptr;
				break;
			case 48:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6f>>*>(obj);
				obj = nullptr;
				break;
			case 49:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2d>>*>(obj);
				obj = nullptr;
				break;
			case 50:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3d>>*>(obj);
				obj = nullptr;
				break;
			case 51:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4d>>*>(obj);
				obj = nullptr;
				break;
			case 52:
				delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6d>>*>(obj);
				obj = nullptr;
				break;
			case 53:
				if constexpr (std::is_same_v<Array, cv::_InputOutputArray>) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<bool>>>*>(obj);
					obj = nullptr;
				}
				break;
			case 54:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::RotatedRect>>>*>(obj);
				obj = nullptr;
				break;
			case 55:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Range>>>*>(obj);
				obj = nullptr;
				break;
			case 56:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Moments>>>*>(obj);
				obj = nullptr;
				break;
			case 57:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<uchar>>>*>(obj);
				obj = nullptr;
				break;
			case 58:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<schar>>>*>(obj);
				obj = nullptr;
				break;
			case 59:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<char>>>*>(obj);
				obj = nullptr;
				break;
			case 60:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<ushort>>>*>(obj);
				obj = nullptr;
				break;
			case 61:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<short>>>*>(obj);
				obj = nullptr;
				break;
			case 62:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<int>>>*>(obj);
				obj = nullptr;
				break;
			case 63:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<float>>>*>(obj);
				obj = nullptr;
				break;
			case 64:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<double>>>*>(obj);
				obj = nullptr;
				break;
			case 65:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3i>>>*>(obj);
				obj = nullptr;
				break;
			case 66:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3f>>>*>(obj);
				obj = nullptr;
				break;
			case 67:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3d>>>*>(obj);
				obj = nullptr;
				break;
			case 68:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2f>>>*>(obj);
				obj = nullptr;
				break;
			case 69:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2d>>>*>(obj);
				obj = nullptr;
				break;
			case 70:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point>>>*>(obj);
				obj = nullptr;
				break;
			case 71:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2f>>>*>(obj);
				obj = nullptr;
				break;
			case 72:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2d>>>*>(obj);
				obj = nullptr;
				break;
			case 73:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect>>>*>(obj);
				obj = nullptr;
				break;
			case 74:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2f>>>*>(obj);
				obj = nullptr;
				break;
			case 75:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2d>>>*>(obj);
				obj = nullptr;
				break;
			case 76:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size>>>*>(obj);
				obj = nullptr;
				break;
			case 77:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2b>>>*>(obj);
				obj = nullptr;
				break;
			case 78:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3b>>>*>(obj);
				obj = nullptr;
				break;
			case 79:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4b>>>*>(obj);
				obj = nullptr;
				break;
			case 80:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2s>>>*>(obj);
				obj = nullptr;
				break;
			case 81:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3s>>>*>(obj);
				obj = nullptr;
				break;
			case 82:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4s>>>*>(obj);
				obj = nullptr;
				break;
			case 83:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2w>>>*>(obj);
				obj = nullptr;
				break;
			case 84:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3w>>>*>(obj);
				obj = nullptr;
				break;
			case 85:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4w>>>*>(obj);
				obj = nullptr;
				break;
			case 86:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2i>>>*>(obj);
				obj = nullptr;
				break;
			case 87:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3i>>>*>(obj);
				obj = nullptr;
				break;
			case 88:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4i>>>*>(obj);
				obj = nullptr;
				break;
			case 89:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6i>>>*>(obj);
				obj = nullptr;
				break;
			case 90:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec8i>>>*>(obj);
				obj = nullptr;
				break;
			case 91:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2f>>>*>(obj);
				obj = nullptr;
				break;
			case 92:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3f>>>*>(obj);
				obj = nullptr;
				break;
			case 93:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4f>>>*>(obj);
				obj = nullptr;
				break;
			case 94:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6f>>>*>(obj);
				obj = nullptr;
				break;
			case 95:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2d>>>*>(obj);
				obj = nullptr;
				break;
			case 96:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3d>>>*>(obj);
				obj = nullptr;
				break;
			case 97:
				delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4d>>>*>(obj);
				obj = nullptr;
				break;
			case 98:
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
						} else {
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
						} else {
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
							*reinterpret_cast<std::shared_ptr<cv::Mat>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<cv::Mat>*>(src.obj);
						} else {
							dst.obj = new std::shared_ptr<cv::Mat>(*reinterpret_cast<std::shared_ptr<cv::Mat>*>(src.obj));
						}
					}
					if (dst.obj) {
						dst.reset(**reinterpret_cast<std::shared_ptr<cv::Mat>*>(dst.obj));
					}
					
				}
				break;
			case 4:
				if constexpr (!is_arrays_type && (std::is_same_v<Array, cv::_InputArray>)) {
					
					if (&src != &dst) {
						if (dst.obj) {
							*reinterpret_cast<std::shared_ptr<cv::Scalar>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<cv::Scalar>*>(src.obj);
						} else {
							dst.obj = new std::shared_ptr<cv::Scalar>(*reinterpret_cast<std::shared_ptr<cv::Scalar>*>(src.obj));
						}
					}
					if (dst.obj) {
						dst.reset(**reinterpret_cast<std::shared_ptr<cv::Scalar>*>(dst.obj));
					}
					
				}
				break;
			case 5:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Mat>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Mat>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Mat>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Mat>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Mat>>*>(dst.obj));
				}
				
				break;
			case 6:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::UMat>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::UMat>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::UMat>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::UMat>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::UMat>>*>(dst.obj));
				}
				
				break;
			case 7:
				if constexpr (std::is_same_v<Array, cv::_InputArray>) {
					
					if (&src != &dst) {
						if (dst.obj) {
							*reinterpret_cast<std::shared_ptr<std::vector<bool>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<bool>>*>(src.obj);
						} else {
							dst.obj = new std::shared_ptr<std::vector<bool>>(*reinterpret_cast<std::shared_ptr<std::vector<bool>>*>(src.obj));
						}
					}
					if (dst.obj) {
						dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<bool>>*>(dst.obj));
					}
					
				}
				break;
			case 8:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::RotatedRect>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::RotatedRect>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::RotatedRect>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::RotatedRect>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::RotatedRect>>*>(dst.obj));
				}
				
				break;
			case 9:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Range>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Range>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Range>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Range>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Range>>*>(dst.obj));
				}
				
				break;
			case 10:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Moments>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Moments>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Moments>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Moments>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Moments>>*>(dst.obj));
				}
				
				break;
			case 11:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<uchar>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<uchar>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<uchar>>(*reinterpret_cast<std::shared_ptr<std::vector<uchar>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<uchar>>*>(dst.obj));
				}
				
				break;
			case 12:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<schar>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<schar>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<schar>>(*reinterpret_cast<std::shared_ptr<std::vector<schar>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<schar>>*>(dst.obj));
				}
				
				break;
			case 13:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<char>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<char>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<char>>(*reinterpret_cast<std::shared_ptr<std::vector<char>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<char>>*>(dst.obj));
				}
				
				break;
			case 14:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<ushort>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<ushort>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<ushort>>(*reinterpret_cast<std::shared_ptr<std::vector<ushort>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<ushort>>*>(dst.obj));
				}
				
				break;
			case 15:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<short>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<short>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<short>>(*reinterpret_cast<std::shared_ptr<std::vector<short>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<short>>*>(dst.obj));
				}
				
				break;
			case 16:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<int>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<int>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<int>>(*reinterpret_cast<std::shared_ptr<std::vector<int>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<int>>*>(dst.obj));
				}
				
				break;
			case 17:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<float>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<float>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<float>>(*reinterpret_cast<std::shared_ptr<std::vector<float>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<float>>*>(dst.obj));
				}
				
				break;
			case 18:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<double>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<double>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<double>>(*reinterpret_cast<std::shared_ptr<std::vector<double>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<double>>*>(dst.obj));
				}
				
				break;
			case 19:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Point3i>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Point3i>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Point3i>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point3i>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Point3i>>*>(dst.obj));
				}
				
				break;
			case 20:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Point3f>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Point3f>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Point3f>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point3f>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Point3f>>*>(dst.obj));
				}
				
				break;
			case 21:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Point3d>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Point3d>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Point3d>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point3d>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Point3d>>*>(dst.obj));
				}
				
				break;
			case 22:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Point2f>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Point2f>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Point2f>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point2f>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Point2f>>*>(dst.obj));
				}
				
				break;
			case 23:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Point2d>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Point2d>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Point2d>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point2d>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Point2d>>*>(dst.obj));
				}
				
				break;
			case 24:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Point>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Point>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Point>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Point>>*>(dst.obj));
				}
				
				break;
			case 25:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2f>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2f>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Rect2f>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2f>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2f>>*>(dst.obj));
				}
				
				break;
			case 26:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2d>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2d>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Rect2d>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2d>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2d>>*>(dst.obj));
				}
				
				break;
			case 27:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Rect>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Rect>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Rect>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Rect>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Rect>>*>(dst.obj));
				}
				
				break;
			case 28:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Size2f>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Size2f>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Size2f>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Size2f>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Size2f>>*>(dst.obj));
				}
				
				break;
			case 29:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Size2d>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Size2d>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Size2d>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Size2d>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Size2d>>*>(dst.obj));
				}
				
				break;
			case 30:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Size>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Size>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Size>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Size>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Size>>*>(dst.obj));
				}
				
				break;
			case 31:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2b>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2b>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec2b>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2b>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2b>>*>(dst.obj));
				}
				
				break;
			case 32:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3b>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3b>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec3b>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3b>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3b>>*>(dst.obj));
				}
				
				break;
			case 33:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4b>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4b>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec4b>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4b>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4b>>*>(dst.obj));
				}
				
				break;
			case 34:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2s>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2s>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec2s>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2s>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2s>>*>(dst.obj));
				}
				
				break;
			case 35:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3s>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3s>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec3s>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3s>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3s>>*>(dst.obj));
				}
				
				break;
			case 36:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4s>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4s>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec4s>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4s>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4s>>*>(dst.obj));
				}
				
				break;
			case 37:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2w>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2w>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec2w>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2w>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2w>>*>(dst.obj));
				}
				
				break;
			case 38:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3w>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3w>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec3w>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3w>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3w>>*>(dst.obj));
				}
				
				break;
			case 39:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4w>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4w>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec4w>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4w>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4w>>*>(dst.obj));
				}
				
				break;
			case 40:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2i>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2i>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec2i>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2i>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2i>>*>(dst.obj));
				}
				
				break;
			case 41:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3i>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3i>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec3i>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3i>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3i>>*>(dst.obj));
				}
				
				break;
			case 42:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4i>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4i>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec4i>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4i>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4i>>*>(dst.obj));
				}
				
				break;
			case 43:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6i>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6i>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec6i>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6i>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6i>>*>(dst.obj));
				}
				
				break;
			case 44:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec8i>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec8i>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec8i>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec8i>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec8i>>*>(dst.obj));
				}
				
				break;
			case 45:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2f>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2f>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec2f>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2f>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2f>>*>(dst.obj));
				}
				
				break;
			case 46:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3f>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3f>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec3f>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3f>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3f>>*>(dst.obj));
				}
				
				break;
			case 47:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4f>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4f>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec4f>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4f>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4f>>*>(dst.obj));
				}
				
				break;
			case 48:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6f>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6f>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec6f>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6f>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6f>>*>(dst.obj));
				}
				
				break;
			case 49:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2d>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2d>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec2d>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2d>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2d>>*>(dst.obj));
				}
				
				break;
			case 50:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3d>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3d>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec3d>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3d>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3d>>*>(dst.obj));
				}
				
				break;
			case 51:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4d>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4d>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec4d>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4d>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4d>>*>(dst.obj));
				}
				
				break;
			case 52:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6d>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6d>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<cv::Vec6d>>(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6d>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6d>>*>(dst.obj));
				}
				
				break;
			case 53:
				if constexpr (std::is_same_v<Array, cv::_InputOutputArray>) {
					
					if (&src != &dst) {
						if (dst.obj) {
							*reinterpret_cast<std::shared_ptr<std::vector<std::vector<bool>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<bool>>>*>(src.obj);
						} else {
							dst.obj = new std::shared_ptr<std::vector<std::vector<bool>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<bool>>>*>(src.obj));
						}
					}
					if (dst.obj) {
						dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<bool>>>*>(dst.obj));
					}
					
				}
				break;
			case 54:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::RotatedRect>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::RotatedRect>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::RotatedRect>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::RotatedRect>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::RotatedRect>>>*>(dst.obj));
				}
				
				break;
			case 55:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Range>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Range>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Range>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Range>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Range>>>*>(dst.obj));
				}
				
				break;
			case 56:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Moments>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Moments>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Moments>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Moments>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Moments>>>*>(dst.obj));
				}
				
				break;
			case 57:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<uchar>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<uchar>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<uchar>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<uchar>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<uchar>>>*>(dst.obj));
				}
				
				break;
			case 58:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<schar>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<schar>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<schar>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<schar>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<schar>>>*>(dst.obj));
				}
				
				break;
			case 59:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<char>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<char>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<char>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<char>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<char>>>*>(dst.obj));
				}
				
				break;
			case 60:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<ushort>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<ushort>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<ushort>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<ushort>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<ushort>>>*>(dst.obj));
				}
				
				break;
			case 61:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<short>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<short>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<short>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<short>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<short>>>*>(dst.obj));
				}
				
				break;
			case 62:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<int>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<int>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<int>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<int>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<int>>>*>(dst.obj));
				}
				
				break;
			case 63:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<float>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<float>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<float>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<float>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<float>>>*>(dst.obj));
				}
				
				break;
			case 64:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<double>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<double>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<double>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<double>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<double>>>*>(dst.obj));
				}
				
				break;
			case 65:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3i>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3i>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Point3i>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3i>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3i>>>*>(dst.obj));
				}
				
				break;
			case 66:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3f>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3f>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Point3f>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3f>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3f>>>*>(dst.obj));
				}
				
				break;
			case 67:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3d>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3d>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Point3d>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3d>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3d>>>*>(dst.obj));
				}
				
				break;
			case 68:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2f>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2f>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Point2f>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2f>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2f>>>*>(dst.obj));
				}
				
				break;
			case 69:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2d>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2d>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Point2d>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2d>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2d>>>*>(dst.obj));
				}
				
				break;
			case 70:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Point>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point>>>*>(dst.obj));
				}
				
				break;
			case 71:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2f>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2f>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Rect2f>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2f>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2f>>>*>(dst.obj));
				}
				
				break;
			case 72:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2d>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2d>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Rect2d>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2d>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2d>>>*>(dst.obj));
				}
				
				break;
			case 73:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Rect>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect>>>*>(dst.obj));
				}
				
				break;
			case 74:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2f>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2f>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Size2f>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2f>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2f>>>*>(dst.obj));
				}
				
				break;
			case 75:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2d>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2d>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Size2d>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2d>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2d>>>*>(dst.obj));
				}
				
				break;
			case 76:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Size>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size>>>*>(dst.obj));
				}
				
				break;
			case 77:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2b>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2b>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2b>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2b>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2b>>>*>(dst.obj));
				}
				
				break;
			case 78:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3b>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3b>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3b>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3b>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3b>>>*>(dst.obj));
				}
				
				break;
			case 79:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4b>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4b>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4b>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4b>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4b>>>*>(dst.obj));
				}
				
				break;
			case 80:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2s>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2s>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2s>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2s>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2s>>>*>(dst.obj));
				}
				
				break;
			case 81:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3s>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3s>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3s>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3s>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3s>>>*>(dst.obj));
				}
				
				break;
			case 82:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4s>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4s>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4s>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4s>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4s>>>*>(dst.obj));
				}
				
				break;
			case 83:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2w>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2w>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2w>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2w>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2w>>>*>(dst.obj));
				}
				
				break;
			case 84:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3w>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3w>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3w>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3w>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3w>>>*>(dst.obj));
				}
				
				break;
			case 85:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4w>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4w>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4w>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4w>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4w>>>*>(dst.obj));
				}
				
				break;
			case 86:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2i>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2i>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2i>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2i>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2i>>>*>(dst.obj));
				}
				
				break;
			case 87:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3i>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3i>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3i>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3i>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3i>>>*>(dst.obj));
				}
				
				break;
			case 88:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4i>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4i>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4i>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4i>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4i>>>*>(dst.obj));
				}
				
				break;
			case 89:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6i>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6i>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec6i>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6i>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6i>>>*>(dst.obj));
				}
				
				break;
			case 90:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec8i>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec8i>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec8i>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec8i>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec8i>>>*>(dst.obj));
				}
				
				break;
			case 91:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2f>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2f>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2f>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2f>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2f>>>*>(dst.obj));
				}
				
				break;
			case 92:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3f>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3f>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3f>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3f>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3f>>>*>(dst.obj));
				}
				
				break;
			case 93:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4f>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4f>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4f>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4f>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4f>>>*>(dst.obj));
				}
				
				break;
			case 94:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6f>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6f>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec6f>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6f>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6f>>>*>(dst.obj));
				}
				
				break;
			case 95:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2d>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2d>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec2d>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2d>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2d>>>*>(dst.obj));
				}
				
				break;
			case 96:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3d>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3d>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec3d>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3d>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3d>>>*>(dst.obj));
				}
				
				break;
			case 97:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4d>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4d>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec4d>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4d>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4d>>>*>(dst.obj));
				}
				
				break;
			case 98:
				
				if (&src != &dst) {
					if (dst.obj) {
						*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6d>>>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6d>>>*>(src.obj);
					} else {
						dst.obj = new std::shared_ptr<std::vector<std::vector<cv::Vec6d>>>(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6d>>>*>(src.obj));
					}
				}
				if (dst.obj) {
					dst.reset(**reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6d>>>*>(dst.obj));
				}
				
				break;
			case 99:
				dst.reset(dst.vval);
				break;
			case 100:
				dst.reset(dst.mval);
				break;
			default:
				if (&src != &dst) {
					dst.ptr = src.ptr;
				}
			}
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

	// InputArray, outputArray, InputOutputArray
	template<typename Array, typename _To = sol::object>
	struct OptionalArray : public _OptionalArray<Array, _To, false> {};

	// InputArrayOfArrays, outputArrayOfArrays, InputOutputArrayOfArrays
	template<typename Array, typename _To = sol::object>
	struct OptionalArrays : public _OptionalArray<Array, _To, true> {};

	template<typename Array, typename _To = sol::object>
	inline decltype(auto) maybe_arrays(const _To& obj, Array*) {
		return OptionalArrays<Array>(obj);
	}

	template<typename Array>
	inline decltype(auto) maybe_arrays(const std::shared_ptr<Array>& ptr) {
		return OptionalArrays<Array>(ptr);
	}

	template<typename Array, typename _To = sol::object>
	inline decltype(auto) maybe_array(const _To& obj, Array*) {
		return OptionalArray<Array>(obj);
	}

	template<typename Array>
	inline decltype(auto) maybe_array(const std::shared_ptr<Array>& ptr) {
		return OptionalArray<Array>(ptr);
	}
}