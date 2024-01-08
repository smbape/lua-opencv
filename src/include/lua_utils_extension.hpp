#pragma once

#include <lua_generated_include.hpp>
#include <lua_utils_arrays.hpp>

namespace LUA_MODULE_NAME {
	void register_utils_extension(sol::state_view& lua, sol::table& module);

	// cv::Ptr
	template<typename _To, typename _Tp>
	inline auto maybe_impl(_To& obj, cv::Ptr<_Tp>*) {
		auto wrapper = obj.template as<sol::optional<std::shared_ptr<_Tp>>>();
		if (wrapper) {
			return std::make_shared<cv::Ptr<_Tp>>(*wrapper);
		}

		return std::shared_ptr<cv::Ptr<_Tp>>();
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
			return std::shared_ptr<std::vector<cv::Ptr<_Tp>>>();
		}

		std::shared_ptr<std::vector<cv::Ptr<_Tp>>> res(new std::vector<cv::Ptr<_Tp>>());
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
	inline auto maybe_impl(_To& obj, cv::Point_<_Tp>*, bool nested) {
		if (obj == sol::lua_nil) {
			std::shared_ptr<cv::Point_<_Tp>> res(new cv::Point_<_Tp>());
			return res;
		}

		if (obj.get_type() == sol::type::userdata) {
			if (obj.template is<std::shared_ptr<cv::Point_<_Tp>>>()) {
				return obj.template as<std::shared_ptr<cv::Point_<_Tp>>>();
			}

			if (obj.template is<cv::Point_<_Tp>&>()) {
				return reference_internal(obj.template as<cv::Point_<_Tp>&>());
			}

			if constexpr (!std::is_same_v<_Tp, float>) {
				if (!nested) {
					auto maybe_float = maybe_impl(obj, static_cast<cv::Point_<float>*>(nullptr), true);
					if (maybe_float) {
						return std::make_shared<cv::Point_<_Tp>>(*maybe_float);
					}
				}
			}

			if constexpr (!std::is_same_v<_Tp, double>) {
				if (!nested) {
					auto maybe_double = maybe_impl(obj, static_cast<cv::Point_<double>*>(nullptr), true);
					if (maybe_double) {
						return std::make_shared<cv::Point_<_Tp>>(*maybe_double);
					}
				}
			}

			if constexpr (!std::is_same_v<_Tp, int>) {
				if (!nested) {
					auto maybe_int = maybe_impl(obj, static_cast<cv::Point_<int>*>(nullptr), true);
					if (maybe_int) {
						return std::make_shared<cv::Point_<_Tp>>(*maybe_int);
					}
				}
			}

			return std::shared_ptr<cv::Point_<_Tp>>();
		}

		auto vec = maybe_impl(obj, static_cast<std::vector<_Tp>*>(nullptr));
		if (!vec || (*vec).size() != 2) {
			return std::shared_ptr<cv::Point_<_Tp>>();
		}

		std::shared_ptr<cv::Point_<_Tp>> res(new cv::Point_<_Tp>());

		(*res).x = (*vec)[0];
		(*res).y = (*vec)[1];

		return res;
	}

	template<typename _To, typename _Tp>
	inline auto maybe_impl(_To& obj, cv::Point_<_Tp>* ptr) {
		return maybe_impl(obj, ptr, false);
	}

	template<typename _To, typename _Tp>
	inline auto maybe_impl(const _To& obj, cv::Point_<_Tp>* ptr) {
		return maybe_impl(const_cast<_To&>(obj), ptr);
	}

	// cv::Point3_
	template<typename _To, typename _Tp>
	inline auto maybe_impl(_To& obj, cv::Point3_<_Tp>*, bool nested) {
		if (obj == sol::lua_nil) {
			std::shared_ptr<cv::Point3_<_Tp>> res(new cv::Point3_<_Tp>());
			return res;
		}

		if (obj.get_type() == sol::type::userdata) {
			if (obj.template is<std::shared_ptr<cv::Point3_<_Tp>>>()) {
				return obj.template as<std::shared_ptr<cv::Point3_<_Tp>>>();
			}

			if (obj.template is<cv::Point3_<_Tp>&>()) {
				return reference_internal(obj.template as<cv::Point3_<_Tp>&>());
			}

			if constexpr (!std::is_same_v<_Tp, float>) {
				if (!nested) {
					auto maybe_float = maybe_impl(obj, static_cast<cv::Point3_<float>*>(nullptr), true);
					if (maybe_float) {
						return std::make_shared<cv::Point3_<_Tp>>(*maybe_float);
					}
				}
			}

			if constexpr (!std::is_same_v<_Tp, double>) {
				if (!nested) {
					auto maybe_double = maybe_impl(obj, static_cast<cv::Point3_<double>*>(nullptr), true);
					if (maybe_double) {
						return std::make_shared<cv::Point3_<_Tp>>(*maybe_double);
					}
				}
			}

			if constexpr (!std::is_same_v<_Tp, int>) {
				if (!nested) {
					auto maybe_int = maybe_impl(obj, static_cast<cv::Point3_<int>*>(nullptr), true);
					if (maybe_int) {
						return std::make_shared<cv::Point3_<_Tp>>(*maybe_int);
					}
				}
			}

			return std::shared_ptr<cv::Point3_<_Tp>>();
		}

		auto vec = maybe_impl(obj, static_cast<std::vector<_Tp>*>(nullptr));
		if (!vec || (*vec).size() != 3) {
			return std::shared_ptr<cv::Point3_<_Tp>>();
		}

		std::shared_ptr<cv::Point3_<_Tp>> res(new cv::Point3_<_Tp>());

		(*res).x = (*vec)[0];
		(*res).y = (*vec)[1];
		(*res).z = (*vec)[2];

		return res;
	}

	template<typename _To, typename _Tp>
	inline auto maybe_impl(_To& obj, cv::Point3_<_Tp>* ptr) {
		return maybe_impl(obj, ptr, false);
	}

	template<typename _To, typename _Tp>
	inline auto maybe_impl(const _To& obj, cv::Point3_<_Tp>* ptr) {
		return maybe_impl(const_cast<_To&>(obj), ptr);
	}

	// cv::Rect_
	template<typename _To, typename _Tp>
	inline auto maybe_impl(_To& obj, cv::Rect_<_Tp>*, bool nested) {
		if (obj == sol::lua_nil) {
			std::shared_ptr<cv::Rect_<_Tp>> res(new cv::Rect_<_Tp>());
			return res;
		}

		if (obj.get_type() == sol::type::userdata) {
			if (obj.template is<std::shared_ptr<cv::Rect_<_Tp>>>()) {
				return obj.template as<std::shared_ptr<cv::Rect_<_Tp>>>();
			}

			if (obj.template is<cv::Rect_<_Tp>&>()) {
				return reference_internal(obj.template as<cv::Rect_<_Tp>&>());
			}

			if constexpr (!std::is_same_v<_Tp, float>) {
				if (!nested) {
					auto maybe_float = maybe_impl(obj, static_cast<cv::Rect_<float>*>(nullptr), true);
					if (maybe_float) {
						return std::make_shared<cv::Rect_<_Tp>>(*maybe_float);
					}
				}
			}

			if constexpr (!std::is_same_v<_Tp, double>) {
				if (!nested) {
					auto maybe_double = maybe_impl(obj, static_cast<cv::Rect_<double>*>(nullptr), true);
					if (maybe_double) {
						return std::make_shared<cv::Rect_<_Tp>>(*maybe_double);
					}
				}
			}

			if constexpr (!std::is_same_v<_Tp, int>) {
				if (!nested) {
					auto maybe_int = maybe_impl(obj, static_cast<cv::Rect_<int>*>(nullptr), true);
					if (maybe_int) {
						return std::make_shared<cv::Rect_<_Tp>>(*maybe_int);
					}
				}
			}

			return std::shared_ptr<cv::Rect_<_Tp>>();
		}

		auto vec = maybe_impl(obj, static_cast<std::vector<_Tp>*>(nullptr));
		if (!vec || (*vec).size() != 4) {
			return std::shared_ptr<cv::Rect_<_Tp>>();
		}

		std::shared_ptr<cv::Rect_<_Tp>> res(new cv::Rect_<_Tp>());

		(*res).x = (*vec)[0];
		(*res).y = (*vec)[1];
		(*res).width = (*vec)[2];
		(*res).height = (*vec)[3];

		return res;
	}


	template<typename _To, typename _Tp>
	inline auto maybe_impl(_To& obj, cv::Rect_<_Tp>* ptr) {
		return maybe_impl(obj, ptr, false);
	}

	template<typename _To, typename _Tp>
	inline auto maybe_impl(const _To& obj, cv::Rect_<_Tp>* ptr) {
		return maybe_impl(const_cast<_To&>(obj), ptr);
	}

	// cv::Scalar_
	template<typename _To, typename _Tp>
	inline auto maybe_impl(_To& obj, cv::Scalar_<_Tp>*) {
		if (obj == sol::lua_nil) {
			std::shared_ptr<cv::Scalar_<_Tp>> res(new cv::Scalar_<_Tp>());
			return res;
		}

		if (obj.get_type() == sol::type::userdata) {
			if (obj.template is<std::shared_ptr<cv::Scalar_<_Tp>>>()) {
				return obj.template as<std::shared_ptr<cv::Scalar_<_Tp>>>();
			}

			if (obj.template is<cv::Scalar_<_Tp>&>()) {
				return reference_internal(obj.template as<cv::Scalar_<_Tp>&>());
			}

			return std::shared_ptr<cv::Scalar_<_Tp>>();
		}

		auto maybe_type = obj.template as<sol::optional<_Tp>>();
		if (maybe_type) {
			return std::make_shared<cv::Scalar_<_Tp>>(*maybe_type);
		}

		auto vec = maybe_impl(obj, static_cast<std::vector<_Tp>*>(nullptr));
		if (!vec || (*vec).size() > 4) {
			return std::shared_ptr<cv::Scalar_<_Tp>>();
		}

		std::shared_ptr<cv::Scalar_<_Tp>> res(new cv::Scalar_<_Tp>());

		for (int i = 0; i < 4 && i < (*vec).size(); i++) {
			(*res)[i] = (*vec)[i];
		}

		return res;
	}

	template<typename _To, typename _Tp>
	inline auto maybe_impl(const _To& obj, cv::Scalar_<_Tp>* ptr) {
		return maybe_impl(const_cast<_To&>(obj), ptr);
	}

	// cv::Size_
	template<typename _To, typename _Tp>
	inline auto maybe_impl(_To& obj, cv::Size_<_Tp>*, bool nested) {
		if (obj == sol::lua_nil) {
			std::shared_ptr<cv::Size_<_Tp>> res(new cv::Size_<_Tp>());
			return res;
		}

		if (obj.get_type() == sol::type::userdata) {
			if (obj.template is<std::shared_ptr<cv::Size_<_Tp>>>()) {
				return obj.template as<std::shared_ptr<cv::Size_<_Tp>>>();
			}

			if (obj.template is<cv::Size_<_Tp>&>()) {
				return reference_internal(obj.template as<cv::Size_<_Tp>&>());
			}

			if constexpr (!std::is_same_v<_Tp, float>) {
				if (!nested) {
					auto maybe_float = maybe_impl(obj, static_cast<cv::Size_<float>*>(nullptr), true);
					if (maybe_float) {
						return std::make_shared<cv::Size_<_Tp>>(*maybe_float);
					}
				}
			}

			if constexpr (!std::is_same_v<_Tp, double>) {
				if (!nested) {
					auto maybe_double = maybe_impl(obj, static_cast<cv::Size_<double>*>(nullptr), true);
					if (maybe_double) {
						return std::make_shared<cv::Size_<_Tp>>(*maybe_double);
					}
				}
			}

			if constexpr (!std::is_same_v<_Tp, int>) {
				if (!nested) {
					auto maybe_int = maybe_impl(obj, static_cast<cv::Size_<int>*>(nullptr), true);
					if (maybe_int) {
						return std::make_shared<cv::Size_<_Tp>>(*maybe_int);
					}
				}
			}

			return std::shared_ptr<cv::Size_<_Tp>>();
		}

		auto vec = maybe_impl(obj, static_cast<std::vector<_Tp>*>(nullptr));
		if (!vec || (*vec).size() != 2) {
			return std::shared_ptr<cv::Size_<_Tp>>();
		}

		std::shared_ptr<cv::Size_<_Tp>> res(new cv::Size_<_Tp>());

		(*res).width = (*vec)[0];
		(*res).height = (*vec)[1];

		return res;
	}

	template<typename _To, typename _Tp>
	inline auto maybe_impl(_To& obj, cv::Size_<_Tp>* ptr) {
		return maybe_impl(obj, ptr, false);
	}

	template<typename _To, typename _Tp>
	inline auto maybe_impl(const _To& obj, cv::Size_<_Tp>* ptr) {
		return maybe_impl(const_cast<_To&>(obj), ptr);
	}

	// cv::Vec
	template<typename _To, typename _Tp, int cn>
	inline auto maybe_impl(_To& obj, cv::Vec<_Tp, cn>*) {
		if (obj == sol::lua_nil) {
			std::shared_ptr<cv::Vec<_Tp, cn>> res(new cv::Vec<_Tp, cn>());
			return res;
		}

		if (obj.get_type() == sol::type::userdata) {
			if (obj.template is<std::shared_ptr<cv::Vec<_Tp, cn>>>()) {
				return obj.template as<std::shared_ptr<cv::Vec<_Tp, cn>>>();
			}

			if (obj.template is<cv::Vec<_Tp, cn>&>()) {
				return reference_internal(obj.template as<cv::Vec<_Tp, cn>&>());
			}

			if constexpr (cn == 2) {
				auto maybe_point = maybe_impl(obj, static_cast<cv::Point_<_Tp>*>(nullptr));
				if (maybe_point) {
					return std::make_shared<cv::Vec<_Tp, cn>>(*maybe_point);
				}
			}

			if constexpr (cn == 3) {
				auto maybe_point = maybe_impl(obj, static_cast<cv::Point3_<_Tp>*>(nullptr));
				if (maybe_point) {
					return std::make_shared<cv::Vec<_Tp, cn>>(*maybe_point);
				}
			}

			return std::shared_ptr<cv::Vec<_Tp, cn>>();
		}

		auto vec = maybe_impl(obj, static_cast<std::vector<_Tp>*>(nullptr));
		if (!vec || (*vec).size() != cn) {
			return std::shared_ptr<cv::Vec<_Tp, cn>>();
		}

		std::shared_ptr<cv::Vec<_Tp, cn>> res(new cv::Vec<_Tp, cn>());

		for (int i = 0; i < cn; i++) {
			(*res)[i] = (*vec)[i];
		}

		return res;
	}

	template<typename _To, typename _Tp, int cn>
	inline auto maybe_impl(const _To& obj, cv::Vec<_Tp, cn>* ptr) {
		return maybe_impl(const_cast<_To&>(obj), ptr);
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
