#pragma once

#ifdef HAVE_OPENCV_DNN

#include <lua_bridge_common.hdr.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

namespace LUA_MODULE_NAME {

	// =====================================================================================
	// https://github.com/opencv/opencv/blob/4.11.0/modules/dnn/misc/python/pyopencv_dnn.hpp
	// =====================================================================================

	inline cv::dnn::DictValue lua_to(lua_State* L, int index, cv::dnn::DictValue*, bool& is_valid);

	inline void lua_push(lua_State* L, const cv::dnn::DictValue& dv);

	inline void lua_to(lua_State* L, int index, cv::dnn::LayerParams& lp, bool& is_valid);

	inline cv::dnn::LayerParams lua_to(lua_State* L, int index, cv::dnn::LayerParams*, bool& is_valid);

	inline void lua_push(lua_State* L, const cv::dnn::LayerParams& dv);

	inline void lua_push(lua_State* L, const std::vector<cv::dnn::Target>& t);

}

#endif
