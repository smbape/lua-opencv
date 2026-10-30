#pragma once

#include <lua_bridge_common.hdr.hpp>
#include <opencv2/opencv.hpp>

#ifdef HAVE_OPENCV_GAPI
#include <opencv2/gapi.hpp>
#endif

namespace LUA_MODULE_NAME {

	// ================================
	// cv::Range
	// ================================

	inline void lua_to(lua_State* L, int index, cv::Range& range, bool& is_valid);

	inline cv::Range lua_to(lua_State* L, int index, cv::Range*, bool& is_valid);

	inline void lua_push(lua_State* L, const cv::Range& range);

	inline void lua_to(lua_State* L, int index, std::vector<cv::Range>& ranges, bool& is_valid);


	// ================================
	// cv::Ptr
	// ================================

	template<typename T>
	inline cv::Ptr<T> lua_to(lua_State* L, int index, cv::Ptr<T>*, bool& is_valid);

	template<typename T>
	inline void lua_push(lua_State* L, cv::Ptr<T>& ptr);


	// ================================
	// cv::Point_
	// ================================

	template<typename _Tp>
	inline cv::Point_<_Tp> lua_to(lua_State* L, int index, cv::Point_<_Tp>*, bool& is_valid);

	template<typename _Tp>
	inline void lua_push(lua_State* L, const cv::Point_<_Tp>& pt);


	// ================================
	// cv::Point3_
	// ================================

	template<typename _Tp>
	inline cv::Point3_<_Tp> lua_to(lua_State* L, int index, cv::Point3_<_Tp>*, bool& is_valid);

	template<typename _Tp>
	inline void lua_push(lua_State* L, const cv::Point3_<_Tp>& pt);


	// ================================
	// cv::Rect_
	// ================================

	template<typename _Tp>
	inline cv::Rect_<_Tp> lua_to(lua_State* L, int index, cv::Rect_<_Tp>*, bool& is_valid);

	template<typename _Tp>
	inline void lua_push(lua_State* L, const cv::Rect_<_Tp>& rect);


	// ================================
	// cv::Scalar_
	// ================================

	template<typename _Tp>
	inline cv::Scalar_<_Tp> lua_to(lua_State* L, int index, cv::Scalar_<_Tp>*, bool& is_valid);

	template<typename _Tp>
	inline void lua_push(lua_State* L, const cv::Scalar_<_Tp>& s);


	// ================================
	// cv::Size_
	// ================================

	template<typename _Tp>
	inline cv::Size_<_Tp> lua_to(lua_State* L, int index, cv::Size_<_Tp>*, bool& is_valid);

	template<typename _Tp>
	inline void lua_push(lua_State* L, const cv::Size_<_Tp>& rect);


	// ================================
	// cv::Vec
	// ================================

	template<typename _Tp, int cn>
	inline cv::Vec<_Tp, cn> lua_to(lua_State* L, int index, cv::Vec<_Tp, cn>*, bool& is_valid);

	template<typename _Tp, int cn>
	inline void lua_push(lua_State* L, const cv::Vec<_Tp, cn>& vec);

}
