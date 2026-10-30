#pragma once

#ifdef HAVE_OPENCV_FLANN

#include <lua_bridge_flann.hdr.hpp>
#include <lua_generated_include.hpp>
#include <register_all.hpp>
#include <lua_bridge_common.hpp>

namespace LUA_MODULE_NAME {

	// https://github.com/opencv/opencv/blob/4.11.0/modules/dnn/misc/python/pyopencv_dnn.hpp#L18-L103
	inline void lua_to(lua_State* L, int index, cv::flann::IndexParams& p, bool& is_valid) {
		using K = std::string;
		using V = std::variant<bool, int, double>;
		using Map = std::map<K, V>;

		auto kv_holder = lua_to(L, index, static_cast<Map*>(nullptr), is_valid);
		if (!is_valid) {
			return;
		}
		decltype(auto) kv = extract_holder(kv_holder, static_cast<Map*>(nullptr));

		for (const auto& [k, v] : kv) {
			if (std::holds_alternative<bool>(v)) {
				p.setBool(k, std::get<bool>(v));
			}
			else if (std::holds_alternative<int>(v)) {
				if (k == "algorithm") {
					p.setAlgorithm(std::get<int>(v));
				}
				else {
					p.setInt(k, std::get<int>(v));
				}
			}
			else if (std::holds_alternative<double>(v)) {
				if (k == "eps") {
					p.setFloat(k, static_cast<float>(std::get<double>(v)));
				}
				else {
					p.setDouble(k, std::get<double>(v));
				}
			}
		}
	}

	inline std::shared_ptr<cv::flann::IndexParams> lua_to(lua_State* L, int index, cv::flann::IndexParams*, bool& is_valid) {
		auto p = std::make_shared<cv::flann::IndexParams>();
		lua_to(L, index, *p, is_valid);
		return p;
	}

	inline void lua_to(lua_State* L, int index, cv::flann::SearchParams& value, bool& is_valid) {
		lua_to(L, index, static_cast<cv::flann::IndexParams&>(value), is_valid);
	}

	inline std::shared_ptr<cv::flann::SearchParams> lua_to(lua_State* L, int index, cv::flann::SearchParams*, bool& is_valid) {
		auto value = std::make_shared<cv::flann::SearchParams>();
		lua_to(L, index, *value, is_valid);
		return value;
	}

}

#endif
