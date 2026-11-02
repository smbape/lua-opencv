#pragma once

#ifdef HAVE_OPENCV_FLANN

#include <lua_bridge_common.hdr.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/flann.hpp>

namespace LUA_MODULE_NAME {

	// =========================================================================================
	// https://github.com/opencv/opencv/tree/4.11.0/modules/flann/misc/python/pyopencv_flann.hpp
	// =========================================================================================

	inline void lua_to(lua_State* L, int index, cv::flann::IndexParams& p, bool& is_valid);

	inline std::shared_ptr<cv::flann::IndexParams> lua_to(lua_State* L, int index, cv::flann::IndexParams*, bool& is_valid);

	inline void lua_to(lua_State* L, int index, cv::flann::SearchParams& value, bool& is_valid);

	inline std::shared_ptr<cv::flann::SearchParams> lua_to(lua_State* L, int index, cv::flann::SearchParams*, bool& is_valid);

}

#endif
