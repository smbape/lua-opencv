#pragma once

#include <lua_utils.hpp>
#include <lua_generated_include.hpp>

namespace LUA_MODULE_NAME {
	void register_extension(sol::state_view& lua, sol::table& module);

	// cv::Ptr
	template<typename _To, typename _Tp>
	inline auto maybe_impl(_To& obj, cv::Ptr<_Tp>*) {
		auto wrapper = obj.template as<sol::optional<std::shared_ptr<_Tp>>>();
		if (!wrapper) {
			return sol::optional<cv::Ptr<_Tp>>();
		}

		return sol::optional<cv::Ptr<_Tp>>(sol::in_place, std::move(cv::Ptr<_Tp>(*wrapper)));
	}

	template<typename _To, typename _Tp>
	inline auto maybe_impl(const _To& obj, cv::Ptr<_Tp>* ptr) {
		return maybe_impl(const_cast<_To&>(obj), ptr);
	}

	template<typename _Tp>
	inline auto return_as_impl(cv::Ptr<_Tp>& ptr, sol::state_view& lua) {
		return std::shared_ptr<_Tp>(ptr);
	}

	template<typename _Tp>
	inline auto return_as_impl(const cv::Ptr<_Tp>& ptr, sol::state_view& lua) {
		return std::shared_ptr<_Tp>(ptr);
	}

	// std::vector<cv::Ptr>
	template<typename _To, typename _Tp>
	inline auto maybe_impl(_To& obj, std::vector<cv::Ptr<_Tp>>*) {
		auto vec = obj.template as<sol::optional<std::vector<std::shared_ptr<_Tp>>>>();
		if (!vec) {
			return sol::optional<std::vector<cv::Ptr<_Tp>>>();
		}

		sol::optional<std::vector<cv::Ptr<_Tp>>> res(sol::in_place, std::vector<cv::Ptr<_Tp>>());
		(*res).insert(std::end(*res), std::begin(*vec), std::end(*vec));
		return res;
	}

	template<typename _To, typename _Tp>
	inline auto maybe_impl(const _To& obj, std::vector<cv::Ptr<_Tp>>* ptr) {
		return maybe_impl(const_cast<_To&>(obj), ptr);
	}

	template<typename _Tp>
	inline auto return_as_impl(std::vector<cv::Ptr<_Tp>>& vec, sol::state_view& lua) {
		std::vector<std::shared_ptr<_Tp>> res;
		res.insert(std::end(res), std::begin(vec), std::end(vec));
		return res;
	}

	template<typename _Tp>
	inline auto return_as_impl(const std::vector<cv::Ptr<_Tp>>& vec, sol::state_view& lua) {
		std::vector<std::shared_ptr<_Tp>> res;
		res.insert(std::end(res), std::begin(vec), std::end(vec));
		return res;
	}

	// cv::Point_
	template<typename _To, typename _Tp>
	inline auto maybe_impl(_To& obj, cv::Point_<_Tp>*) {
		if (obj == sol::lua_nil) {
			sol::optional<cv::Point_<_Tp>> res(sol::in_place, std::move(cv::Point_<_Tp>()));
			return res;
		}

		auto maybe = obj.template as<sol::optional<cv::Point_<_Tp>>>();
		if (maybe) {
			return maybe;
		}

		auto vec = maybe_impl(obj, static_cast<std::vector<_Tp>*>(nullptr));
		if (!vec || (*vec).size() != 2) {
			return maybe;
		}

		sol::optional<cv::Point_<_Tp>> res(sol::in_place, std::move(cv::Point_<_Tp>()));

		if (obj.template is<std::vector<_Tp>>()) {
			(*res).x = (*vec)[0];
			(*res).y = (*vec)[1];
		}

		return res;
	}

	template<typename _To, typename _Tp>
	inline auto maybe_impl(const _To& obj, cv::Point_<_Tp>* ptr) {
		return maybe_impl(const_cast<_To&>(obj), ptr);
	}

	template<typename _Tp>
	inline auto return_as_impl(cv::Point_<_Tp>& obj, sol::state_view& lua) {
		sol::table res = lua.create_table();
		res[1] = obj.x;
		res[2] = obj.y;
		return res;
	}

	template<typename _Tp>
	inline auto return_as_impl(const cv::Point_<_Tp>& obj, sol::state_view& lua) {
		return return_as_impl(const_cast<cv::Point_<_Tp>&>(obj), lua);
	}

	template<typename _Tp>
	inline auto return_as_impl(std::vector<cv::Point_<_Tp>>& vec, sol::state_view& lua) {
		sol::table res = lua.create_table();
		int i = 0;
		for (const auto& obj : vec) {
			res[++i] = return_as_impl(obj, lua);
		}
		return res;
	}

	template<typename _Tp>
	inline auto return_as_impl(const std::vector<cv::Point_<_Tp>>& vec, sol::state_view& lua) {
		return return_as_impl(const_cast<std::vector<cv::Point_<_Tp>>&>(vec), lua);
	}

	template<typename _Tp>
	inline auto return_as_impl(std::vector<std::vector<cv::Point_<_Tp>>>& vec, sol::state_view& lua) {
		sol::table res = lua.create_table();
		int i = 0;
		for (const auto& obj : vec) {
			res[++i] = return_as_impl(obj, lua);
		}
		return res;
	}

	template<typename _Tp>
	inline auto return_as_impl(const std::vector<std::vector<cv::Point_<_Tp>>>& vec, sol::state_view& lua) {
		return return_as_impl(const_cast<std::vector<std::vector<cv::Point_<_Tp>>>&>(vec), lua);
	}

	template<typename _Tp>
	inline auto return_as_impl(std::vector<std::tuple<cv::Point_<_Tp>, double>>& vec, sol::state_view& lua) {
		sol::table res = lua.create_table();
		int i = 0;
		for (const auto& obj : vec) {
			sol::table elem = lua.create_table();
			elem[1] = return_as_impl(std::get<0>(obj), lua);
			elem[2] = std::get<1>(obj);
			res[++i] = elem;
		}
		return res;
	}

	template<typename _Tp>
	inline auto return_as_impl(const std::vector<std::tuple<cv::Point_<_Tp>, double>>& vec, sol::state_view& lua) {
		return return_as_impl(const_cast<std::vector<std::tuple<cv::Point_<_Tp>, double>>&>(vec), lua);
	}

	template<typename _Tp>
	inline auto return_as_impl(cv::Point3_<_Tp>& obj, sol::state_view& lua) {
		sol::table res = lua.create_table();
		res[1] = obj.x;
		res[2] = obj.y;
		res[3] = obj.z;
		return res;
	}

	template<typename _Tp>
	inline auto return_as_impl(const cv::Point3_<_Tp>& obj, sol::state_view& lua) {
		return return_as_impl(const_cast<cv::Point3_<_Tp>&>(obj), lua);
	}

	template<typename _Tp>
	inline auto return_as_impl(std::vector<cv::Point3_<_Tp>>& vec, sol::state_view& lua) {
		sol::table res = lua.create_table();
		int i = 0;
		for (const auto& obj : vec) {
			res[++i] = return_as_impl(obj, lua);
		}
		return res;
	}

	template<typename _Tp>
	inline auto return_as_impl(const std::vector<cv::Point3_<_Tp>>& vec, sol::state_view& lua) {
		return return_as_impl(const_cast<std::vector<cv::Point3_<_Tp>>&>(vec), lua);
	}

	template<typename _Tp>
	inline auto return_as_impl(std::vector<std::vector<cv::Point3_<_Tp>>>& vec, sol::state_view& lua) {
		sol::table res = lua.create_table();
		int i = 0;
		for (const auto& obj : vec) {
			res[++i] = return_as_impl(obj, lua);
		}
		return res;
	}

	template<typename _Tp>
	inline auto return_as_impl(const std::vector<std::vector<cv::Point3_<_Tp>>>& vec, sol::state_view& lua) {
		return return_as_impl(const_cast<std::vector<std::vector<cv::Point3_<_Tp>>>&>(vec), lua);
	}

	// cv::Rect_
	template<typename _To, typename _Tp>
	inline auto maybe_impl(_To& obj, cv::Rect_<_Tp>*) {
		if (obj == sol::lua_nil) {
			sol::optional<cv::Rect_<_Tp>> res(sol::in_place, std::move(cv::Rect_<_Tp>()));
			return res;
		}

		auto maybe = obj.template as<sol::optional<cv::Rect_<_Tp>>>();
		if (maybe) {
			return maybe;
		}

		auto vec = maybe_impl(obj, static_cast<std::vector<_Tp>*>(nullptr));
		if (!vec || (*vec).size() != 4) {
			return maybe;
		}

		sol::optional<cv::Rect_<_Tp>> res(sol::in_place, std::move(cv::Rect_<_Tp>()));

		(*res).x = (*vec)[0];
		(*res).y = (*vec)[1];
		(*res).width = (*vec)[2];
		(*res).height = (*vec)[3];

		return res;
	}

	template<typename _To, typename _Tp>
	inline auto maybe_impl(const _To& obj, cv::Rect_<_Tp>* ptr) {
		return maybe_impl(const_cast<_To&>(obj), ptr);
	}

	template<typename _Tp>
	inline auto return_as_impl(cv::Rect_<_Tp>& obj, sol::state_view& lua) {
		sol::table res = lua.create_table();
		res[1] = obj.x;
		res[2] = obj.y;
		res[3] = obj.width;
		res[4] = obj.height;
		return res;
	}

	template<typename _Tp>
	inline auto return_as_impl(const cv::Rect_<_Tp>& obj, sol::state_view& lua) {
		return return_as_impl(const_cast<cv::Rect_<_Tp>&>(obj), lua);
	}

	template<typename _Tp>
	inline auto return_as_impl(std::vector<cv::Rect_<_Tp>>& vec, sol::state_view& lua) {
		sol::table res = lua.create_table();
		int i = 0;
		for (const auto& obj : vec) {
			res[++i] = return_as_impl(obj, lua);
		}
		return res;
	}

	template<typename _Tp>
	inline auto return_as_impl(const std::vector<cv::Rect_<_Tp>>& vec, sol::state_view& lua) {
		return return_as_impl(const_cast<std::vector<cv::Rect_<_Tp>>&>(vec), lua);
	}

	// cv::Scalar_
	template<typename _To, typename _Tp>
	inline auto maybe_impl(_To& obj, cv::Scalar_<_Tp>*) {
		if (obj == sol::lua_nil) {
			sol::optional<cv::Scalar_<_Tp>> res(sol::in_place, std::move(cv::Scalar_<_Tp>()));
			return res;
		}

		auto maybe = obj.template as<sol::optional<cv::Scalar_<_Tp>>>();;
		if (maybe) {
			return maybe;
		}

		auto maybe_type = obj.template as<sol::optional<_Tp>>();
		if (maybe_type) {
			return sol::optional<cv::Scalar_<_Tp>>(sol::in_place, std::move(cv::Scalar_<_Tp>(*maybe_type)));
		}

		auto vec = maybe_impl(obj, static_cast<std::vector<_Tp>*>(nullptr));
		if (!vec || (*vec).size() > 4) {
			return maybe;
		}

		sol::optional<cv::Scalar_<_Tp>> res(sol::in_place, std::move(cv::Scalar_<_Tp>()));

		if (obj.template is<std::vector<_Tp>>()) {
			for (int i = 0; i < 4 && i < (*vec).size(); i++) {
				(*res)[i] = (*vec)[i];
			}
		}

		return res;
	}

	template<typename _To, typename _Tp>
	inline auto maybe_impl(const _To& obj, cv::Scalar_<_Tp>* ptr) {
		return maybe_impl(const_cast<_To&>(obj), ptr);
	}

	template<typename _Tp>
	inline auto return_as_impl(cv::Scalar_<_Tp>& obj, sol::state_view& lua) {
		sol::table res = lua.create_table();
		for (int i = 0; i < 4; i++) {
			res[i + 1] = obj[i];
		}
		return res;
	}

	template<typename _Tp>
	inline auto return_as_impl(const cv::Scalar_<_Tp>& obj, sol::state_view& lua) {
		return return_as_impl(const_cast<cv::Scalar_<_Tp>&>(obj), lua);
	}

	// cv::Size_
	template<typename _To, typename _Tp>
	inline auto maybe_impl(_To& obj, cv::Size_<_Tp>*) {
		if (obj == sol::lua_nil) {
			sol::optional<cv::Size_<_Tp>> res(sol::in_place, std::move(cv::Size_<_Tp>()));
			return res;
		}

		auto maybe = obj.template as<sol::optional<cv::Size_<_Tp>>>();
		if (maybe) {
			return maybe;
		}

		auto vec = maybe_impl(obj, static_cast<std::vector<_Tp>*>(nullptr));
		if (!vec || (*vec).size() != 2) {
			return maybe;
		}

		sol::optional<cv::Size_<_Tp>> res(sol::in_place, std::move(cv::Size_<_Tp>()));

		(*res).width = (*vec)[0];
		(*res).height = (*vec)[1];

		return res;
	}

	template<typename _To, typename _Tp>
	inline auto maybe_impl(const _To& obj, cv::Size_<_Tp>* ptr) {
		return maybe_impl(const_cast<_To&>(obj), ptr);
	}

	template<typename _Tp>
	inline auto return_as_impl(cv::Size_<_Tp>& obj, sol::state_view& lua) {
		sol::table res = lua.create_table();
		res[1] = obj.width;
		res[2] = obj.height;
		return res;
	}

	template<typename _Tp>
	inline auto return_as_impl(const cv::Size_<_Tp>& obj, sol::state_view& lua) {
		return return_as_impl(const_cast<cv::Size_<_Tp>&>(obj), lua);
	}

	template<typename _Tp>
	inline auto return_as_impl(std::vector<cv::Size_<_Tp>>& vec, sol::state_view& lua) {
		sol::table res = lua.create_table();
		int i = 0;
		for (const auto& obj : vec) {
			res[++i] = return_as_impl(obj, lua);
		}
		return res;
	}

	template<typename _Tp>
	inline auto return_as_impl(const std::vector<cv::Size_<_Tp>>& vec, sol::state_view& lua) {
		return return_as_impl(const_cast<std::vector<cv::Size_<_Tp>>&>(vec), lua);
	}

	// cv::Vec
	template<typename _To, typename _Tp, int cn>
	inline auto maybe_impl(_To& obj, cv::Vec<_Tp, cn>*) {
		if (obj == sol::lua_nil) {
			sol::optional<cv::Vec<_Tp, cn>> res(sol::in_place, std::move(cv::Vec<_Tp, cn>()));
			return res;
		}

		auto maybe = obj.template as<sol::optional<cv::Vec<_Tp, cn>>>();;
		if (maybe) {
			return maybe;
		}

		auto vec = maybe_impl(obj, static_cast<std::vector<_Tp>*>(nullptr));
		if (!vec || (*vec).size() != cn) {
			return maybe;
		}

		sol::optional<cv::Vec<_Tp, cn>> res(sol::in_place, std::move(cv::Vec<_Tp, cn>()));

		if (obj.template is<std::vector<_Tp>>()) {
			for (int i = 0; i < cn; i++) {
				(*res)[i] = (*vec)[i];
			}
		}

		return res;
	}

	template<typename _To, typename _Tp, int cn>
	inline auto maybe_impl(const _To& obj, cv::Vec<_Tp, cn>* ptr) {
		return maybe_impl(const_cast<_To&>(obj), ptr);
	}

	template<typename _Tp, int cn>
	inline auto return_as_impl(cv::Vec<_Tp, cn>& obj, sol::state_view& lua) {
		sol::table res = lua.create_table();
		for (int i = 0; i < cn; i++) {
			res[i + 1] = obj[i];
		}
		return res;
	}

	template<typename _Tp, int cn>
	inline auto return_as_impl(const cv::Vec<_Tp, cn>& obj, sol::state_view& lua) {
		return return_as_impl(const_cast<cv::Vec<_Tp, cn>&>(obj), lua);
	}

	template<typename _Tp, int cn>
	inline auto return_as_impl(std::vector<cv::Vec<_Tp, cn>>& vec, sol::state_view& lua) {
		sol::table res = lua.create_table();
		int i = 0;
		for (const auto& obj : vec) {
			res[++i] = return_as_impl(obj, lua);
		}
		return res;
	}

	template<typename _Tp, int cn>
	inline auto return_as_impl(const std::vector<cv::Vec<_Tp, cn>>& vec, sol::state_view& lua) {
		return return_as_impl(const_cast<std::vector<cv::Vec<_Tp, cn>>&>(vec), lua);
	}

	// InputArray, outputArray, InputOutputArray
	template<typename Array, typename _To = sol::object>
	struct OptionalArray
	{
		OptionalArray(_To& obj) {
			if (obj == sol::lua_nil) {
				setField(*this, *this, 3);
				return;
			}

			auto maybe_Array = maybe_impl(obj, static_cast<Array*>(nullptr));
			if (maybe_Array) {
				ptr = reference_internal(*maybe_Array);
				return;
			}

			auto maybe_GpuMat = maybe_impl(obj, static_cast<cv::cuda::GpuMat*>(nullptr));
			if (maybe_GpuMat) {
				reset(*maybe_GpuMat);
				return;
			}

			auto maybe_Mat = maybe_impl(obj, static_cast<cv::Mat*>(nullptr));
			if (maybe_Mat) {
				reset(*maybe_Mat);
				return;
			}

			auto maybe_UMat = maybe_impl(obj, static_cast<cv::UMat*>(nullptr));
			if (maybe_UMat) {
				reset(*maybe_UMat);
				return;
			}

			auto maybe_Scalar = maybe_impl(obj, static_cast<cv::Scalar*>(nullptr));
			if (maybe_Scalar) {
				sval = *maybe_Scalar;
				setField(*this, *this, 1);
				return;
			}

			if constexpr (std::is_same_v<Array, cv::_InputArray>) {
				auto maybe_double = maybe_impl(obj, static_cast<double*>(nullptr));
				if (maybe_double) {
					dval = *maybe_double;
					setField(*this, *this, 2);
					return;
				}
			}
		}

		OptionalArray(const _To& obj) : OptionalArray(const_cast<_To&>(obj)) {}

		OptionalArray(const std::shared_ptr<Array>& ptr) : ptr(ptr) {}

		OptionalArray(const OptionalArray& other) {
			setField(other, *this, other.field);
		}

		OptionalArray() = default;

		template<typename T>
		inline void reset(T& obj) {
			ptr = std::make_shared<Array>(obj);
		}

		template<typename T>
		inline void reset(const T& obj) {
			ptr = std::make_shared<Array>(obj);
		}

		static void setField(const OptionalArray& src, OptionalArray& dst, std::uint8_t _field) {
			dst.field = _field;

			switch (_field) {
			case 1:
				if (&src != &dst) {
					dst.sval = src.sval;
				}
				dst.reset(dst.sval);
				break;
			case 2:
				if constexpr (std::is_same_v<Array, cv::_InputArray>) {
					if (&src != &dst) {
						dst.dval = src.dval;
					}
					dst.reset(dst.dval);
				}
				break;
			case 3:
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

		cv::Scalar sval;
		double dval;
		cv::Mat mval;
	};

	template<typename Array, typename _To = sol::object>
	inline decltype(auto) maybe_array(const _To& obj, Array*) {
		return OptionalArray<Array>(obj);
	}

	template<typename Array>
	inline decltype(auto) maybe_array(const std::shared_ptr<Array>& ptr) {
		return OptionalArray<Array>(ptr);
	}

	// InputArrayOfArrays, outputArrayOfArrays, InputOutputArrayOfArrays
	template<typename Array, typename _To = sol::object>
	struct OptionalArrays
	{
		OptionalArrays(_To& obj) {
			if (obj == sol::lua_nil) {
				setField(*this, *this, 1);
				return;
			}

			auto maybe_Array = maybe_impl(obj, static_cast<Array*>(nullptr));
			if (maybe_Array) {
				ptr = reference_internal(*maybe_Array);
				return;
			}

			auto maybe_Mat = maybe_impl(obj, static_cast<std::vector<cv::Mat>*>(nullptr));
			if (maybe_Mat) {
				val1 = *maybe_Mat;
				setField(*this, *this, 1);
				return;
			}

			auto maybe_UMat = maybe_impl(obj, static_cast<std::vector<cv::UMat>*>(nullptr));
			if (maybe_UMat) {
				val2 = *maybe_UMat;
				setField(*this, *this, 2);
				return;
			}

			auto maybe_RotatedRect = maybe_impl(obj, static_cast<std::vector<cv::RotatedRect>*>(nullptr));
			if (maybe_RotatedRect) {
				val3 = *maybe_RotatedRect;
				setField(*this, *this, 3);
				return;
			}

			auto maybe_char = maybe_impl(obj, static_cast<std::vector<char>*>(nullptr));
			if (maybe_char) {
				val4 = *maybe_char;
				setField(*this, *this, 4);
				return;
			}

			auto maybe_uchar = maybe_impl(obj, static_cast<std::vector<uchar>*>(nullptr));
			if (maybe_uchar) {
				val5 = *maybe_uchar;
				setField(*this, *this, 5);
				return;
			}

			auto maybe_int = maybe_impl(obj, static_cast<std::vector<int>*>(nullptr));
			if (maybe_int) {
				val6 = *maybe_int;
				setField(*this, *this, 6);
				return;
			}

			auto maybe_float = maybe_impl(obj, static_cast<std::vector<float>*>(nullptr));
			if (maybe_float) {
				val7 = *maybe_float;
				setField(*this, *this, 7);
				return;
			}

			auto maybe_double = maybe_impl(obj, static_cast<std::vector<double>*>(nullptr));
			if (maybe_double) {
				val8 = *maybe_double;
				setField(*this, *this, 8);
				return;
			}

			auto maybe_Point = maybe_impl(obj, static_cast<std::vector<cv::Point>*>(nullptr));
			if (maybe_Point) {
				val9 = *maybe_Point;
				setField(*this, *this, 9);
				return;
			}

			auto maybe_Point2f = maybe_impl(obj, static_cast<std::vector<cv::Point2f>*>(nullptr));
			if (maybe_Point2f) {
				val10 = *maybe_Point2f;
				setField(*this, *this, 10);
				return;
			}

			auto maybe_Rect = maybe_impl(obj, static_cast<std::vector<cv::Rect>*>(nullptr));
			if (maybe_Rect) {
				val11 = *maybe_Rect;
				setField(*this, *this, 11);
				return;
			}

			auto maybe_Size = maybe_impl(obj, static_cast<std::vector<cv::Size>*>(nullptr));
			if (maybe_Size) {
				val12 = *maybe_Size;
				setField(*this, *this, 12);
				return;
			}

			auto maybe_Vec6f = maybe_impl(obj, static_cast<std::vector<cv::Vec6f>*>(nullptr));
			if (maybe_Vec6f) {
				val13 = *maybe_Vec6f;
				setField(*this, *this, 13);
				return;
			}

			auto maybe_VectorOfCchar = maybe_impl(obj, static_cast<std::vector<std::vector<char>>*>(nullptr));
			if (maybe_VectorOfCchar) {
				val14 = *maybe_VectorOfCchar;
				setField(*this, *this, 14);
				return;
			}

			auto maybe_VectorOfIint = maybe_impl(obj, static_cast<std::vector<std::vector<int>>*>(nullptr));
			if (maybe_VectorOfIint) {
				val15 = *maybe_VectorOfIint;
				setField(*this, *this, 15);
				return;
			}

			auto maybe_VectorOfPPoint = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Point>>*>(nullptr));
			if (maybe_VectorOfPPoint) {
				val16 = *maybe_VectorOfPPoint;
				setField(*this, *this, 16);
				return;
			}

			auto maybe_VectorOfPPoint2f = maybe_impl(obj, static_cast<std::vector<std::vector<cv::Point2f>>*>(nullptr));
			if (maybe_VectorOfPPoint2f) {
				val17 = *maybe_VectorOfPPoint2f;
				setField(*this, *this, 17);
				return;
			}
		}

		OptionalArrays(const _To& obj) : OptionalArrays(const_cast<_To&>(obj)) {}

		OptionalArrays(const std::shared_ptr<Array>& ptr) : ptr(ptr) {}

		OptionalArrays(const OptionalArrays& other) {
			other.setField(other, *this, other.field);
		}

		OptionalArrays() = default;

		template<typename T>
		inline void reset(T& obj) {
			ptr = std::make_shared<Array>(obj);
		}

		template<typename T>
		inline void reset(const T& obj) {
			ptr = std::make_shared<Array>(obj);
		}

		static void setField(const OptionalArrays& src, OptionalArrays& dst, std::uint8_t _field) {
			dst.field = _field;

			switch (_field) {
			case 1:
				if (&src != &dst) {
					dst.val1 = src.val1;
				}
				dst.reset(dst.val1);
				break;
			case 2:
				if (&src != &dst) {
					dst.val2 = src.val2;
				}
				dst.reset(dst.val2);
				break;
			case 3:
				if (&src != &dst) {
					dst.val3 = src.val3;
				}
				dst.reset(dst.val3);
				break;
			case 4:
				if (&src != &dst) {
					dst.val4 = src.val4;
				}
				dst.reset(dst.val4);
				break;
			case 5:
				if (&src != &dst) {
					dst.val5 = src.val5;
				}
				dst.reset(dst.val5);
				break;
			case 6:
				if (&src != &dst) {
					dst.val6 = src.val6;
				}
				dst.reset(dst.val6);
				break;
			case 7:
				if (&src != &dst) {
					dst.val7 = src.val7;
				}
				dst.reset(dst.val7);
				break;
			case 8:
				if (&src != &dst) {
					dst.val8 = src.val8;
				}
				dst.reset(dst.val8);
				break;
			case 9:
				if (&src != &dst) {
					dst.val9 = src.val9;
				}
				dst.reset(dst.val9);
				break;
			case 10:
				if (&src != &dst) {
					dst.val10 = src.val10;
				}
				dst.reset(dst.val10);
				break;
			case 11:
				if (&src != &dst) {
					dst.val11 = src.val11;
				}
				dst.reset(dst.val11);
				break;
			case 12:
				if (&src != &dst) {
					dst.val12 = src.val12;
				}
				dst.reset(dst.val12);
				break;
			case 13:
				if (&src != &dst) {
					dst.val13 = src.val13;
				}
				dst.reset(dst.val13);
				break;
			case 14:
				if (&src != &dst) {
					dst.val14 = src.val14;
				}
				dst.reset(dst.val14);
				break;
			case 15:
				if (&src != &dst) {
					dst.val15 = src.val15;
				}
				dst.reset(dst.val15);
				break;
			case 16:
				if (&src != &dst) {
					dst.val16 = src.val16;
				}
				dst.reset(dst.val16);
				break;
			case 17:
				if (&src != &dst) {
					dst.val17 = src.val17;
				}
				dst.reset(dst.val17);
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

		std::vector<cv::Mat> val1;
		std::vector<cv::UMat> val2;
		std::vector<cv::RotatedRect> val3;
		std::vector<char> val4;
		std::vector<uchar> val5;
		std::vector<int> val6;
		std::vector<float> val7;
		std::vector<double> val8;
		std::vector<cv::Point> val9;
		std::vector<cv::Point2f> val10;
		std::vector<cv::Rect> val11;
		std::vector<cv::Size> val12;
		std::vector<cv::Vec6f> val13;
		std::vector<std::vector<char>> val14;
		std::vector<std::vector<int>> val15;
		std::vector<std::vector<cv::Point>> val16;
		std::vector<std::vector<cv::Point2f>> val17;
	};

	template<typename Array, typename _To = sol::object>
	inline decltype(auto) maybe_arrays(const _To& obj, Array*) {
		return OptionalArrays<Array>(obj);
	}

	template<typename Array>
	inline decltype(auto) maybe_arrays(const std::shared_ptr<Array>& ptr) {
		return OptionalArrays<Array>(ptr);
	}
}

namespace cv {
	inline bool operator== (const UMat& a, const UMat& b) {
		return &a == &b;
	}

	inline bool operator== (const MediaFrame& a, const MediaFrame& b) {
		return &a == &b;
	}

	inline bool operator== (const RMat& a, const RMat& b) {
		return &a == &b;
	}

	inline bool operator== (const DMatch& a, const DMatch& b) {
		return &a == &b;
	}

	inline bool operator== (const KeyPoint& a, const KeyPoint& b) {
		return &a == &b;
	}

	namespace detail {
		inline bool operator== (const VectorRef& a, const VectorRef& b) {
			return &a == &b;
		}

		inline bool operator== (const OpaqueRef& a, const OpaqueRef& b) {
			return &a == &b;
		}
	}

	namespace gapi::wip::draw {
		inline bool operator== (const Text& a, const Text& b) {
			return &a == &b;
		}

		inline bool operator== (const FText& a, const FText& b) {
			return &a == &b;
		}

		inline bool operator== (const Rect& a, const Rect& b) {
			return &a == &b;
		}

		inline bool operator== (const Circle& a, const Circle& b) {
			return &a == &b;
		}

		inline bool operator== (const Line& a, const Line& b) {
			return &a == &b;
		}

		inline bool operator== (const Mosaic& a, const Mosaic& b) {
			return &a == &b;
		}

		inline bool operator== (const Image& a, const Image& b) {
			return &a == &b;
		}

		inline bool operator== (const Poly& a, const Poly& b) {
			return &a == &b;
		}
	}

#ifdef __OPENCV_DESCRIPTOR_HPP__
	namespace line_descriptor {
		inline bool operator== (const KeyLine& a, const KeyLine& b) {
			return &a == &b;
		}
	}
#endif
}

namespace sol {
	template <>
	struct is_container<cv::FileNode> : std::false_type { };

	template <>
	struct is_to_stringable<cv::GScalarDesc> : std::false_type { };

	template <>
	struct is_to_stringable<cv::GArrayDesc> : std::false_type { };

	template <>
	struct is_to_stringable<cv::GOpaqueDesc> : std::false_type { };
} // namespace sol
