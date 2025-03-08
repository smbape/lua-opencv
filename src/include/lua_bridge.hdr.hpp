#pragma once

#include <luadef.hpp>
#include <opencv2/opencv.hpp>

#ifdef HAVE_OPENCV_DNN
#include <opencv2/dnn.hpp>
#endif

#ifdef HAVE_OPENCV_FLANN
#include <opencv2/flann.hpp>
#endif

#ifdef HAVE_OPENCV_GAPI
#include <opencv2/gapi.hpp>
#endif

namespace LUA_MODULE_NAME {
#ifdef HAVE_OPENCV_DNN
	// =====================================================================================
	// https://github.com/opencv/opencv/blob/4.11.0/modules/dnn/misc/python/pyopencv_dnn.hpp
	// =====================================================================================
	inline bool lua_is(lua_State* L, int index, cv::dnn::DictValue*);

	inline cv::dnn::DictValue lua_to(lua_State* L, int index, cv::dnn::DictValue*);

	inline int lua_push(lua_State* L, const cv::dnn::DictValue& dv);

	inline bool lua_is(lua_State* L, int index, cv::dnn::LayerParams*);

	inline void lua_to(lua_State* L, int index, cv::dnn::LayerParams& lp);

	inline cv::dnn::LayerParams lua_to(lua_State* L, int index, cv::dnn::LayerParams*);

	inline int lua_push(lua_State* L, const cv::dnn::LayerParams& dv);

	inline int lua_push(lua_State* L, const std::vector<cv::dnn::Target>& t);
#endif

#ifdef HAVE_OPENCV_FLANN
	// =========================================================================================
	// https://github.com/opencv/opencv/tree/4.11.0/modules/flann/misc/python/pyopencv_flann.hpp
	// =========================================================================================
	inline bool lua_is(lua_State* L, int index, cv::flann::IndexParams*);

	inline void lua_to(lua_State* L, int index, cv::flann::IndexParams& p);

	inline std::shared_ptr<cv::flann::IndexParams> lua_to(lua_State* L, int index, cv::flann::IndexParams*);

	inline bool lua_is(lua_State* L, int index, cv::flann::SearchParams*);

	inline void lua_to(lua_State* L, int index, cv::flann::SearchParams& value);

	inline std::shared_ptr<cv::flann::SearchParams> lua_to(lua_State* L, int index, cv::flann::SearchParams*);
#endif

#ifdef HAVE_OPENCV_GAPI
	// =========================================================================================
	// https://github.com/opencv/opencv/tree/4.11.0/modules/gapi/misc/python/pyopencv_gapi.hpp
	// =========================================================================================
	inline bool lua_is(lua_State* L, int index, cv::gapi::wip::draw::Prim*);

	inline cv::gapi::wip::draw::Prim lua_to(lua_State* L, int index, cv::gapi::wip::draw::Prim*);

	inline int lua_push(lua_State* L, const cv::gapi::wip::draw::Prim& prim);

	inline bool lua_is(lua_State* L, int index, cv::GMetaArg*);

	inline cv::GMetaArg lua_to(lua_State* L, int index, cv::GMetaArg*);

	inline bool lua_is(lua_State* L, int index, cv::GArg*);

	inline std::shared_ptr<cv::GArg> lua_to(lua_State* L, int index, cv::GArg*);

	inline int lua_push(lua_State* L, const cv::GArg& value);

	inline int lua_push(lua_State* L, const cv::detail::OpaqueRef& o);

	inline int lua_push(lua_State* L, const cv::detail::VectorRef& v);

	inline cv::GRunArg lua_to(lua_State* L, int index, cv::GRunArg*);

	inline int lua_push(lua_State* L, const cv::GRunArg& v);

	inline int lua_push(lua_State* L, const cv::GOptRunArg& v);

	// TODO
	// {"kernels", CV_PY_FN_WITH_KW(pyopencv_cv_gapi_kernels), "kernels(...) -> GKernelPackage"}, \
	// {"__op", CV_PY_FN_WITH_KW(pyopencv_cv_gapi_op), "__op(...) -> retval\n"},
#endif


	// ================================
	// cv::Range
	// ================================
	inline bool lua_is(lua_State* L, int index, cv::Range*);

	inline void lua_to(lua_State* L, int index, cv::Range& range);

	inline cv::Range lua_to(lua_State* L, int index, cv::Range*);

	inline int lua_push(lua_State* L, const cv::Range& range);

	inline void lua_to(lua_State* L, int index, std::vector<cv::Range>& ranges);


	// ================================
	// cv::Ptr
	// ================================
	template<typename T>
	inline bool lua_is(lua_State* L, int index, cv::Ptr<T>*);

	template<typename T>
	inline cv::Ptr<T> lua_to(lua_State* L, int index, cv::Ptr<T>*);

	template<typename T>
	inline int lua_push(lua_State* L, cv::Ptr<T>& ptr);


	// ================================
	// cv::Point_
	// ================================
	template<typename _Tp>
	inline bool lua_is(lua_State* L, int index, cv::Point_<_Tp>*);

	template<typename _Tp>
	inline cv::Point_<_Tp> lua_to(lua_State* L, int index, cv::Point_<_Tp>*);

	template<typename _Tp>
	inline int lua_push(lua_State* L, const cv::Point_<_Tp>& pt);


	// ================================
	// cv::Point3_
	// ================================
	template<typename _Tp>
	inline bool lua_is(lua_State* L, int index, cv::Point3_<_Tp>*);

	template<typename _Tp>
	inline cv::Point3_<_Tp> lua_to(lua_State* L, int index, cv::Point3_<_Tp>*);

	template<typename _Tp>
	inline int lua_push(lua_State* L, const cv::Point3_<_Tp>& pt);

	// ================================
	// cv::Rect_
	// ================================
	template<typename _Tp>
	inline bool lua_is(lua_State* L, int index, cv::Rect_<_Tp>*);

	template<typename _Tp>
	inline cv::Rect_<_Tp> lua_to(lua_State* L, int index, cv::Rect_<_Tp>*);

	template<typename _Tp>
	inline int lua_push(lua_State* L, const cv::Rect_<_Tp>& rect);

	// ================================
	// cv::Scalar_
	// ================================
	template<typename _Tp>
	inline bool lua_is(lua_State* L, int index, cv::Scalar_<_Tp>*);

	template<typename _Tp>
	inline cv::Scalar_<_Tp> lua_to(lua_State* L, int index, cv::Scalar_<_Tp>*);

	template<typename _Tp>
	inline int lua_push(lua_State* L, const cv::Scalar_<_Tp>& s);


	// ================================
	// cv::Size_
	// ================================
	template<typename _Tp>
	inline bool lua_is(lua_State* L, int index, cv::Size_<_Tp>*);

	template<typename _Tp>
	inline cv::Size_<_Tp> lua_to(lua_State* L, int index, cv::Size_<_Tp>*);

	template<typename _Tp>
	inline int lua_push(lua_State* L, const cv::Size_<_Tp>& rect);


	// ================================
	// cv::Vec
	// ================================
	template<typename _Tp, int cn>
	inline bool lua_is(lua_State* L, int index, cv::Vec<_Tp, cn>*);

	template<typename _Tp, int cn>
	inline cv::Vec<_Tp, cn> lua_to(lua_State* L, int index, cv::Vec<_Tp, cn>*);

	template<typename _Tp, int cn>
	inline int lua_push(lua_State* L, const cv::Vec<_Tp, cn>& vec);


	// ================================
	// cv::util::variant
	// ================================
	template<typename... _Ts>
	inline bool lua_is(lua_State* L, int index, cv::util::variant<_Ts...>* ptr);

	template<typename... _Ts>
	inline cv::util::variant<_Ts...> lua_to(lua_State* L, int index, cv::util::variant<_Ts...>* ptr);

	template<typename... _Ts>
	inline int lua_push(lua_State* L, const cv::util::variant<_Ts...>& value);


	// ================================
	// cv::optional
	// ================================
	template<typename T>
	inline int lua_push(lua_State* L, const cv::optional<T>& opt);
}
