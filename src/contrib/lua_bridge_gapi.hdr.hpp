#pragma once

#ifdef HAVE_OPENCV_GAPI

#include <lua_bridge_common.hdr.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/gapi.hpp>

namespace LUA_MODULE_NAME {

	// ================================
	// cv::util::variant
	// ================================

	template<typename... _Ts>
	inline cv::util::variant<_Ts...> lua_to(lua_State* L, int index, cv::util::variant<_Ts...>* ptr, bool& is_valid);


	// ================================
	// cv::optional
	// ================================

	template<typename T>
	inline void lua_push(lua_State* L, const cv::optional<T>& opt);

	// ================================

	template<typename... _Ts>
	inline void lua_push(lua_State* L, const cv::util::variant<_Ts...>& value);

	inline void lua_push(lua_State* L, const cv::gapi::wip::draw::Prim& prim);

	inline cv::gapi::wip::draw::Prim lua_to(lua_State* L, int index, cv::gapi::wip::draw::Prim*, bool& is_valid);

	inline cv::GMetaArg lua_to(lua_State* L, int index, cv::GMetaArg*, bool& is_valid);

	inline void lua_push(lua_State* L, const cv::GArg& value);

	inline std::shared_ptr<cv::GArg> lua_to(lua_State* L, int index, cv::GArg*, bool& is_valid);

	inline void lua_push(lua_State* L, const cv::detail::OpaqueRef& o);

	inline void lua_push(lua_State* L, const cv::detail::VectorRef& v);

	inline void lua_push(lua_State* L, const cv::GRunArg& v);

	inline std::shared_ptr<cv::GRunArg> lua_to(lua_State* L, int index, cv::GRunArg*, bool& is_valid);

	inline void lua_push(lua_State* L, const cv::GOptRunArg& v);

	// TODO
	// {"kernels", CV_PY_FN_WITH_KW(pyopencv_cv_gapi_kernels), "kernels(...) -> GKernelPackage"}, \
	// {"__op", CV_PY_FN_WITH_KW(pyopencv_cv_gapi_op), "__op(...) -> retval\n"},

	inline cv::detail::ExtractArgsCallback lua_to(lua_State* L, int index, cv::detail::ExtractArgsCallback*, bool& is_valid);
	inline cv::detail::ExtractMetaCallback lua_to(lua_State* L, int index, cv::detail::ExtractMetaCallback*, bool& is_valid);

	inline std::shared_ptr<cv::GProtoInputArgs> lua_to(lua_State* L, int index, cv::GProtoInputArgs*, bool& is_valid);
	inline std::shared_ptr<cv::GProtoOutputArgs> lua_to(lua_State* L, int index, cv::GProtoOutputArgs*, bool& is_valid);

	inline std::shared_ptr<cv::gapi::wip::IStreamSource> lua_to(lua_State* L, int index, cv::gapi::wip::IStreamSource*, bool& is_valid);
}

#endif
