#pragma once

#ifdef HAVE_OPENCV_DNN

#include <lua_bridge_dnn.hdr.hpp>
#include <lua_generated_include.hpp>
#include <register_all.hpp>
#include <lua_bridge_common.hpp>


namespace LUA_MODULE_NAME {

	// https://github.com/opencv/opencv/blob/4.11.0/modules/dnn/misc/python/pyopencv_dnn.hpp#L7-L37
	inline cv::dnn::DictValue lua_to(lua_State* L, int index, cv::dnn::DictValue*, bool& is_valid) {
		is_valid = lua_isnil(L, index);
		if (is_valid) {
			return cv::dnn::DictValue();
		}

		{
			auto value = lua_to(L, index, static_cast<int64*>(nullptr), is_valid);
			if (is_valid) {
				return cv::dnn::DictValue(value);
			}
		}

		{
			auto value = lua_to(L, index, static_cast<double*>(nullptr), is_valid);
			if (is_valid) {
				return cv::dnn::DictValue(value);
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::string*>(nullptr), is_valid);
			if (is_valid) {
				return cv::dnn::DictValue(value);
			}
		}

		return cv::dnn::DictValue();
	}

	// https://github.com/opencv/opencv/blob/4.11.0/modules/dnn/misc/python/pyopencv_dnn.hpp#L40-L51
	template<typename T>
	void lua_push_dict_value(lua_State* L, const cv::dnn::DictValue& dv) {
		if (dv.size() > 1) {
			std::vector<T> vec(dv.size());
			for (int i = 0; i < dv.size(); ++i) {
				vec[i] = dv.get<T>(i);
			}
			lua_push(L, vec);
		}
		else {
			lua_push(L, dv.get<T>());
		}
	}

	// https://github.com/opencv/opencv/blob/4.11.0/modules/dnn/misc/python/pyopencv_dnn.hpp#L54-L61
	inline void lua_push(lua_State* L, const cv::dnn::DictValue& dv) {
		if (dv.isInt()) lua_push_dict_value<int>(L, dv);
		else if (dv.isReal()) lua_push_dict_value<float>(L, dv);
		else if (dv.isString()) lua_push_dict_value<cv::String>(L, dv);
		else luaL_error(L, "Unknown value type");
	}

	// https://github.com/opencv/opencv/blob/4.11.0/modules/dnn/misc/python/pyopencv_dnn.hpp#L75-L88
	inline void lua_to(lua_State* L, int index, cv::dnn::LayerParams& lp, bool& is_valid) {
		using K = std::string;
		using V = cv::dnn::DictValue;
		using Map = std::map<K, V>;

		auto kv_holder = lua_to(L, index, static_cast<Map*>(nullptr), is_valid);
		if (!is_valid) {
			return;
		}
		decltype(auto) kv = extract_holder(kv_holder, static_cast<Map*>(nullptr));

		for (const auto& [k, v] : kv) {
			lp.set(k, v);
		}
	}

	inline cv::dnn::LayerParams lua_to(lua_State* L, int index, cv::dnn::LayerParams*, bool& is_valid) {
		cv::dnn::LayerParams lp;
		lua_to(L, index, lp, is_valid);
		return lp;
	}

	// https://github.com/opencv/opencv/blob/4.11.0/modules/dnn/misc/python/pyopencv_dnn.hpp#L65-L72
	inline void lua_push(lua_State* L, const cv::dnn::LayerParams& lp) {
		lua_newtable(L);
		int index = lua_gettop(L);
		int i = 0;
		for (auto it = lp.begin(); it != lp.end(); ++it) {
			lua_push(L, it->first);
			lua_push(L, it->second);
			lua_rawset(L, index);
		}
	}

	// https://github.com/opencv/opencv/blob/4.11.0/modules/dnn/misc/python/pyopencv_dnn.hpp#L91-L94
	inline void lua_push(lua_State* L, const std::vector<cv::dnn::Target>& t) {
		lua_push(L, std::vector<int>(t.begin(), t.end()));
	}

}

#endif
