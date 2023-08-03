#pragma once

#include <lua_utils.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

namespace LUA_MODULE_NAME {
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
	bool object_is_impl(_To& obj, cv::Point_<_Tp>*) {
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
	bool object_is_impl(_To& obj, cv::Rect_<_Tp>*) {
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
	bool object_is_impl(_To& obj, cv::Scalar_<_Tp>*) {
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
	bool object_is_impl(_To& obj, cv::Size_<_Tp>*) {
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
	bool object_is_impl(_To& obj, cv::Vec<_Tp, cn>*) {
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

	template<typename Array, typename T, typename _To = sol::object>
	bool object_is_array(const _To& obj) {
		return object_is_impl(obj, static_cast<Array*>(nullptr))
			|| object_is_impl(obj, static_cast<cv::cuda::GpuMat*>(nullptr))
			|| object_is_impl(obj, static_cast<cv::Mat*>(nullptr))
			|| object_is_impl(obj, static_cast<cv::UMat*>(nullptr))
			|| object_is_impl(obj, static_cast<cv::Scalar*>(nullptr))
			|| std::is_same<Array, cv::_InputArray>::value&& object_is_impl(obj, static_cast<double*>(nullptr))
			;
	}

	template<typename Array, typename T, typename _To = sol::object>
	bool object_is_array(_To& obj) {
		return object_is_impl(obj, static_cast<Array*>(nullptr))
			|| object_is_impl(obj, static_cast<cv::cuda::GpuMat*>(nullptr))
			|| object_is_impl(obj, static_cast<cv::Mat*>(nullptr))
			|| object_is_impl(obj, static_cast<cv::UMat*>(nullptr))
			|| object_is_impl(obj, static_cast<cv::Scalar*>(nullptr))
			|| std::is_same<Array, cv::_InputArray>::value&& object_is_impl(obj, static_cast<double*>(nullptr))
			;
	}

	template<typename Array, typename T, typename _To = sol::object>
	auto object_as_array(const _To& obj) {
		if (object_is_impl(obj, static_cast<Array*>(nullptr))) {
			const Array& input = object_as_impl(obj, static_cast<Array*>(nullptr));
			return reference_internal(input);
		}

		if (object_is_impl(obj, static_cast<cv::cuda::GpuMat*>(nullptr))) {
			auto& input = object_as_impl(obj, static_cast<cv::cuda::GpuMat*>(nullptr));
			return std::make_shared<Array>(input);
		}

		if (object_is_impl(obj, static_cast<cv::Mat*>(nullptr))) {
			auto& input = object_as_impl(obj, static_cast<cv::Mat*>(nullptr));
			return std::make_shared<Array>(input);
		}

		if (object_is_impl(obj, static_cast<cv::UMat*>(nullptr))) {
			auto& input = object_as_impl(obj, static_cast<cv::UMat*>(nullptr));
			return std::make_shared<Array>(input);
		}

		if (object_is_impl(obj, static_cast<cv::Scalar*>(nullptr))) {
			auto input = object_as_impl(obj, static_cast<cv::Scalar*>(nullptr));
			return std::make_shared<Array>(input);
		}

		if constexpr (std::is_same_v<Array, cv::_InputArray>) {
			if (object_is_impl(obj, static_cast<double*>(nullptr))) {
				auto input = object_as_impl(obj, static_cast<double*>(nullptr));
				return std::make_shared<Array>(input);
			}
		}

		return std::shared_ptr<Array>();
	}

	template<typename Array, typename T, typename _To = sol::object>
	auto object_as_array(_To& obj) {
		if (object_is_impl(obj, static_cast<Array*>(nullptr))) {
			const Array& input = object_as_impl(obj, static_cast<Array*>(nullptr));
			return reference_internal(input);
		}

		if (object_is_impl(obj, static_cast<cv::cuda::GpuMat*>(nullptr))) {
			auto& input = object_as_impl(obj, static_cast<cv::cuda::GpuMat*>(nullptr));
			return std::make_shared<Array>(input);
		}

		if (object_is_impl(obj, static_cast<cv::Mat*>(nullptr))) {
			auto& input = object_as_impl(obj, static_cast<cv::Mat*>(nullptr));
			return std::make_shared<Array>(input);
		}

		if (object_is_impl(obj, static_cast<cv::UMat*>(nullptr))) {
			auto& input = object_as_impl(obj, static_cast<cv::UMat*>(nullptr));
			return std::make_shared<Array>(input);
		}

		if (object_is_impl(obj, static_cast<cv::Scalar*>(nullptr))) {
			auto input = object_as_impl(obj, static_cast<cv::Scalar*>(nullptr));
			return std::make_shared<Array>(input);
		}

		if constexpr (std::is_same_v<Array, cv::_InputArray>) {
			if (object_is_impl(obj, static_cast<double*>(nullptr))) {
				auto input = object_as_impl(obj, static_cast<double*>(nullptr));
				return std::make_shared<Array>(input);
			}
		}

		return std::shared_ptr<Array>();
	}

	template<typename Array, typename T, typename _To = sol::object>
	bool object_is_arrays(const _To& obj) {
		return object_is_impl(obj, static_cast<Array*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<cv::cuda::GpuMat>*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<cv::Mat>*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<cv::UMat>*>(nullptr))
			;
	}

	template<typename Array, typename T, typename _To = sol::object>
	bool object_is_arrays(_To& obj) {
		return object_is_impl(obj, static_cast<Array*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<cv::cuda::GpuMat>*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<cv::Mat>*>(nullptr))
			|| object_is_impl(obj, static_cast<std::vector<cv::UMat>*>(nullptr))
			;
	}

	template<typename Array, typename T, typename _To = sol::object>
	auto object_as_arrays(const _To& obj) {
		if (object_is_impl(obj, static_cast<Array*>(nullptr))) {
			const Array& input = object_as_impl(obj, static_cast<Array*>(nullptr));
			return reference_internal(input);
		}

		if (object_is_impl(obj, static_cast<std::vector<cv::cuda::GpuMat>*>(nullptr))) {
			auto input = object_as_impl(obj, static_cast<std::vector<cv::cuda::GpuMat>*>(nullptr));
			return std::make_shared<Array>(input);
		}

		if (object_is_impl(obj, static_cast<std::vector<cv::Mat>*>(nullptr))) {
			auto input = object_as_impl(obj, static_cast<std::vector<cv::Mat>*>(nullptr));
			return std::make_shared<Array>(input);
		}

		if (object_is_impl(obj, static_cast<std::vector<cv::UMat>*>(nullptr))) {
			auto input = object_as_impl(obj, static_cast<std::vector<cv::UMat>*>(nullptr));
			return std::make_shared<Array>(input);
		}

		return std::shared_ptr<Array>();
	}

	template<typename Array, typename T, typename _To = sol::object>
	auto object_as_arrays(_To& obj) {
		if (object_is_impl(obj, static_cast<Array*>(nullptr))) {
			const Array& input = object_as_impl(obj, static_cast<Array*>(nullptr));
			return reference_internal(input);
		}

		if (object_is_impl(obj, static_cast<std::vector<cv::cuda::GpuMat>*>(nullptr))) {
			auto input = object_as_impl(obj, static_cast<std::vector<cv::cuda::GpuMat>*>(nullptr));
			return std::make_shared<Array>(input);
		}

		if (object_is_impl(obj, static_cast<std::vector<cv::Mat>*>(nullptr))) {
			auto input = object_as_impl(obj, static_cast<std::vector<cv::Mat>*>(nullptr));
			return std::make_shared<Array>(input);
		}

		if (object_is_impl(obj, static_cast<std::vector<cv::UMat>*>(nullptr))) {
			auto input = object_as_impl(obj, static_cast<std::vector<cv::UMat>*>(nullptr));
			return std::make_shared<Array>(input);
		}

		return std::shared_ptr<Array>();
	}
}

#include <opencv2/gapi/gscalar.hpp>
#include <opencv2/gapi/garray.hpp>
#include <opencv2/gapi/gopaque.hpp>

namespace sol {
	template <>
	struct is_container<cv::FileNode> : std::false_type { };

    template <>
    struct meta::supports_op_left_shift<std::ostream, cv::GScalarDesc> : std::false_type { };

    template <>
    struct meta::supports_op_left_shift<std::ostream, cv::GArrayDesc> : std::false_type { };

    template <>
    struct meta::supports_op_left_shift<std::ostream, cv::GOpaqueDesc> : std::false_type { };
} // namespace sol
