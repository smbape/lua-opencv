#pragma once

#include <lua_utils.hpp>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/gapi/garray.hpp>
#include <opencv2/gapi/gopaque.hpp>
#include <opencv2/gapi/gscalar.hpp>
#include <opencv2/gapi/media.hpp>
#include <opencv2/gapi/rmat.hpp>

namespace LUA_MODULE_NAME {
	// cv::Ptr
	template<typename _To, typename _Tp>
	bool object_is_impl(const _To& obj, cv::Ptr<_Tp>*) {
		return obj.template is<std::shared_ptr<_Tp>>();
	}

	template<typename _To, typename _Tp>
	auto object_as_impl(const _To& obj, cv::Ptr<_Tp>*) {
		cv::Ptr<_Tp> res = obj.template as<std::shared_ptr<_Tp>>();
		return res;
	}

	template<typename _To, typename _Tp>
	auto object_as_impl(_To& obj, cv::Ptr<_Tp>*) {
		cv::Ptr<_Tp> res = obj.template as<std::shared_ptr<_Tp>>();
		return res;
	}

	template<typename _Tp>
	auto return_cast_impl(const cv::Ptr<_Tp>& ptr) {
		return std::shared_ptr<_Tp>(ptr);
	}

	template<typename _Tp>
	auto return_cast_impl(cv::Ptr<_Tp>& ptr) {
		return std::shared_ptr<_Tp>(ptr);
	}

	// std::vector<cv::Ptr>
	template<typename _To, typename _Tp>
	bool object_is_impl(const _To& obj, std::vector<cv::Ptr<_Tp>>*) {
		return obj.template is<std::vector<std::shared_ptr<_Tp>>>();
	}

	template<typename _To, typename _Tp>
	auto object_as_impl(const _To& obj, std::vector<cv::Ptr<_Tp>>*) {
		std::vector<cv::Ptr<_Tp>> res;
		const auto& vec = obj.template as<std::vector<std::shared_ptr<_Tp>>>();
		res.insert(std::end(res), std::begin(vec), std::end(vec));
		return res;
	}

	template<typename _To, typename _Tp>
	auto object_as_impl(_To& obj, std::vector<cv::Ptr<_Tp>>*) {
		std::vector<cv::Ptr<_Tp>> res;
		const auto& vec = obj.template as<std::vector<std::shared_ptr<_Tp>>>();
		res.insert(std::end(res), std::begin(vec), std::end(vec));
		return res;
	}

	template<typename _Tp>
	auto return_cast_impl(const std::vector<cv::Ptr<_Tp>>& vec) {
		std::vector<std::shared_ptr<_Tp>> res;
		res.insert(std::end(res), std::begin(vec), std::end(vec));
		return res;
	}

	template<typename _Tp>
	auto return_cast_impl(std::vector<cv::Ptr<_Tp>>& vec) {
		std::vector<std::shared_ptr<_Tp>> res;
		res.insert(std::end(res), std::begin(vec), std::end(vec));
		return res;
	}

	// cv::Point_
	template<typename _To, typename _Tp>
	bool object_is_impl(const _To& obj, cv::Point_<_Tp>*) {
		if (obj.template is<cv::Point_<_Tp>>()) {
			return true;
		}

		if (obj.template is<std::vector<_Tp>>()) {
			const auto size = obj.template as<std::vector<_Tp>>().size();
			return size > 0 && size <= 2;
		}

		return false;
	}

	template<typename _To, typename _Tp>
	auto object_as_impl(const _To& obj, cv::Point_<_Tp>*) {
		if (obj.template is<cv::Point_<_Tp>>()) {
			return obj.template as<cv::Point_<_Tp>>();
		}

		cv::Point_<_Tp> res;

		if (obj.template is<std::vector<_Tp>>()) {
			const auto& vec = obj.template as<std::vector<_Tp>>();
			res.x = vec[0];
			res.y = vec[1];
		}

		return res;
	}

	template<typename _To, typename _Tp>
	auto object_as_impl(_To& obj, cv::Point_<_Tp>*) {
		if (obj.template is<cv::Point_<_Tp>>()) {
			return obj.template as<cv::Point_<_Tp>>();
		}

		cv::Point_<_Tp> res;

		if (obj.template is<std::vector<_Tp>>()) {
			const auto& vec = obj.template as<std::vector<_Tp>>();
			res.x = vec[0];
			res.y = vec[1];
		}

		return res;
	}

	// cv::Rect_
	template<typename _To, typename _Tp>
	bool object_is_impl(const _To& obj, cv::Rect_<_Tp>*) {
		if (obj.template is<cv::Rect_<_Tp>>()) {
			return true;
		}

		if (obj.template is<std::vector<_Tp>>()) {
			const auto size = obj.template as<std::vector<_Tp>>().size();
			return size > 0 && size <= 4;
		}

		return false;
	}

	template<typename _To, typename _Tp>
	auto object_as_impl(const _To& obj, cv::Rect_<_Tp>*) {
		if (obj.template is<cv::Rect_<_Tp>>()) {
			return obj.template as<cv::Rect_<_Tp>>();
		}

		cv::Rect_<_Tp> res;

		if (obj.template is<std::vector<_Tp>>()) {
			const auto& vec = obj.template as<std::vector<_Tp>>();
			res.x = vec[0];
			res.y = vec[1];
			res.width = vec[2];
			res.height = vec[3];
		}

		return res;
	}

	template<typename _To, typename _Tp>
	auto object_as_impl(_To& obj, cv::Rect_<_Tp>*) {
		if (obj.template is<cv::Rect_<_Tp>>()) {
			return obj.template as<cv::Rect_<_Tp>>();
		}

		cv::Rect_<_Tp> res;

		if (obj.template is<std::vector<_Tp>>()) {
			const auto& vec = obj.template as<std::vector<_Tp>>();
			res.x = vec[0];
			res.y = vec[1];
			res.width = vec[2];
			res.height = vec[3];
		}

		return res;
	}

	// cv::Scalar_
	template<typename _To, typename _Tp>
	bool object_is_impl(const _To& obj, cv::Scalar_<_Tp>*) {
		if (obj.template is<cv::Scalar_<_Tp>>() || obj.template is<_Tp>()) {
			return true;
		}

		if (obj.template is<std::vector<_Tp>>()) {
			const auto size = obj.template as<std::vector<_Tp>>().size();
			return size > 0 && size <= 4;
		}

		return false;
	}

	template<typename _To, typename _Tp>
	auto object_as_impl(const _To& obj, cv::Scalar_<_Tp>*) {
		if (obj.template is<cv::Scalar_<_Tp>>()) {
			return obj.template as<cv::Scalar_<_Tp>>();
		}

		if (obj.template is<_Tp>()) {
			return cv::Scalar_<_Tp>(obj.template as<_Tp>());
		}

		cv::Scalar_<_Tp> res;

		if (obj.template is<std::vector<_Tp>>()) {
			const auto& vec = obj.template as<std::vector<_Tp>>();
			for (int i = 0; i < 4 && i < vec.size(); i++) {
				res[i] = vec[i];
			}
		}

		return res;
	}

	template<typename _To, typename _Tp>
	auto object_as_impl(_To& obj, cv::Scalar_<_Tp>*) {
		if (obj.template is<cv::Scalar_<_Tp>>()) {
			return obj.template as<cv::Scalar_<_Tp>>();
		}

		if (obj.template is<_Tp>()) {
			return cv::Scalar_<_Tp>(obj.template as<_Tp>());
		}

		cv::Scalar_<_Tp> res;

		if (obj.template is<std::vector<_Tp>>()) {
			const auto& vec = obj.template as<std::vector<_Tp>>();
			for (int i = 0; i < 4 && i < vec.size(); i++) {
				res[i] = vec[i];
			}
		}

		return res;
	}

	// cv::Size_
	template<typename _To, typename _Tp>
	bool object_is_impl(const _To& obj, cv::Size_<_Tp>*) {
		if (obj.template is<cv::Size_<_Tp>>()) {
			return true;
		}

		if (obj.template is<std::vector<_Tp>>()) {
			const auto size = obj.template as<std::vector<_Tp>>().size();
			return size > 0 && size <= 2;
		}

		return false;
	}

	template<typename _To, typename _Tp>
	auto object_as_impl(const _To& obj, cv::Size_<_Tp>*) {
		if (obj.template is<cv::Size_<_Tp>>()) {
			return obj.template as<cv::Size_<_Tp>>();
		}

		cv::Size_<_Tp> res;

		if (obj.template is<std::vector<_Tp>>()) {
			const auto& vec = obj.template as<std::vector<_Tp>>();
			res.width = vec[0];
			res.height = vec[1];
		}

		return res;
	}

	template<typename _To, typename _Tp>
	auto object_as_impl(_To& obj, cv::Size_<_Tp>*) {
		if (obj.template is<cv::Size_<_Tp>>()) {
			return obj.template as<cv::Size_<_Tp>>();
		}

		cv::Size_<_Tp> res;

		if (obj.template is<std::vector<_Tp>>()) {
			const auto& vec = obj.template as<std::vector<_Tp>>();
			res.width = vec[0];
			res.height = vec[1];
		}

		return res;
	}

	// cv::Vec
	template<typename _To, typename _Tp, int cn>
	bool object_is_impl(const _To& obj, cv::Vec<_Tp, cn>*) {
		if (obj.template is<cv::Vec<_Tp, cn>>() || obj.template is<_Tp>()) {
			return true;
		}

		if (obj.template is<std::vector<_Tp>>()) {
			const auto size = obj.template as<std::vector<_Tp>>().size();
			return size > 0 && size <= cn;
		}

		return false;
	}

	template<typename _To, typename _Tp, int cn>
	auto object_as_impl(const _To& obj, cv::Vec<_Tp, cn>*) {
		if (obj.template is<cv::Vec<_Tp, cn>>()) {
			return obj.template as<cv::Vec<_Tp, cn>>();
		}

		if (obj.template is<_Tp>()) {
			return cv::Vec<_Tp, cn>(obj.template as<_Tp>());
		}

		cv::Vec<_Tp, cn> res;

		if (obj.template is<std::vector<_Tp>>()) {
			const auto& vec = obj.template as<std::vector<_Tp>>();
			for (int i = 0; i < cn && i < vec.size(); i++) {
				res[i] = vec[i];
			}
		}

		return res;
	}

	template<typename _To, typename _Tp, int cn>
	auto object_as_impl(_To& obj, cv::Vec<_Tp, cn>*) {
		if (obj.template is<cv::Vec<_Tp, cn>>()) {
			return obj.template as<cv::Vec<_Tp, cn>>();
		}

		if (obj.template is<_Tp>()) {
			return cv::Vec<_Tp, cn>(obj.template as<_Tp>());
		}

		cv::Vec<_Tp, cn> res;

		if (obj.template is<std::vector<_Tp>>()) {
			const auto& vec = obj.template as<std::vector<_Tp>>();
			for (int i = 0; i < cn && i < vec.size(); i++) {
				res[i] = vec[i];
			}
		}

		return res;
	}

	template<typename Array, typename _To = sol::object>
	bool object_is_array(const _To& obj, Array*) {
		return object_is_impl(obj, static_cast<Array*>(nullptr))
			|| object_is_impl(obj, static_cast<cv::cuda::GpuMat*>(nullptr))
			|| object_is_impl(obj, static_cast<cv::Mat*>(nullptr))
			|| object_is_impl(obj, static_cast<cv::UMat*>(nullptr))
			|| object_is_impl(obj, static_cast<cv::Scalar*>(nullptr))
			|| std::is_same<Array, cv::_InputArray>::value&& object_is_impl(obj, static_cast<double*>(nullptr))
			;
	}

	template<typename Array, typename _To = sol::object>
	struct ArraySharedPtr
	{
		ArraySharedPtr(const _To& obj) {
			if (object_is_impl(obj, static_cast<Array*>(nullptr))) {
				const Array& input = object_as_impl(obj, static_cast<Array*>(nullptr));
				ptr = reference_internal(input);
				return;
			}

			if (object_is_impl(obj, static_cast<cv::cuda::GpuMat*>(nullptr))) {
				auto& input = object_as_impl(obj, static_cast<cv::cuda::GpuMat*>(nullptr));
				ptr = std::make_shared<Array>(input);
				return;
			}

			if (object_is_impl(obj, static_cast<cv::Mat*>(nullptr))) {
				auto& input = object_as_impl(obj, static_cast<cv::Mat*>(nullptr));
				ptr = std::make_shared<Array>(input);
				return;
			}

			if (object_is_impl(obj, static_cast<cv::UMat*>(nullptr))) {
				auto& input = object_as_impl(obj, static_cast<cv::UMat*>(nullptr));
				ptr = std::make_shared<Array>(input);
				return;
			}

			if (object_is_impl(obj, static_cast<cv::Scalar*>(nullptr))) {
				sval = object_as_impl(obj, static_cast<cv::Scalar*>(nullptr));
				ptr = std::make_shared<Array>(sval);
				return;
			}

			if constexpr (std::is_same_v<Array, cv::_InputArray>) {
				if (object_is_impl(obj, static_cast<double*>(nullptr))) {
					dval = object_as_impl(obj, static_cast<double*>(nullptr));
					ptr = std::make_shared<Array>(dval);
					return;
				}
			}
		}

		ArraySharedPtr(const std::shared_ptr<Array>& ptr) : ptr(ptr) {}

		auto& operator*() {
			return *ptr;
		}

		std::shared_ptr<Array> ptr;
		cv::Scalar sval;
		double dval;
	};

	template<typename Array, typename _To = sol::object>
	decltype(auto) object_as_array(const _To& obj, Array*) {
		return ArraySharedPtr<Array>(obj);
	}

	template<typename Array>
	decltype(auto) object_as_array(const std::shared_ptr<Array>& ptr) {
		return ArraySharedPtr<Array>(ptr);
	}

	template<typename Array, typename _To = sol::object>
	bool object_is_arrays(const _To& obj, Array*) {
		return object_is_impl(obj, static_cast<Array*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<cv::cuda::GpuMat>*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<cv::Mat>*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<cv::UMat>*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<cv::RotatedRect>*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<char>*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<uchar>*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<int>*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<float>*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<double>*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<cv::Point>*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<cv::Point2f>*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<cv::Rect>*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<cv::Size>*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<cv::Vec6f>*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<std::vector<char>>*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<std::vector<int>>*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<std::vector<cv::Point>>*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<std::vector<cv::Point2f>>*>(nullptr))
			;
	}

	template<typename Array, typename _To = sol::object>
	struct ArraysSharedPtr
	{
		ArraysSharedPtr(const _To& obj) {
			if (object_is_impl(obj, static_cast<Array*>(nullptr))) {
				const Array& input = object_as_impl(obj, static_cast<Array*>(nullptr));
				ptr = reference_internal(input);
				return;
			}

			if (object_is_impl(obj, static_cast<std::vector<cv::cuda::GpuMat>*>(nullptr))) {
				val0 = object_as_impl(obj, static_cast<std::vector<cv::cuda::GpuMat>*>(nullptr));
				ptr = std::make_shared<Array>(val0);
				return;
			}

			if (object_is_impl(obj, static_cast<std::vector<cv::Mat>*>(nullptr))) {
				val1 = object_as_impl(obj, static_cast<std::vector<cv::Mat>*>(nullptr));
				ptr = std::make_shared<Array>(val1);
				return;
			}

			if (object_is_impl(obj, static_cast<std::vector<cv::UMat>*>(nullptr))) {
				val2 = object_as_impl(obj, static_cast<std::vector<cv::UMat>*>(nullptr));
				ptr = std::make_shared<Array>(val2);
				return;
			}

			if (object_is_impl(obj, static_cast<std::vector<cv::RotatedRect>*>(nullptr))) {
				val3 = object_as_impl(obj, static_cast<std::vector<cv::RotatedRect>*>(nullptr));
				ptr = std::make_shared<Array>(val3);
				return;
			}

			if (object_is_impl(obj, static_cast<std::vector<char>*>(nullptr))) {
				val4 = object_as_impl(obj, static_cast<std::vector<char>*>(nullptr));
				ptr = std::make_shared<Array>(val4);
				return;
			}

			if (object_is_impl(obj, static_cast<std::vector<uchar>*>(nullptr))) {
				val5 = object_as_impl(obj, static_cast<std::vector<uchar>*>(nullptr));
				ptr = std::make_shared<Array>(val5);
				return;
			}

			if (object_is_impl(obj, static_cast<std::vector<int>*>(nullptr))) {
				val6 = object_as_impl(obj, static_cast<std::vector<int>*>(nullptr));
				ptr = std::make_shared<Array>(val6);
				return;
			}

			if (object_is_impl(obj, static_cast<std::vector<float>*>(nullptr))) {
				val7 = object_as_impl(obj, static_cast<std::vector<float>*>(nullptr));
				ptr = std::make_shared<Array>(val7);
				return;
			}

			if (object_is_impl(obj, static_cast<std::vector<double>*>(nullptr))) {
				val8 = object_as_impl(obj, static_cast<std::vector<double>*>(nullptr));
				ptr = std::make_shared<Array>(val8);
				return;
			}

			if (object_is_impl(obj, static_cast<std::vector<cv::Point>*>(nullptr))) {
				val9 = object_as_impl(obj, static_cast<std::vector<cv::Point>*>(nullptr));
				ptr = std::make_shared<Array>(val9);
				return;
			}

			if (object_is_impl(obj, static_cast<std::vector<cv::Point2f>*>(nullptr))) {
				val10 = object_as_impl(obj, static_cast<std::vector<cv::Point2f>*>(nullptr));
				ptr = std::make_shared<Array>(val10);
				return;
			}

			if (object_is_impl(obj, static_cast<std::vector<cv::Rect>*>(nullptr))) {
				val11 = object_as_impl(obj, static_cast<std::vector<cv::Rect>*>(nullptr));
				ptr = std::make_shared<Array>(val11);
				return;
			}

			if (object_is_impl(obj, static_cast<std::vector<cv::Size>*>(nullptr))) {
				val12 = object_as_impl(obj, static_cast<std::vector<cv::Size>*>(nullptr));
				ptr = std::make_shared<Array>(val12);
				return;
			}

			if (object_is_impl(obj, static_cast<std::vector<cv::Vec6f>*>(nullptr))) {
				val13 = object_as_impl(obj, static_cast<std::vector<cv::Vec6f>*>(nullptr));
				ptr = std::make_shared<Array>(val13);
				return;
			}

			if (object_is_impl(obj, static_cast<std::vector<std::vector<char>>*>(nullptr))) {
				val14 = object_as_impl(obj, static_cast<std::vector<std::vector<char>>*>(nullptr));
				ptr = std::make_shared<Array>(val14);
				return;
			}

			if (object_is_impl(obj, static_cast<std::vector<std::vector<int>>*>(nullptr))) {
				val15 = object_as_impl(obj, static_cast<std::vector<std::vector<int>>*>(nullptr));
				ptr = std::make_shared<Array>(val15);
				return;
			}

			if (object_is_impl(obj, static_cast<std::vector<std::vector<cv::Point>>*>(nullptr))) {
				val16 = object_as_impl(obj, static_cast<std::vector<std::vector<cv::Point>>*>(nullptr));
				ptr = std::make_shared<Array>(val16);
				return;
			}

			if (object_is_impl(obj, static_cast<std::vector<std::vector<cv::Point2f>>*>(nullptr))) {
				val17 = object_as_impl(obj, static_cast<std::vector<std::vector<cv::Point2f>>*>(nullptr));
				ptr = std::make_shared<Array>(val17);
				return;
			}
		}

		ArraysSharedPtr(const std::shared_ptr<Array>& ptr) : ptr(ptr) {}

		auto& operator*() {
			return *ptr;
		}

		std::shared_ptr<Array> ptr;
		std::vector<cv::cuda::GpuMat> val0;
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
	decltype(auto) object_as_arrays(const _To& obj, Array*) {
		return ArraysSharedPtr<Array>(obj);
	}

	template<typename Array>
	decltype(auto) object_as_arrays(const std::shared_ptr<Array>& ptr) {
		return ArraysSharedPtr<Array>(ptr);
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
