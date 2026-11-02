#pragma once

#include <lua_bridge_common.hdr.hpp>
#include <lua_bridge.hdr.hpp>
#include <lua_bridge_arrays.hdr.hpp>
#include <lua_bridge_dnn.hdr.hpp>
#include <lua_bridge_flann.hdr.hpp>
#include <lua_bridge_gapi.hdr.hpp>

#include <lua_generated_include.hpp>
#include <register_all.hpp>

#include <lua_bridge_common.hpp>
#include <lua_bridge_arrays.hpp>
#include <lua_bridge_dnn.hpp>
#include <lua_bridge_flann.hpp>
#include <lua_bridge_gapi.hpp>

namespace LUA_MODULE_NAME {

	// ================================
	// cv::Range
	// ================================

	inline void lua_to(lua_State* L, int index, cv::Range& range, bool& is_valid) {
		auto vec_holder = lua_to(L, index, static_cast<std::pair<int, int>*>(nullptr), is_valid);
		if (!is_valid) {
			return;
		}
		decltype(auto) vec = extract_holder(vec_holder, static_cast<std::pair<int, int>*>(nullptr));
		range.start = vec.first;
		range.end = vec.second;
	}

	inline cv::Range lua_to(lua_State* L, int index, cv::Range*, bool& is_valid) {
		cv::Range out;
		lua_to(L, index, out, is_valid);
		return out;
	}

	inline void lua_push(lua_State* L, const cv::Range& range) {
		lua_createtable(L, 2, 0);
		int i = 0;
		lua_push(L, range.start); lua_rawseti(L, -2, ++i);
		lua_push(L, range.end); lua_rawseti(L, -2, ++i);
	}

	inline void lua_to(lua_State* L, int index, std::vector<cv::Range>& ranges, bool& is_valid) {
		if (lua_isuserdata(L, index)) {
			ranges = *lua_userdata_to(L, index, static_cast<std::vector<cv::Range>*>(nullptr), is_valid);
			return;
		}

		auto vec_holder = lua_to(L, index, static_cast<std::vector<std::pair<int, int>>*>(nullptr), is_valid);
		if (!is_valid) {
			return;
		}
		decltype(auto) vec = extract_holder(vec_holder, static_cast<std::vector<std::pair<int, int>>*>(nullptr));

		ranges.resize(vec.size());

		auto i = 0;
		for (const auto& [start, end] : vec) {
			ranges[i].start = start;
			ranges[i].end = end;
			i++;
		}
	}


	// ================================
	// cv::Ptr
	// ================================

	template<typename T>
	inline cv::Ptr<T> lua_to(lua_State* L, int index, cv::Ptr<T>*, bool& is_valid) {
		return lua_to(L, index, static_cast<T*>(nullptr), is_valid);
	}

	template<typename T>
	inline void lua_push(lua_State* L, cv::Ptr<T>& ptr) {
		lua_push(L, std::shared_ptr<T>(ptr));
	}


	// ================================
	// cv::Point_
	// ================================

	template<typename _Tp>
	inline cv::Point_<_Tp> lua_to(lua_State* L, int index, cv::Point_<_Tp>*, bool& is_valid) {
		auto pt_holder = lua_to(L, index, static_cast<std::tuple<_Tp, _Tp>*>(nullptr), is_valid);
		if (!is_valid) {
			return cv::Point_<_Tp>();
		}
		decltype(auto) pt = extract_holder(pt_holder, static_cast<std::tuple<_Tp, _Tp>*>(nullptr));

		return cv::Point_<_Tp>(std::get<0>(pt), std::get<1>(pt));
	}

	template<typename _Tp>
	inline void lua_push(lua_State* L, const cv::Point_<_Tp>& pt) {
		lua_createtable(L, 2, 0);
		int i = 0;
		lua_push(L, pt.x); lua_rawseti(L, -2, ++i);
		lua_push(L, pt.y); lua_rawseti(L, -2, ++i);
	}


	// ================================
	// cv::Point3_
	// ================================

	template<typename _Tp>
	inline cv::Point3_<_Tp> lua_to(lua_State* L, int index, cv::Point3_<_Tp>*, bool& is_valid) {
		auto pt_holder = lua_to(L, index, static_cast<std::tuple<_Tp, _Tp, _Tp>*>(nullptr), is_valid);
		if (!is_valid) {
			return cv::Point3_<_Tp>();
		}
		decltype(auto) pt = extract_holder(pt_holder, static_cast<std::tuple<_Tp, _Tp, _Tp>*>(nullptr));

		return cv::Point3_<_Tp>(std::get<0>(pt), std::get<1>(pt), std::get<2>(pt));
	}

	template<typename _Tp>
	inline void lua_push(lua_State* L, const cv::Point3_<_Tp>& pt) {
		lua_createtable(L, 3, 0);
		int i = 0;
		lua_push(L, pt.x); lua_rawseti(L, -2, ++i);
		lua_push(L, pt.y); lua_rawseti(L, -2, ++i);
		lua_push(L, pt.z); lua_rawseti(L, -2, ++i);
	}


	// ================================
	// cv::Rect_
	// ================================

	template<typename _Tp>
	inline cv::Rect_<_Tp> lua_to(lua_State* L, int index, cv::Rect_<_Tp>*, bool& is_valid) {
		auto rect_holder = lua_to(L, index, static_cast<std::tuple<_Tp, _Tp, _Tp, _Tp>*>(nullptr), is_valid);
		if (!is_valid) {
			return cv::Rect_<_Tp>();
		}
		decltype(auto) rect = extract_holder(rect_holder, static_cast<std::tuple<_Tp, _Tp, _Tp, _Tp>*>(nullptr));

		return cv::Rect_<_Tp>(std::get<0>(rect), std::get<1>(rect), std::get<2>(rect), std::get<3>(rect));
	}

	template<typename _Tp>
	inline void lua_push(lua_State* L, const cv::Rect_<_Tp>& rect) {
		lua_createtable(L, 4, 0);
		int i = 0;
		lua_push(L, rect.x); lua_rawseti(L, -2, ++i);
		lua_push(L, rect.y); lua_rawseti(L, -2, ++i);
		lua_push(L, rect.width); lua_rawseti(L, -2, ++i);
		lua_push(L, rect.height); lua_rawseti(L, -2, ++i);
	}


	// ================================
	// cv::Scalar_
	// ================================

	template<typename _Tp>
	inline cv::Scalar_<_Tp> lua_to(lua_State* L, int index, cv::Scalar_<_Tp>*, bool& is_valid) {
		auto v = lua_to(L, index, static_cast<_Tp*>(nullptr), is_valid);
		if (is_valid) {
			return cv::Scalar_<_Tp>(v);
		}

		auto vec_holder = lua_to(L, index, static_cast<std::vector<_Tp>*>(nullptr), is_valid, 4, true);
		if (!is_valid) {
			return cv::Scalar_<_Tp>();
		}
		decltype(auto) vec = extract_holder(vec_holder, static_cast<std::vector<_Tp>*>(nullptr));

		switch (vec.size()) {
		case 1:
			return cv::Scalar_<_Tp>(vec.at(0));
		case 2:
			return cv::Scalar_<_Tp>(vec.at(0), vec.at(1));
		case 3:
			return cv::Scalar_<_Tp>(vec.at(0), vec.at(1), vec.at(2));
		case 4:
			return cv::Scalar_<_Tp>(vec.at(0), vec.at(1), vec.at(2), vec.at(3));
		default:
			return cv::Scalar_<_Tp>();
		}
	}

	template<typename _Tp>
	inline void lua_push(lua_State* L, const cv::Scalar_<_Tp>& s) {
		lua_createtable(L, 4, 0);
		for (int i = 0; i < 4; i++) {
			lua_push(L, s[i]);
			lua_rawseti(L, -2, i + 1);
		}
	}


	// ================================
	// cv::Size_
	// ================================

	template<typename _Tp>
	inline cv::Size_<_Tp> lua_to(lua_State* L, int index, cv::Size_<_Tp>*, bool& is_valid) {
		if (lua_isnil(L, index)) {
			return cv::Size_<_Tp>();
		}

		auto sz_holder = lua_to(L, index, static_cast<std::tuple<_Tp, _Tp>*>(nullptr), is_valid);
		if (!is_valid) {
			return cv::Size_<_Tp>();
		}

		decltype(auto) sz = extract_holder(sz_holder, static_cast<std::tuple<_Tp, _Tp>*>(nullptr));
		return cv::Size_<_Tp>(std::get<0>(sz), std::get<1>(sz));
	}

	template<typename _Tp>
	inline void lua_push(lua_State* L, const cv::Size_<_Tp>& rect) {
		lua_createtable(L, 2, 0);
		int i = 0;
		lua_push(L, rect.width); lua_rawseti(L, -2, ++i);
		lua_push(L, rect.height); lua_rawseti(L, -2, ++i);
	}


	// ================================
	// cv::Vec
	// ================================

	template<typename _Tp, int cn>
	inline cv::Vec<_Tp, cn>  lua_to(lua_State* L, int index, cv::Vec<_Tp, cn>*, bool& is_valid) {
		cv::Vec<_Tp, cn> out;

		auto vec_holder = lua_to(L, index, static_cast<std::vector<_Tp>*>(nullptr), is_valid, cn, true);
		if (!is_valid) {
			return out;
		}

		decltype(auto) vec = extract_holder(vec_holder, static_cast<std::vector<_Tp>*>(nullptr));

		const auto size = vec.size();

		for (int i = 0; i < size; i++) {
			out[i] = vec.at(i);
		}

		for (int i = size; i < cn; i++) {
			out[i] = static_cast<_Tp>(0);
		}

		return out;
	}

	template<typename _Tp, int cn>
	inline void lua_push(lua_State* L, const cv::Vec<_Tp, cn>& vec) {
		lua_createtable(L, cn, 0);
		for (int i = 0; i < cn; i++) {
			lua_push(L, vec[i]);
			lua_rawseti(L, -2, i + 1);
		}
	}

}

namespace LUA_MODULE_NAME {
	void register_extensions(lua_State* L);
}
